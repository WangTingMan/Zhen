#pragma once
#include <BT/BluetoothAddress.h>

class BluetoothA2DPSinkInterface
{

public:

    static BluetoothA2DPSinkInterface& GetInterface();

    virtual ~BluetoothA2DPSinkInterface(){}

    virtual void Init() = 0;

    virtual bool Connect
        (
        BluetoothAddress a_address
        ) = 0;

    virtual bool disconnect
        (
        BluetoothAddress a_address
        ) = 0;

    virtual void SetAudioFocus
        (
        bool a_grant
        ) = 0;
};

