#include "BluetoothPAN.h"
#include "Adaptor.h"
#include "BT/StackLayer/BluetoothPANInterface.h"

#define BluetoothPAN_MODULE "BluetoothPAN"

namespace Bluetooth
{

    BluetoothPAN::BluetoothPAN()
    {
        SetModuleName(BluetoothPAN_MODULE);
    }

    std::shared_ptr<BluetoothPAN> BluetoothPAN::GetMoudle()
    {
        auto hf = Adaptor::GetInstance().FindModule(BluetoothPAN_MODULE);
        std::shared_ptr< BluetoothPAN > hfModule = std::dynamic_pointer_cast<BluetoothPAN>(hf);
        return hfModule;
    }

    bool BluetoothPAN::Connect
        (
        BluetoothAddress a_address
        )
    {
        return BluetoothPANInterface::GetInterface().Connect(a_address);
    }

    bool BluetoothPAN::Disconnect
        (
        BluetoothAddress a_address
        )
    {
        return BluetoothPANInterface::GetInterface().disconnect(a_address);
    }

    std::vector<BluetoothAddress> BluetoothPAN::GetConnectedDevices()
    {
        std::vector<BluetoothAddress> devs;
        return devs;
    }

    ConnectionStatus BluetoothPAN::GetConnectionState(BluetoothAddress a_address)
    {
        ConnectionStatus status = ConnectionStatus::ServiceDisconnected;
        return status;
    }

    ConnectionStatus BluetoothPAN::GetConnectionState()const
    {
        ConnectionStatus status = ConnectionStatus::ServiceDisconnected;
        return status;
    }

}

