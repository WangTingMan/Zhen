#pragma once
#include <array>
#include <cstdint>
#include <string>

class BluetoothAddress
{

public:

    static constexpr unsigned int kLength = 6;

    uint8_t address[kLength];

    BluetoothAddress() = default;
    BluetoothAddress( const uint8_t( &addr )[6] );
    BluetoothAddress( std::array<uint8_t, 6> addr );

    bool operator<( const BluetoothAddress& rhs ) const
    {
        return ( std::memcmp( address, rhs.address, sizeof( address ) ) < 0 );
    }
    bool operator==( const BluetoothAddress& rhs ) const
    {
        return ( std::memcmp( address, rhs.address, sizeof( address ) ) == 0 );
    }
    bool operator>( const BluetoothAddress& rhs ) const { return ( rhs < *this ); }
    bool operator<=( const BluetoothAddress& rhs ) const { return !( *this > rhs ); }
    bool operator>=( const BluetoothAddress& rhs ) const { return !( *this < rhs ); }
    bool operator!=( const BluetoothAddress& rhs ) const { return !( *this == rhs ); }

    bool IsEmpty() const { return *this == kEmpty; }

    std::string ToString() const;

    // Converts |string| to RawAddress and places it in |to|. If |from| does
    // not represent a Bluetooth address, |to| is not modified and this function
    // returns false. Otherwise, it returns true.
    static bool FromString( const std::string& from, BluetoothAddress& to );

    // Copies |from| raw Bluetooth address octets to the local object.
    // Returns the number of copied octets - should be always RawAddress::kLength
    size_t FromOctets( const uint8_t* from );

    static const BluetoothAddress kEmpty;  // 00:00:00:00:00:00
    static const BluetoothAddress kAny;    // FF:FF:FF:FF:FF:FF

    static bool IsValidAddress( const std::string& address );
};

