#pragma once
#include <memory>

#include <Zhen/global.h>

#include "ConnectableModule.h"
#include "BluetoothAddress.h"
#include "BluetoothCommonDefs.h"

#include <BT/BluetoothHF.h>

namespace Bluetooth {

class BluetoothHFAG : public ConnectableModule
{

public:

    BluetoothHFAG();

    static std::shared_ptr<BluetoothHFAG> GetMoudle();

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

    virtual bool DialNewNumber( std::string a_number );

    void HandleCindCallback( BluetoothAddress a_address );

    void HandleCopsCallback( BluetoothAddress a_address );

    void HandleCnumCallback( BluetoothAddress a_address );

    void HandleClccCallback( BluetoothAddress a_address );

    void HandleDialCallback( BluetoothAddress a_address, std::string a_phone_number );

    void HandleConnectionStatusChangedFromStack
        (
        BluetoothAddress a_address,
        ConnectionStatus a_status
        );

    void PhoneStateChange();

private:

    BluetoothAddress m_address;
    ConnectionStatus m_status;
    std::vector<CallSession> m_callSessions;
};

}
