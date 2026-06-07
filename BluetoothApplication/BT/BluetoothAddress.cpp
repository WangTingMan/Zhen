#include "BluetoothAddress.h"

#include <stdint.h>
#include <algorithm>
#include <vector>

static_assert( sizeof( BluetoothAddress ) == 6, "RawAddress must be 6 bytes long!" );

const BluetoothAddress BluetoothAddress::kAny{ {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} };
const BluetoothAddress BluetoothAddress::kEmpty{ {0x00, 0x00, 0x00, 0x00, 0x00, 0x00} };

BluetoothAddress::BluetoothAddress( const uint8_t( &addr )[6] )
{
    std::copy( addr, addr + kLength, address );
};

BluetoothAddress::BluetoothAddress( std::array<uint8_t, 6> addr_container )
{
    const uint8_t* addr = addr_container.data();
    std::copy( addr, addr + kLength, address );
}

std::string BluetoothAddress::ToString() const
{
    char buffer[100];
    sprintf_s( buffer, "%02X:%02X:%02X:%02X:%02X:%02X", address[0],
                              address[1], address[2], address[3], address[4],
                              address[5] );
    return buffer;
}

bool BluetoothAddress::FromString( const std::string& from, BluetoothAddress& to )
{
    BluetoothAddress new_addr;
    if( from.length() != 17 ) return false;

    std::vector<std::string> byte_tokens;
    std::string temp;
    for( auto ele : from )
    {
        if( ':' != ele )
        {
            temp.push_back( ele );
        }
        else
        {
            if( !temp.empty() )
            {
                byte_tokens.push_back( temp );
                temp.clear();
            }
        }
    }
    byte_tokens.push_back( temp );
    temp.clear();

    if( byte_tokens.size() != 6 ) return false;

    for( int i = 0; i < 6; i++ )
    {
        const auto& token = byte_tokens[i];

        if( token.length() != 2 ) return false;

        char* temp = nullptr;
        new_addr.address[i] = strtol( token.c_str(), &temp, 16 );
        if( *temp != '\0' ) return false;
    }

    to = new_addr;
    return true;
}

size_t BluetoothAddress::FromOctets( const uint8_t* from )
{
    std::copy( from, from + kLength, address );
    return kLength;
};

bool BluetoothAddress::IsValidAddress( const std::string& address )
{
    BluetoothAddress tmp;
    return BluetoothAddress::FromString( address, tmp );
}
