#include "logging.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <chrono>
#include <iomanip>
#include <shared_mutex>

#ifdef _MSC_VER
#include <windows.h>
#endif

#define LOGLINE_SIZE 256

class file_name_cacher
{

public:

    static file_name_cacher& get_instance();

    const char* extract_file_name_from_path( const char* path );

private:

    const char* find_file_name_from_path( const char* path );

    std::shared_mutex m_mutex;
    std::unordered_map<const char*, const char*> m_cached_file_names;
};

file_name_cacher& file_name_cacher::get_instance()
{
    static file_name_cacher instance;
    return instance;
}

const char* file_name_cacher::extract_file_name_from_path( const char* path )
{
    std::shared_lock<std::shared_mutex> sh_locker( m_mutex );
    auto it = m_cached_file_names.find( path );
    if( it != m_cached_file_names.end() )
    {
        return it->second;
    }

    sh_locker.unlock();

    const char* base_name = find_file_name_from_path( path );

    std::lock_guard<std::shared_mutex> locker( m_mutex );
    m_cached_file_names[path] = base_name;

    return base_name;
}

const char* file_name_cacher::find_file_name_from_path( const char* path )
{
    const char* file = path;
    const char* split = path;
    while( file && *( file++ ) != '\0' )
    {
        if( '/' == *file || '\\' == *file )
        {
            split = file + 1;
        }
    }
    if( !split )
    {
        split = "Unkown";
    }
    return split;
}

class LogControlBlock
{

public:

    static LogControlBlock& GetInstance();

    Zhen::LogCallBack GetLogCallback()
    {
        return m_callBack;
    }

    void SetLogCallback( Zhen::LogCallBack a_callback )
    {
        m_callBack = a_callback;
    }

    void SetLogFileName( std::string a_path, std::string a_fileName )
    {
        if( a_path.back() != '/' &&
            a_path.back() != '//' )
        {
            a_path.push_back( '/' );
        }
        std::lock_guard<std::shared_mutex> locker( m_mutex );
        m_path = std::move( a_path );
        m_fileName = std::move( a_fileName );
    }

    void RecordLogLine( std::string a_log );

private:

    LogControlBlock() {}

    std::shared_mutex m_mutex;
    Zhen::LogCallBack m_callBack;
    std::string m_path;
    std::string m_fileName;
    std::ofstream m_os;
};

LogControlBlock& LogControlBlock::GetInstance()
{
    static LogControlBlock instance;
    return instance;
}

void LogControlBlock::RecordLogLine( std::string a_log )
{
    std::lock_guard<std::shared_mutex> locker( m_mutex );
    if( !m_os.is_open() )
    {
        std::string file = m_path;
        file.append( m_fileName );
        m_os.open( file, std::ios::out | std::ios::trunc );
    }

    if( !m_os.is_open() )
    {
        return;
    }

    m_os << a_log << std::flush;
}

static std::string get_timestamp_string()
{
    std::chrono::system_clock::time_point now_ = std::chrono::system_clock::now();
    std::string stamp_str;

    std::time_t time_ = std::chrono::system_clock::to_time_t( now_ );
    std::chrono::milliseconds millisecs = std::chrono::duration_cast< std::chrono::milliseconds >
        ( now_ - std::chrono::system_clock::from_time_t( time_ ) );
    tm tm_;
#ifdef _MSC_VER
    localtime_s( &tm_, &time_ );
#else
    localtime_s( &time_, &tm_ );
#endif

    char buffer[100];
    snprintf( buffer, sizeof buffer,
        "[%02d-%02d %02d:%02d:%02d.%03d]", tm_.tm_mon + 1,
        tm_.tm_mday, tm_.tm_hour, tm_.tm_min, tm_.tm_sec,
        static_cast< int >( millisecs.count() ) );

    stamp_str.append( buffer );
    return stamp_str;
}

