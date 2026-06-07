#pragma once
#include "AbstractModule.h"
#include "BluetoothAddress.h" 
#include "BluetoothCommonDefs.h"

#include <vector>

namespace Bluetooth
{

class ConnectableModule : public AbstractModule
{

public:

    ConnectableModule();

    virtual bool Connect
        (
        BluetoothAddress a_address
        );

    virtual bool Disconnect
        (
        BluetoothAddress a_address
        );

    virtual std::vector<BluetoothAddress> GetConnectedDevices();

    virtual ConnectionStatus GetConnectionState( BluetoothAddress a_address );

    virtual ConnectionStatus GetConnectionState()const;

protected:


};

}

