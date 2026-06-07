#pragma once
#include <memory>
#include <vector>

#include <BT/BluetoothAddress.h>
#include <BT/BluetoothCommonDefs.h>

#include <Zhen/global.h>

class BluetoothPhoneBookPCE
{

public:

    void SearchPSESdpRecord( BluetoothAddress a_address );

    void Connect( BluetoothAddress a_address );

    BluetoothPhoneBookPCE();

    void GetConnectionStatus
        (
        BluetoothAddress& a_address,
        ConnectionStatus& a_status
        );

    static BluetoothPhoneBookPCE& GetInstance();

public:

    boost_ns::signals2::connection ConnectionToConnectionStatusChanged
        (
        std::function<void()> a_fun
        )
    {
        return m_connectionStatusChangedSignal.connect( a_fun );
    }

private:

    void HandleSocketConnectionStatusChanged
        (
        int a_socketID,
        BluetoothAddress a_address,
        ConnectionStatus a_status
        );

    void ConnectionSignals();

private:

    boost_ns::signals2::connection m_sdpQueryFinishedConnection;
    boost_ns::signals2::connection m_connectionStatusConnection;
    boost_ns::signals2::signal<void()> m_connectionStatusChangedSignal;
    ConnectionStatus m_connection_status;
    int m_connection_id = 0;
};

