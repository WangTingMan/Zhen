#pragma once
#include <BT/BluetoothAddress.h>
#include <BT/StackLayer/bluetooth_interface.h>

#include <cstdint>
#include <string>
#include <vector>

class BluetoothRfcommInterface : public bluetooth_interface
{

public:

    static BluetoothRfcommInterface& GetInterface();

    BluetoothRfcommInterface()
        : bluetooth_interface( bluetooth_interface_type::rfcomm )
    {

    }

    virtual ~BluetoothRfcommInterface(){}

    virtual void Init() = 0;

    virtual bool Connect
        (
        BluetoothAddress a_address,
        std::string a_service_name,
        int a_channel
        ) = 0;

    virtual bool disconnect
        (
        BluetoothAddress a_address
        ) = 0;

    /**
     * Try to listen a rfcomm socket as a server.
     * a_service_name the rfcomm service's name
     * a_service_uuid the rfcomm service's UUID
     * a_channel the rfcomm service's port.
     * Warning: If the port specified by a_channel already in use,
     * then will return false.
     */
    virtual bool listen
        (
        std::string const& a_service_name,
        std::vector<uint8_t> a_service_uuid,
        int a_id,
        int a_channel = 0
        ) = 0;

    virtual void custom_test() = 0;
};

