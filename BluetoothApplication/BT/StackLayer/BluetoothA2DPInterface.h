#pragma once
#include <BT/BluetoothAddress.h>

class BluetoothA2DPInterface
{

public:

    static BluetoothA2DPInterface& GetInterface();

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

