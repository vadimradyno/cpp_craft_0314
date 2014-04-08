#include "stock_data.h"
#include <boost/thread.hpp>
#include <queue>
#include <set>

using namespace std;

namespace Constants
{
    namespace Paths
    {
        const string input_file = BINARY_DIR "/input.txt";
    }
}

class cMessageProcessor final
{
    typedef shared_ptr<binary_reader::stock_data> tStockDataPtr;
    typedef queue<tStockDataPtr> tMessages;
    typedef map<string, std::ofstream> tFiles;

public:
    cMessageProcessor()
        : m_run_threads(true)
    {
        for (int i = 0; i < ms_count_threads; ++i)
        {
             m_group_of_slave_threads.create_thread(boost::bind( &cMessageProcessor::write, this));
        }
    }

    ~cMessageProcessor()
    {
        clear();
    }

    void processMessage( std::ifstream& _in )
    {
        try
        {
            boost::mutex::scoped_lock lock_messages(m_wait_messages);
            m_messages.push(make_shared<binary_reader::stock_data>(_in));
        }
        catch (const char* )
        {
        }
        catch (...)
        {

        }
    }

    void stopAndWaitThread()
    {
        m_run_threads = false;
        m_group_of_slave_threads.join_all();
    }

    void clear()
    {
        for (auto& file : m_files)
        {
            file.second.close();
        }
        m_files.clear();
    }

private:
    void write()
    {
        while (true)
        {
            boost::mutex::scoped_lock lock_messages(m_wait_messages);

            if (!m_messages.empty())
            {
                tStockDataPtr message = m_messages.front();
                m_messages.pop() ;
                lock_messages.unlock();

                if (message == nullptr)
                {
                    continue;
                }

                boost::mutex::scoped_lock lock_files(m_wait_files);

                std::stringstream output_path;
                output_path << BINARY_DIR << "/" << message->getStockName() << ".txt";

                auto it = m_files.find(output_path.str());
                if (it == m_files.end())
                {
                    m_files.insert(make_pair<string, std::ofstream>(
                        output_path.str(), ofstream(output_path.str(), std::ios::out | std::ios::binary )
                        ));

                    it = m_files.find(output_path.str());
                }

                message->write( (*it).second );

            }
            else if(m_run_threads)
            {
                lock_messages.unlock();
                boost::this_thread::sleep(boost::posix_time::microsec(ms_sleep_thread_time));
            }
            else
            {
                return;
            }
        }
    }

private:
    tMessages           m_messages;
    boost::mutex        m_wait_messages;
    bool                m_run_threads;
    boost::thread_group m_group_of_slave_threads;
    boost::mutex        m_wait_files;
    tFiles              m_files;

    static const int    ms_count_threads     = 4;
    static const int    ms_sleep_thread_time = 1;
};

int main()
{
    std::ifstream input_file(Constants::Paths::input_file, std::ios::in | ios::binary);

    if (!input_file.is_open())
    {
        return -1;
    }

    cMessageProcessor message_processor;

    while (!input_file.eof())
    {
        message_processor.processMessage(input_file);
    }
    input_file.close();

    message_processor.stopAndWaitThread();
    message_processor.clear();

    return 0;
}

