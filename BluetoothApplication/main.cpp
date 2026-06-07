#include <iostream>
#include <thread>
#include <chrono>
#include <filesystem>
#include <string>
#include <vector>
#include <fstream>

#include "BluetoothHomePage.h"
#include "Zhen/PageManager.h"
#include "Zhen/CommandHolder.h"
#include "Zhen/logging.h"
#include "Zhen/ExecutbleEvent.h"
#include "Zhen/KeyboardReader.h"

#include <windows.h>

//#define USE_GOOGLE_SOURCE

#ifdef USE_GOOGLE_SOURCE 
#pragma comment(lib,"libbluetoothinterface_ori.lib")
#else
#pragma comment(lib,"libbluetoothinterface.lib")
#endif

bool LogCallBackExt
    (
    const char* a_fileName,
    int a_lineNumber,
    Zhen::LogLevel a_level,
    std::string const& a_tag,
    std::string const& a_log
    );

void myInvalidParameterHandler( const wchar_t* expression,
          const wchar_t* function,
          const wchar_t* file,
          unsigned int line,
          uintptr_t pReserved )
{
    // function、file、line在Release下无效
    wprintf( L"Invalid parameter detected in function %s."
     L" File: %s Line: %d\n", function, file, line );
    wprintf( L"Expression: %s\n", expression );
    // 必须抛出异常，否则无法定位错误位置
    throw 1;
}
void myPurecallHandler( void )
{
    printf( "In _purecall_handler." );
    // 必须抛出异常，否则无法定位错误位置
    throw 1;
}

std::ofstream s_outFile;
#define LOG_FILE "D:/bluetooth/AndroidBTInWindows.log"

int main( int a_argc, char* a_argv[] )
{
    SetDllDirectoryA( "E:\\VCLAB\\component\\x64\\Debug" );
    KeyboardReader reader;
    reader.start();

    _invalid_parameter_handler oldHandler;
    _purecall_handler old_pure_handle;

    oldHandler = _set_invalid_parameter_handler( myInvalidParameterHandler );
    old_pure_handle = _set_purecall_handler( myPurecallHandler );

    s_outFile.open( LOG_FILE, std::ios::out | std::ios::trunc );
    Zhen::SetLogCallBack( std::bind( &LogCallBackExt, std::placeholders::_1, std::placeholders::_2,
                                     std::placeholders::_3, std::placeholders::_4, std::placeholders::_5 ) );

    LogDebug() << "Start to run.";
    std::shared_ptr<BasePage> page = std::make_shared<BluetoothHomePage>();
    PageManager::GetInstance().PushPage( std::move( page ) );

    PageManager::GetInstance().run();
    return 0;
}

bool LogCallBackExt
    (
    const char* a_fileName,
    int a_lineNumber,
    Zhen::LogLevel a_level,
    std::string const& a_tag,
    std::string const& a_log
    )
{
    const char* file = a_fileName;
    const char* split = a_fileName;
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
    switch( a_level )
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

    SYSTEMTIME sys;
    GetLocalTime( &sys );
    std::string timeStr;
    char buffer[100];
    sprintf_s( buffer, "[%02d-%02d %02d:%02d:%02d.%03d] ", sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds );

    std::stringstream ss;
    ss << buffer << "[" << type_s << "][" << split << ":" << a_lineNumber << "] " << a_log << std::endl;
    s_outFile << ss.str();
    s_outFile.flush();
    if( s_outFile.is_open() )
    {
        ss.clear();
    }
    return false;
}
