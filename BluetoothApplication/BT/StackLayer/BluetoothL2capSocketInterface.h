#pragma once
#include <BT/BluetoothAddress.h>

class BluetoothL2capSocketInterface
{

public:

    static BluetoothL2capSocketInterface& GetInterface();

    virtual ~BluetoothL2capSocketInterface() {}

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

};