Zhen::LogRecorder::~LogRecorder()
{
    bool ret = true;
    if( m_loggerContent  )
    {
        if( LogControlBlock::GetInstance().GetLogCallback() )
        {
            std::string tag;
            if( m_loggerContent->m_tag )
            {
                tag.assign( m_loggerContent->m_tag );
            }

            ret = LogControlBlock::GetInstance().GetLogCallback()
                (
                m_loggerContent->m_fileName,
                m_loggerContent->m_lineNumber,
                m_loggerContent->m_level,
                tag,
                m_stringStream.str()
                );
        }

        if( ret )
        {
            const char* split = file_name_cacher::get_instance()
                .extract_file_name_from_path( m_loggerContent->m_fileName );

            const char* type_s = "?";
            switch( m_loggerContent->m_level )
            {
            case Zhen::LogLevel::Info:
                type_s = "I";
                break;
            case Zhen::LogLevel::Debug:
                type_s = "D";
                break;
            case Zhen::LogLevel::Warning:
                type_s = "W";
                break;
            case Zhen::LogLevel::Error:
                type_s = "E";
                break;
            case Zhen::LogLevel::Fatal:
                type_s = "F";
                break;
            default:
                break;
            }

            std::stringstream log_line;
            log_line << get_timestamp_string() << "[" << type_s << "][" << GetCurrentThreadID() << "][" << split <<
                ":" << m_loggerContent->m_lineNumber << "] " << m_stringStream.str() << std::endl;

            LogControlBlock::GetInstance().RecordLogLine( log_line.str() );
        }

        if( m_loggerContent->m_level == LogLevel::Fatal )
        {
            std::abort();
        }
    }
}

void Zhen::SetLogCallBack( LogCallBack a_callBack )
{
    LogControlBlock::GetInstance().SetLogCallback( a_callBack );
}

void Zhen::SetLogFileName( std::string a_path, std::string a_fileName )
{
    LogControlBlock::GetInstance().SetLogFileName( a_path, a_fileName );
}

uint64_t Zhen::GetCurrentThreadID()
{
#ifdef _WIN32
    static thread_local uint64_t tid = ::GetCurrentThreadId();
#endif
    return tid;
}

static inline std::string PringFormatLog( const char* a_format, va_list ap )
{
    if( !a_format || !*a_format )
    {
        return "";
    }

    char pBuffer[LOGLINE_SIZE];
    ( void )vsnprintf( pBuffer, LOGLINE_SIZE - 1, a_format, ap );
    return pBuffer;
}

void Zhen::Logger::Recorder( const char* msg, ... ) const
{
    if( m_isLogEater )
    {
        return;
    }

    std::string logStr;
    va_list ap;
    va_start( ap, msg );
    logStr = PringFormatLog( msg, ap );
    va_end( ap );

    LogRecorder recorder;
    std::unique_ptr<LogRecorder::LogContent> content;
    content = std::make_unique<LogRecorder::LogContent>();
    content->m_fileName = m_fileName;
    content->m_isLogEater = m_isLogEater;
    content->m_lineNumber = m_lineNumber;
    content->m_level = m_level;
    recorder.SetLogContent( std::move( content ) );
    recorder << logStr;
}

std::ostream& Zhen::operator<<( std::ostream& os, Zhen::BinaryBuffer a_buffer )
{
    os << binary_to_hex_string( a_buffer.buffer, a_buffer.size );
    return os;
}

std::string Zhen::binary_to_hex_string
    (
    const char* a_buffer,
    uint16_t a_size
    )
{
    if( !a_buffer || a_size == 0 )
    {
        return "";
    }

    std::stringstream ss;
    ss << std::hex << std::uppercase << std::noshowbase;

    for( uint16_t i = 0; i < a_size; ++i )
    {
        ss << std::setw( 2 ) << std::setfill( '0' )
            << ( a_buffer[i] & 0xFF ) << ' ';
    }
    return ss.str();
}
