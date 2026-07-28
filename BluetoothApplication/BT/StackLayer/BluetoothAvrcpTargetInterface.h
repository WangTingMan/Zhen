#pragma once
#include <BT/BluetoothAddress.h>
#include <BT/StackLayer/bluetooth_interface.h>

class BluetoothAvrcpTargetInterface : public bluetooth_interface
{

public:

    static BluetoothAvrcpTargetInterface& GetInterface();

    BluetoothAvrcpTargetInterface()
        : bluetooth_interface(bluetooth_interface_type::avrcp_target)
    {

    }

    virtual ~BluetoothAvrcpTargetInterface(){}

    virtual void Init() = 0;

    virtual bool Connect
        (
        BluetoothAddress a_address
        ) = 0;

    virtual bool disconnect
        (
        BluetoothAddress a_address
        ) = 0;

    virtual bool setActiveDevice
        (
        BluetoothAddress a_address
        ) = 0;

    virtual bool setSilenceDevice
        (
        BluetoothAddress a_address
        ) = 0;
};

