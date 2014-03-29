#include "stock_data.h"

using namespace std;

namespace Constants
{
    namespace Paths
    {
        const string input_file = BINARY_DIR "/input.txt";
        const string output_file = BINARY_DIR "/output.txt";
    }
}

int main()
{
    std::ifstream input_file(Constants::Paths::input_file, std::ios::in | ios::binary);

    if (!input_file.is_open())
    {
        return -1;
    }

    boost::thread_group group_of_slave_threads;

    binary_reader::cProtectedMessageWriter protected_message_writer;

    deque<binary_reader::stock_data> messages;
    while (!input_file.eof())
    {
        messages.emplace_back(input_file);
        binary_reader::stock_data& message = messages[messages.size() - 1];
        group_of_slave_threads.create_thread(boost::bind( &binary_reader::cProtectedMessageWriter::write, &protected_message_writer, &message));
    }
    input_file.close();
    group_of_slave_threads.join_all();
}

