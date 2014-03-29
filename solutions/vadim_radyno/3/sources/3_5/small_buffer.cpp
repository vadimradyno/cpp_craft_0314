#include "market_message.h"
#include <string>
#include <deque>
#include <set>
#include <map>
#include <utility>
#include <algorithm>
#include <iostream>
#include <boost/thread.hpp>
#include <vector>


using namespace std;


namespace Constants
{
    namespace Paths
    {
        const string input_file_test = BINARY_DIR "/input.txt";
        const string input_file = BINARY_DIR "/input_";
        const string output_file = BINARY_DIR "/output.txt";
    }

    const int count_files = 999;
}

typedef set<boost::uint32_t> tSeconds;

struct sMessagesAttributies
{
    sMessagesAttributies(const boost::uint32_t _type)
        : m_type(_type)
        , m_count(0)
    { }

    boost::uint32_t  m_type;
    boost::uint32_t  m_count;
    tSeconds         m_seconds;
};


typedef deque<sMessagesAttributies> tAttributies;


class cMessagesReader final
{
public:
    void readMessages()
    {
        boost::thread_group group_of_slave_threads;

        for (int i = 1; i <= Constants::count_files; ++i)
        {
            group_of_slave_threads.create_thread( boost::bind(&cMessagesReader::readMessage, this, i));
        }

        group_of_slave_threads.join_all();
    }

    void write()
    {
        std::ofstream output_file(Constants::Paths::output_file, std::ios::out | std::ios::binary);
        
        for (sMessagesAttributies& attr : m_attrs)
        {
            output_file.write(reinterpret_cast<char*>(&(attr.m_type)), sizeof(attr.m_type));
            
            double mean = static_cast<double>(attr.m_count) / static_cast<double>(attr.m_seconds.size());
            output_file.write(reinterpret_cast<char*>(&(mean)), sizeof(mean));
        }
        
        output_file.close();
    }

private:
    void readMessage(const int _message_index)
    {
        const string& path_to_message = getPathToFileByIndex(_message_index);

        std::ifstream input_file(path_to_message, std::ios::in | ios::binary);

        if (!input_file.is_open())
        {
            return;
        }

        while (!input_file.eof())
        {
            binary_reader::market_message message(input_file);

            if (!message.isValidType())
            {
                continue;
            }

            auto predicat = [&message](const sMessagesAttributies& attr)->bool
            {
                return message.type() == attr.m_type;
            };

            sMessagesAttributies* message_attribute = nullptr;

            boost::mutex::scoped_lock lock(m_wait);

            tAttributies::iterator it = find_if(m_attrs.begin(), m_attrs.end(), predicat);

            if (it == m_attrs.end())
            {
                m_attrs.emplace_back(message.type());
                message_attribute = &(m_attrs[m_attrs.size() - 1]);
            }
            else
            {
                it = find_if(m_attrs.begin(), m_attrs.end(), predicat);
                message_attribute = &(*it);
            }

            (*message_attribute).m_seconds.insert(message.time());
            ++(*message_attribute).m_count;
        }

        input_file.close();
    }

    string getPathToFileByIndex(const int _message_index)
    {
        std::stringstream file_name;
        file_name << Constants::Paths::input_file;

        if (_message_index < 10)
        {
            file_name << "00";
        }
        else if (_message_index < 100)
        {
            file_name << "0";
        }

        file_name << _message_index << ".txt";

        return file_name.str();
    }

private:
    boost::mutex m_wait;
    tAttributies m_attrs;
};


int main()
{
    cMessagesReader messages_reader;

    messages_reader.readMessages();
    messages_reader.write();

    return 0;
}

