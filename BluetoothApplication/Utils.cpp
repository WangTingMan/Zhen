#include <windows.h>
#include "Utils.h"

#include <iomanip>
#include <sstream>

std::string UTF8_To_string( const std::string& str )
{
    int nwLen = MultiByteToWideChar( CP_UTF8, 0, str.c_str(), -1, NULL, 0 );
    wchar_t* pwBuf = new wchar_t[nwLen + 1];    //一定要加1，不然会出现尾巴 
    memset( pwBuf, 0, nwLen * 2 + 2 );
    MultiByteToWideChar( CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen );
    int nLen = WideCharToMultiByte( CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL );
    char* pBuf = new char[nLen + 1];
    memset( pBuf, 0, nLen + 1 );
    WideCharToMultiByte( CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL );

    std::string strRet = pBuf;

    delete[]pBuf;
    delete[]pwBuf;
    pBuf = NULL;
    pwBuf = NULL;

    return strRet;
}

std::string binary_to_hex_string
    (
    const char* a_buffer,
    uint16_t a_size
    )
{
    if (!a_buffer || a_size == 0)
    {
        return "";
    }

    std::stringstream ss;
    ss << std::hex << std::uppercase << std::noshowbase;

    for (uint16_t i = 0; i < a_size; ++i)
    {
        ss << std::setw(2) << std::setfill('0')
            << (a_buffer[i] & 0xFF) << ' ';
    }
    return ss.str();
}

