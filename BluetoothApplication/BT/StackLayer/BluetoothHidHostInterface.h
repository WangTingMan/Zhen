#pragma once
#include <BT/BluetoothAddress.h>
#include <BT/StackLayer/bluetooth_interface.h>

class BluetoothHidHostInterface : public bluetooth_interface
{

public:

    static BluetoothHidHostInterface& GetInterface();

    BluetoothHidHostInterface()
        : bluetooth_interface( bluetooth_interface_type::hid_host )
    {

    }

    virtual ~BluetoothHidHostInterface(){}

    virtual void Init() = 0;

    virtual bool Connect
        (
        BluetoothAddress a_address
        ) = 0;

    virtual bool disconnect
        (
        BluetoothAddress a_address
        ) = 0;

    virtual void GetReport
        (
        BluetoothAddress a_address
        ) = 0;

};

