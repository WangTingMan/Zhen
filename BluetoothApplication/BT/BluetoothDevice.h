#pragma once
#include <string>

#include "BluetoothAddress.h"

class BluetoothDevice
{

public:

    BluetoothDevice();

    std::string const& GetName()const
    {
        return m_deviceName;
    }

    void SetNameInternal
        (
        std::string a_name
        )
    {
        m_deviceName = a_name;
    }

    void SetAddressInternal
        (
        BluetoothAddress a_addr
        )
    {
        m_address = a_addr;
    }

    BluetoothAddress GetAddress() const
    {
        return m_address;
    }

private:

    std::string m_deviceName;
    BluetoothAddress m_address;
};

