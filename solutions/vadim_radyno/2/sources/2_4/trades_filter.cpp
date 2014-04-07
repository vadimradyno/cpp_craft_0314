#include <iosfwd>
#include <string>

#include "market_message.h"
#include <xutility>
#include <iostream>

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

    std::ofstream output_file(Constants::Paths::output_file, std::ios::out | std::ios::binary);
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

