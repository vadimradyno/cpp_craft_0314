#include "market_message.h"
#include <string>
#include <deque>
#include <set>
#include <map>
#include <utility>
#include <algorithm>
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

int main()
{
    std::ifstream input_file(Constants::Paths::input_file, std::ios::in | ios::binary);

    if (!input_file.is_open())
    {
        return -1;
    }

    tAttributies attrs;
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

        tAttributies::iterator it = find_if(attrs.begin(), attrs.end(), predicat);

        if (it == attrs.end())
        {
            attrs.emplace_back(message.type());
        }

        it = find_if(attrs.begin(), attrs.end(), predicat);

        (*it).m_seconds.insert(message.time());
        ++(*it).m_count;
    }

    input_file.close();

    std::ofstream output_file(Constants::Paths::output_file, std::ios::out | std::ios::binary);

    for (sMessagesAttributies& attr : attrs)
    {
        output_file.write(reinterpret_cast<char*>(&(attr.m_type)), sizeof(attr.m_type));

        double mean = static_cast<double>(attr.m_count) / static_cast<double>(attr.m_seconds.size());
        output_file.write(reinterpret_cast<char*>(&(mean)), sizeof(mean));

        cout << attr.m_type << " " << mean << endl;
    }

    output_file.close();

}

