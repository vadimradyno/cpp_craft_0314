#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;
namespace Constants
{
    namespace Paths
    {
        const string input_file = SOURCE_DIR "/sources/1_1/input.txt";
        const string output_file = SOURCE_DIR "/sources/1_1/output.txt";
    }
    const string ignore_simbols = " -\\";

    namespace Results
    {
        const string good = "YES";
        const string bad  = "NO";
    }
}



bool isIgnoreSimbol(const string& _simbol)
{
    return (Constants::ignore_simbols.npos != Constants::ignore_simbols.find(_simbol));
}


void removeIgnoreSimbols(string& _str)
{
    for (string::const_iterator it = Constants::ignore_simbols.cbegin(); it != Constants::ignore_simbols.cend(); ++it)
    {
        _str.erase(std::remove(_str.begin(), _str.end(), (*it)));
    }
}


bool isValidKeyForText(const string& _key, const string& _text)
{
    int count_rounds = 0;

    for (string::const_iterator key_it = _key.begin(); key_it != _key.end(); ++key_it)
    {
        for (string::const_iterator text_it = _text.begin(); text_it != _text.end(); ++text_it)
        {
            size_t count_identical_simbols = 0;
            if ((*key_it) == (*text_it))
            {
                ++count_identical_simbols;

                if (count_identical_simbols == _key.size())
                {
                    return true;
                }
            }
        }
    }

    return false;
}


void upperCase(string& _text) {
    for(string::iterator it = _text.begin(); it != _text.end(); ++it)
    {
        (*it) = toupper((*it));
    }
}


int main( int argc, char* argv[] )
{
    ifstream input_file( Constants::Paths::input_file );

    if ( !input_file.eof() )
    {
        string first_line;
        getline( input_file, first_line );

        removeIgnoreSimbols(first_line);
        upperCase(first_line);

        while ( !input_file.eof() )
        {
            string key_value;
            std::getline( input_file, key_value );

            removeIgnoreSimbols(key_value);
            upperCase(key_value);

            const string& valid_key = isValidKeyForText(key_value, first_line)  ? Constants::Results::good 
                                                                                : Constants::Results::bad;

            std::cout << valid_key << std::endl;
        }

    }


    input_file.close();
}
