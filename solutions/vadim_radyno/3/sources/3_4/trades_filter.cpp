// #include <iosfwd>
// #include <string>

#include "market_message.h"
// #include <xutility>
// #include <iostream>
// #include <strstream>


#include <iostream>
#include <fstream>
#include <string>
#include <deque>
#include <sstream>
#include <set>
#include <boost/thread.hpp>

using namespace std;

namespace Constants
{
    namespace Paths
    {
        const string input_file = BINARY_DIR "/input_";
        const string output_file = BINARY_DIR "/output_";
    }

    const int count_files = 999;
}

class cProcessorMessage final
{
public:
    cProcessorMessage (const int _message_index)
        : m_message_index(_message_index)
    {}

    void process()
    {
        const string& input_file_name = getPathToFileByIndex(Constants::Paths::input_file);
        std::ifstream input_file(input_file_name, std::ios::in | ios::binary);

        if (!input_file.is_open())
        {
            return;
        }

        const string& output_file_name = getPathToFileByIndex(Constants::Paths::output_file);
        std::ofstream output_file(output_file_name, std::ios::out | std::ios::binary);

        boost::int32_t max_time = 0;

        while (!input_file.eof())
        {
            binary_reader::market_message message(input_file);

            if (message.isValidTime(max_time) && message.isValidType())
            {
                max_time = std::max<boost::int32_t>(message.time(), max_time);
                message.write(output_file);
            }
        }

        output_file.close();
        input_file.close();
    }

private:
    string getPathToFileByIndex(const string& _begin_path)
    {
        std::stringstream file_name;
        file_name << _begin_path;

        if (m_message_index < 10)
        {
            file_name << "00";
        }
        else if (m_message_index < 100)
        {
            file_name << "0";
        }

        file_name << m_message_index << ".txt";

        return file_name.str();
    }

private:
    const int m_message_index;
};

typedef vector<cProcessorMessage> tProcessorMessages;

int main()
{
    boost::thread_group group_of_slave_threads;

    tProcessorMessages processor_messages;
    processor_messages.reserve(Constants::count_files);

    for( size_t i = 1; i <= Constants::count_files; ++i )
    {
        processor_messages.emplace_back(i);
        group_of_slave_threads.create_thread( boost::bind( &cProcessorMessage::process, &processor_messages[i - 1] ) );
    }

    group_of_slave_threads.join_all();

    return 0;
}

