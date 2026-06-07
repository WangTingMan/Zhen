#pragma once
#include <memory>
#include <string>

#include <Zhen/global.h>

#include "BT/ConnectableModule.h"
#include "BT/BluetoothAddress.h"
#include "BT/BluetoothCommonDefs.h"

namespace Bluetooth {

struct LocalRfcommListenPort
{
    std::vector<uint8_t> uuid;
    std::string name;
    int port;
    int id = 0;
};

class BluetoothRfcomm : public ConnectableModule
{

public:

    BluetoothRfcomm();

    static std::shared_ptr<BluetoothRfcomm> GetMoudle();

    int Init()override;

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

    bool Listen
        (
        std::string const& a_service_name,
        std::vector<uint8_t> a_service_uuid,
        int a_channel = 0
        );

public:

    void HandleListenPortChanged(int a_user_id, int a_port);

private:

    std::vector<std::shared_ptr<LocalRfcommListenPort>> m_local_listen_port;
};

}
