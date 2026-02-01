#pragma once
#include <sstream>
#include <memory>
#include <functional>
#include <string>

#include <Zhen/global.h>

#ifndef LOG_TAG
#define LOG_TAG ""
#endif

namespace Zhen
{

enum class LogLevel
{
    Info,
    Debug,
    Warning,
    Error,
    Fatal
};

using LogCallBack = std::function<bool( const char* /*FILE*/, int/*line*/,
    LogLevel, std::string const&/*tag*/, std::string const& ) >;

LIBZHEN_API void SetLogCallBack( LogCallBack a_callBack );

/**
 * set log file name. Will create a log file with name | a_fileName |
 * into folder with name | a_path |.
 */
LIBZHEN_API void SetLogFileName( std::string a_path, std::string a_fileName );

LIBZHEN_API uint64_t GetCurrentThreadID();

LIBZHEN_API std::string binary_to_hex_string
    (
    const char* a_buffer,
    uint16_t a_size
    );

struct BinaryBuffer
{
    BinaryBuffer( const char* a_buffer, uint16_t a_size )
        : buffer( a_buffer )
        , size( a_size )
    {
    }

    BinaryBuffer() {}

    const char* buffer = nullptr;
    uint16_t size = 0u;
};

LIBZHEN_API std::ostream& operator<<( std::ostream& os, BinaryBuffer a_buffer );

class LIBZHEN_API LogRecorder
{

    friend class Logger;

public:

    struct LogContent
    {
        const char* m_fileName = nullptr;
        bool m_isLogEater = false;
        int m_lineNumber = 0;
        LogLevel m_level = LogLevel::Info;
        const char* m_tag = nullptr;
    };

    LogRecorder(){}

    template<typename Types>
    LogRecorder& operator<<( Types&& args )
    {
        if( m_loggerContent &&
            !m_loggerContent->m_isLogEater 
          )
        {
            m_stringStream << args;
        }
        return *this;
    }

    LogRecorder( LogRecorder&& a_right ) noexcept
    {
        m_loggerContent = std::move( a_right.m_loggerContent );
        m_stringStream.swap( a_right.m_stringStream );
    }

    LogRecorder& operator=( LogRecorder&& a_right ) noexcept
    {
        m_loggerContent = std::move( a_right.m_loggerContent );
        m_stringStream.swap( a_right.m_stringStream );
        return *this;
    }

    ~LogRecorder();

private:

    void SetLogContent( std::unique_ptr<LogContent>&& a_content )
    {
        m_loggerContent = std::move( a_content );
    }

    std::stringstream m_stringStream;
    std::unique_ptr<LogContent> m_loggerContent;
};

class LIBZHEN_API Logger
{

public:

    Logger
        (
        const char* a_fileName,
        int         a_lineNumber,
        bool        a_isLogEater,
        LogLevel    a_level,
        const char* a_tag = nullptr
        )
        : m_fileName( a_fileName )
        , m_isLogEater( a_isLogEater )
        , m_lineNumber( a_lineNumber )
        , m_level( a_level )
        , m_tag( a_tag )
    {

    }

    LogRecorder Recorder()
    {
        LogRecorder recorder;
        std::unique_ptr<LogRecorder::LogContent> content;
        content = std::make_unique<LogRecorder::LogContent>();
        content->m_fileName = m_fileName;
        content->m_isLogEater = m_isLogEater;
        content->m_lineNumber = m_lineNumber;
        content->m_level = m_level;
        content->m_tag = m_tag;
        recorder.SetLogContent( std::move( content ) );
        return recorder;
    }

    void Recorder( const char* msg, ... ) const;

private:

    const char* m_fileName = nullptr;
    bool m_isLogEater = false;
    int m_lineNumber = 0;
    LogLevel m_level = LogLevel::Fatal;
    const char* m_tag = nullptr;
};

}

#define LogInfo Zhen::Logger( __FILE__, __LINE__, false, Zhen::LogLevel::Info, LOG_TAG ).Recorder
#define LogDebug Zhen::Logger( __FILE__, __LINE__, false, Zhen::LogLevel::Debug, LOG_TAG ).Recorder
#define LogWarning Zhen::Logger( __FILE__, __LINE__, false, Zhen::LogLevel::Warning, LOG_TAG ).Recorder
#define LogError Zhen::Logger( __FILE__, __LINE__, false, Zhen::LogLevel::Error, LOG_TAG ).Recorder
#define LogDatal Zhen::Logger( __FILE__, __LINE__, false, Zhen::LogLevel::Fatal, LOG_TAG ).Recorder

#define NoLogMarco while (false) Zhen::Logger( __FILE__, __LINE__, true,  Zhen::LogLevel::Info, LOG_TAG ).Recorder

