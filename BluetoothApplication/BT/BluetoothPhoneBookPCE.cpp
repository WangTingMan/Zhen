#include "BluetoothPhoneBookPCE.h"
#include "Adaptor.h"

#include <Zhen/ExecutbleEvent.h>
#include <Zhen/PageManager.h>

BluetoothPhoneBookPCE::BluetoothPhoneBookPCE()
{
    std::shared_ptr< ExecutbleEvent > event = std::make_shared<ExecutbleEvent>();
    event->SetExecutableFunction(
        std::bind( &BluetoothPhoneBookPCE::ConnectionSignals, this ) );
    PageManager::GetInstance().PostEvent( event );
}

BluetoothPhoneBookPCE& BluetoothPhoneBookPCE::GetInstance()
{
    static BluetoothPhoneBookPCE instance;
    return instance;
}

void BluetoothPhoneBookPCE::SearchPSESdpRecord( BluetoothAddress a_address )
{
    std::vector<uint8_t> uuid;
    uuid.push_back( 0x2F );
    uuid.push_back( 0x11 );
}

void BluetoothPhoneBookPCE::Connect( BluetoothAddress a_address )
{
}

void BluetoothPhoneBookPCE::HandleSocketConnectionStatusChanged
    (
    int a_socketID,
    BluetoothAddress a_address,
    ConnectionStatus  a_status
    )
{
    m_connectionStatusChangedSignal();
}

void BluetoothPhoneBookPCE::GetConnectionStatus
    (
    BluetoothAddress& a_address,
    ConnectionStatus& a_status
    )
{
}

void BluetoothPhoneBookPCE::ConnectionSignals()
{
}
