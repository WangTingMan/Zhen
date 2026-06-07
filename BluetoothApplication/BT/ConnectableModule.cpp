#include "ConnectableModule.h"

namespace Bluetooth
{

ConnectableModule::ConnectableModule()
{

}

bool ConnectableModule::Connect
    (
    BluetoothAddress a_address
    )
{
    return false;
}

bool ConnectableModule::Disconnect
    (
    BluetoothAddress a_address
    )
{
    return false;
}

std::vector<BluetoothAddress> ConnectableModule::GetConnectedDevices()
{
    std::vector<BluetoothAddress> ret;
    return ret;
}

ConnectionStatus ConnectableModule::GetConnectionState( BluetoothAddress a_address )
{
    return ConnectionStatus::ServiceDisconnected;
}

ConnectionStatus ConnectableModule::GetConnectionState()const
{
    return ConnectionStatus::ServiceDisconnected;
}

}

