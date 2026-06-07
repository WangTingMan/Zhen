#pragma once
#include <BT/BluetoothAddress.h>

class BluetoothAvrcpTargetInterface
{

public:

    static BluetoothAvrcpTargetInterface& GetInterface();

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

