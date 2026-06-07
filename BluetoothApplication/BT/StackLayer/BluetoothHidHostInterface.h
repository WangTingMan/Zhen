#pragma once
#include <BT/BluetoothAddress.h>

class BluetoothHidHostInterface
{

public:

    static BluetoothHidHostInterface& GetInterface();

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

