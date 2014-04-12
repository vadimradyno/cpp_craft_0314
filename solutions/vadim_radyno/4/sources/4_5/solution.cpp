#include "solution.h"

task4_5::solution::solution( const data_type& data )
    : m_data(data)
    , m_max(getFirstValueFromData())
    , m_min(m_max)
{
    
    boost::thread_group group_of_slave_threads;
    int counter_thread = ms_count_thread;
    while(counter_thread--)
    {
        group_of_slave_threads.create_thread(boost::bind( &solution::find_min_max, this));
    }

    group_of_slave_threads.join_all();
}

int task4_5::solution::get_min() const
{
    return m_min;
}
int task4_5::solution::get_max() const
{
    return m_max;
}


int task4_5::solution::getFirstValueFromData() const
{
    for (auto data_elem : m_data)
    {
        for (int current_value : data_elem)
        {
            return current_value;
        }
    }

    return 0;
}


void task4_5::solution::find_min_max()
{
    while (true)
    {
        boost::mutex::scoped_lock lock_data(m_wait_data);
        if (m_data.empty())
        {
            return;
        }

        auto data = m_data.back();
        m_data.pop_back();
        lock_data.unlock();

        boost::mutex::scoped_lock lock_min_and_max(m_wait_min_max);
        for (const int current_value : data)
        {
            m_min = std::min(current_value, m_min);
            m_max = std::max(current_value, m_max);
        }
    }
}
