#pragma once
#include <memory>

#include <Zhen/global.h>

#include "BT/ConnectableModule.h"
#include "BT/BluetoothAddress.h"
#include "BT/BluetoothCommonDefs.h"
#include "BT/StackLayer/BluetoothHidHostInterface.h"

namespace Bluetooth {

class BluetoothHidHost : public ConnectableModule
{

public:

    BluetoothHidHost();

    static std::shared_ptr<BluetoothHidHost> GetMoudle();

    int Init()override;

    virtual bool Connect
        (
        BluetoothAddress a_address
        );

    virtual bool Disconnect
        (
        BluetoothAddress a_address
        );

    void GetReport
        (
        BluetoothAddress a_address
        );

    virtual std::vector<BluetoothAddress> GetConnectedDevices();

    virtual ConnectionStatus GetConnectionState( BluetoothAddress a_address );

    virtual ConnectionStatus GetConnectionState()const;

    void GetConnectionStatus
        (
        BluetoothAddress& a_address,
        ConnectionStatus& a_status
        );

    boost_ns::signals2::connection ConnectToConnectionStatusChanged
        (
        std::function<void(BluetoothAddress, ConnectionStatus)> a_fun
        )
    {
        return m_connectionStatusChangedSignal.connect(a_fun);
    }

public:

    void HandleConnectionStatusChanged
        (
        BluetoothAddress a_address,
        ConnectionStatus a_status
        );

private:

    boost_ns::signals2::signal<void(BluetoothAddress, ConnectionStatus)> m_connectionStatusChangedSignal;
    BluetoothAddress m_address;
    ConnectionStatus m_status;
};

}
