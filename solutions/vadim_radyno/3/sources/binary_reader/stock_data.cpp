#include "stock_data.h"

binary_reader::stock_data::stock_data( std::ifstream& _in )
    : m_price_(0)
    , m_vwap_(0)
    , m_volume_(0)
    , m_f1_(0)
    , m_t1_(0)
    , m_f2_(0)
    , m_f3_(0)
    , m_f4_(0)
{
    memset(m_stock_name_, '\0', ms_stock_name_max_size);
    readArray<char>(_in, m_stock_name_, ms_stock_name_max_size);
    readArray<char>(_in, m_date_time_, ms_data_time_size);

    m_price_ = readValue<double>(_in);
    m_vwap_ = readValue<double>(_in);
    m_volume_ = readValue<boost::uint32_t>(_in);
    m_f1_ = readValue<double>(_in);
    m_t1_ = readValue<double>(_in);
    m_f2_ = readValue<double>(_in);
    m_f3_ = readValue<double>(_in);
    m_f4_ = readValue<double>(_in);
}


binary_reader::stock_data::stock_data( const char* _stock_name,
                                      const char* _date_time,
                                      const double _price,
                                      const double _vwap,
                                      const boost::uint32_t _volume,
                                      const double _f1,
                                      const double _t1,
                                      const double _f2,
                                      const double _f3,
                                      const double _f4 )
    : m_price_(_price)
    , m_vwap_(_vwap)
    , m_volume_(_volume)
    , m_f1_(_f1)
    , m_t1_(_t1)
    , m_f2_(_f2)
    , m_f3_(_f3)
    , m_f4_(_f4)
{
    const boost::uint32_t date_time_size = std::min<boost::uint32_t>(static_cast<boost::uint32_t>(strlen(_date_time)), ms_data_time_size);
    strncpy(m_date_time_, _date_time, date_time_size);

    const boost::uint32_t stock_name_size = std::min<boost::uint32_t>(static_cast<boost::uint32_t>(strlen(_stock_name)), ms_stock_name_max_size);
    strncpy(m_stock_name_, _stock_name, stock_name_size);
}


binary_reader::stock_data::~stock_data()
{

}



void binary_reader::stock_data::write( std::ofstream& out )
{
    std::string new_stock_name(m_stock_name_);
    new_stock_name.resize(ms_new_stock_name_max_size);

    out.seekp(0, std::ios::end);

    out.write(new_stock_name.c_str(), ms_new_stock_name_max_size);

    const boost::uint32_t count_day = getCountDay();
    out.write(reinterpret_cast<const char*>(&count_day), sizeof(count_day));

    out.write(reinterpret_cast<char*>(&m_vwap_), sizeof(m_vwap_));

    out.write(reinterpret_cast<char*>(&m_volume_), sizeof(m_volume_));

    out.write(reinterpret_cast<char*>(&m_f2_), sizeof(m_f2_));
}


void binary_reader::stock_data::write_raw( std::ofstream& out )
{	
	// your code. Can be emty
	// this method is used for testing. It writes data to the binary file without convertion.
}


boost::uint32_t binary_reader::stock_data::getCountDay() const
{
    uint32_t year = 0, month = 0, day = 0;

    sscanf_s( m_date_time_, "%4d%2d%2d" ,&year ,&month ,&day );

    return (year - 1) * ms_count_days_in_year + (month - 1) * ms_count_days_im_month + day;
}


////////////////////////////////////////////////////////////////////////////////////

void binary_reader::cProtectedMessageWriter::write( stock_data* _stock_data )
{
    if (nullptr == _stock_data)
    {
        return;
    }

    const std::string& stock_name = _stock_data->getStockName();

    if (stock_name.empty())
    {
        return;
    }

    boost::mutex::scoped_lock lock_files(m_wait_files_locker);

    auto it = m_files_locker.find(stock_name);
    if (it == m_files_locker.end())
    {
        m_files_locker.insert(std::make_pair(stock_name, tMutexPtr(new boost::mutex())));
    }

    tMutexPtr& wait_file = m_files_locker[stock_name];

    lock_files.unlock();

    boost::mutex::scoped_lock lock_file(*wait_file.get());

    std::stringstream output_puth;
    output_puth << BINARY_DIR << "/" << stock_name << ".txt";

    std::ofstream output_file(output_puth.str(), std::ios::out | std::ios::binary | std::ios::app);

    _stock_data->write(output_file);

    output_file.close();
}
