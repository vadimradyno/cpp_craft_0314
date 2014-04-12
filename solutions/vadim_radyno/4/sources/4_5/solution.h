#ifndef _TASK4_5_SOLUTION_H_
#define _TASK4_5_SOLUTION_H_

#include <vector>
#include <boost/thread.hpp>

namespace task4_5
{
	typedef std::vector< std::vector< int > > data_type;

	class solution
	{
	public:
		explicit solution( const data_type& data );
		int get_min() const;
		int get_max() const;

    private:
        void find_min_max();
        int getFirstValueFromData() const;

    private:
        data_type m_data;
        int m_max;
        int m_min;
        boost::mutex m_wait_data;
        boost::mutex m_wait_min_max;

        static const int ms_count_thread = 4;
	};
}

#endif // _TASK4_5_SOLUTION_H_

