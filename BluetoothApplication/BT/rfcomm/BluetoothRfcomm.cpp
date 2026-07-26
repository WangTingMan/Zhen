#include "BluetoothRfcomm.h"
#include "BT/Adaptor.h"
#include "BT/StackLayer/BluetoothRfcommInterface.h"

#include <Zhen/PageManager.h>
#include <Zhen/ExecutbleEvent.h>
#include <Zhen/logging.h>

#define RFCOMM_MODULE "RFCOMM_MODULE"

namespace Bluetooth {

BluetoothRfcomm::BluetoothRfcomm()
{
    SetModuleName( RFCOMM_MODULE );
}

std::shared_ptr<BluetoothRfcomm> BluetoothRfcomm::GetMoudle()
{
    auto rfc = Adaptor::GetInstance().FindModule( RFCOMM_MODULE );
    std::shared_ptr<BluetoothRfcomm> module_ = std::dynamic_pointer_cast<BluetoothRfcomm>( rfc );
    return module_;
}

int BluetoothRfcomm::Init()
{
    BluetoothRfcommInterface::GetInterface().Init();

    std::string service_name = "SPP test port";
    std::vector<uint8_t> service_spp_uuid{0x00, 0x00, 0x11, 0x01, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00,
                0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb};
    int channel = 0;
    std::function<void()> fun = std::bind( &BluetoothRfcomm::Listen, GetMoudle(),
                                           service_name, service_spp_uuid, channel );
    auto executableEvent = std::make_shared<ExecutbleEvent>( fun );
    PageManager::GetInstance().PostEvent( executableEvent );

    return 0;
}

bool BluetoothRfcomm::Connect
    (
    BluetoothAddress a_address
    )
{
    return BluetoothRfcommInterface::GetInterface().Connect( a_address, "", 0 );
}

bool BluetoothRfcomm::Disconnect
    (
    BluetoothAddress a_address
    )
{
    return BluetoothRfcommInterface::GetInterface().disconnect( a_address );
}

std::vector<BluetoothAddress> BluetoothRfcomm::GetConnectedDevices()
{
    std::vector<BluetoothAddress> connected_devs;
    return connected_devs;
}

ConnectionStatus BluetoothRfcomm::GetConnectionState( BluetoothAddress a_address )
{
    return ConnectionStatus::ServiceDisconnected;
}

ConnectionStatus BluetoothRfcomm::GetConnectionState()const
{
    return ConnectionStatus::ServiceDisconnected;
}

bool BluetoothRfcomm::Listen
    (
    std::string const& a_service_name,
    std::vector<uint8_t> a_service_uuid,
    int a_channel
    )
{
    auto port = std::make_shared<LocalRfcommListenPort>();
    port->name = a_service_name;
    port->port = a_channel;
    port->uuid = a_service_uuid;
    port->id = 10;
    m_local_listen_port.push_back( port );

    return BluetoothRfcommInterface::GetInterface().listen( a_service_name, a_service_uuid, port->id, a_channel );
}

void BluetoothRfcomm::CustomTest()
{
    BluetoothRfcommInterface::GetInterface().custom_test();
}

void BluetoothRfcomm::HandleListenPortChanged( int a_user_id, int a_port )
{
    for( auto ele : m_local_listen_port )
    {
        if( ele->id == a_user_id )
        {
            ele->port = a_port;
            return;
        }
    }
}

}
