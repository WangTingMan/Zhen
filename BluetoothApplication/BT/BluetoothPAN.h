#pragma once
#include "ConnectableModule.h"
#include "BluetoothAddress.h"
#include "BluetoothCommonDefs.h"

#include <memory>

namespace Bluetooth
{

class BluetoothPAN : public ConnectableModule
{

public:

    BluetoothPAN();

    static std::shared_ptr<BluetoothPAN> GetMoudle();

    virtual bool Connect
        (
        BluetoothAddress a_address
        );

    virtual bool Disconnect
        (
        BluetoothAddress a_address
        );

    virtual std::vector<BluetoothAddress> GetConnectedDevices();

    virtual ConnectionStatus GetConnectionState(BluetoothAddress a_address);

    virtual ConnectionStatus GetConnectionState()const;
};

}

