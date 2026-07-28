#pragma once
#include <BT/BluetoothAddress.h>
#include <BT/StackLayer/bluetooth_interface.h>

class BluetoothA2DPInterface : public bluetooth_interface
{

public:

    static BluetoothA2DPInterface& GetInterface();

    BluetoothA2DPInterface()
        : bluetooth_interface( bluetooth_interface_type::a2dp_source )
    {

    }

    virtual ~BluetoothA2DPInterface(){}

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

