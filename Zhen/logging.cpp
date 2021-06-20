#include "logging.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>

#define LOGLINE_SIZE 256

Zhen::LogCallBack s_callBack;

Zhen::LogRecorder::~LogRecorder()
{
    bool ret = false;
    if( m_loggerContent  )
    {
        if( s_callBack )
        {
            std::string tag;
            if( m_loggerContent->m_tag )
            {
                tag.assign( m_loggerContent->m_tag );
            }

            ret = s_callBack
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
            const char* file = m_loggerContent->m_fileName;
            const char* split = m_loggerContent->m_fileName;
            while( file && *( file++ ) != '\0' )
            {
                if( '/' == *file ||
                    '\\' == *file )
                {
                    split = file + 1;
                }
            }
            if( !split )
            {
                split = "Unkown";
            }

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

            std::cout << "[" << type_s << "][" << split << ":" << m_loggerContent->m_lineNumber << "] " << m_stringStream.str() << std::endl;
        }

        if( m_loggerContent->m_level == LogLevel::Fatal )
        {
            std::abort();
        }
    }
}

void Zhen::SetLogCallBack( LogCallBack a_callBack )
{
    s_callBack = a_callBack;
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

