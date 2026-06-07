#include "BluetoothA2DPSource.h"
#include "BT/StackLayer/BluetoothA2DPInterface.h"
#include "BT/StackLayer/BluetoothAvrcpTargetInterface.h"

#include "../Adaptor.h"

#include <Zhen/PageManager.h>
#include <Zhen/ExecutbleEvent.h>
#include <Zhen/logging.h>

#define A2DP_SOURCE_MODULE "A2DP_SOURCE_MODULE"

class BluetoothA2DPSourceImpl
{

public:

    void HandleRequestAudioEndSession()
    {
        m_streamStatus = Bluetooth::BluetoothA2DPSource::Stopped;
        m_streamStatusChangedSignal();
    }

public:

    boost_ns::signals2::signal<void()> m_streamStatusChangedSignal;

    Bluetooth::BluetoothA2DPSource::StreamStatus m_streamStatus = Bluetooth::BluetoothA2DPSource::Stopped;
    ConnectionStatus m_connectionStatus = ConnectionStatus::ServiceDisconnected;
    BluetoothAddress m_remoteAddress;
};

namespace Bluetooth
{

BluetoothA2DPSource::BluetoothA2DPSource()
{
    SetModuleName( A2DP_SOURCE_MODULE );
}

bool BluetoothA2DPSource::handle_btav_mandatory_codec_preferred_callback(
    const BluetoothAddress& bd_addr )
{
    return true;
}

void BluetoothA2DPSource::HandleConnectStatusChanged
    (
    BluetoothAddress a_address,
    ConnectionStatus a_status
    )
{
    m_impl->m_remoteAddress = a_address;
    m_impl->m_connectionStatus = a_status;
    m_connectionStatusChangedSignal( a_address, a_status );
}

std::shared_ptr<BluetoothA2DPSource> BluetoothA2DPSource::GetMoudle()
{
    auto hf = Adaptor::GetInstance().FindModule( A2DP_SOURCE_MODULE );
    std::shared_ptr< BluetoothA2DPSource > hfModule = std::dynamic_pointer_cast< BluetoothA2DPSource >( hf );
    return hfModule;
}

int BluetoothA2DPSource::Init()
{
    if( !m_impl )
    {
        m_impl = std::make_shared<BluetoothA2DPSourceImpl>();
        BluetoothA2DPInterface::GetInterface().Init();
        BluetoothAvrcpTargetInterface::GetInterface().Init();
    }
    return 0;
}

bool BluetoothA2DPSource::Connect
    (
    BluetoothAddress a_address
    )
{
    return BluetoothA2DPInterface::GetInterface().Connect(a_address);
}

bool BluetoothA2DPSource::disconnect
    (
    BluetoothAddress a_address
    )
{
    return BluetoothA2DPInterface::GetInterface().disconnect(a_address);
}

bool BluetoothA2DPSource::setActiveDevice
    (
    BluetoothAddress a_address
    )
{
    bool ret =  BluetoothA2DPInterface::GetInterface().setActiveDevice(a_address);
    if( ret )
    {
        m_activeDeviceChangedSignal( a_address );
    }
    return ret;
}

bool BluetoothA2DPSource::setSilenceDevice
    (
    BluetoothAddress a_address
    )
{
    return BluetoothA2DPInterface::GetInterface().setSilenceDevice(a_address);
}

void BluetoothA2DPSource::GetConnectionStatus
    (
    BluetoothAddress& a_address,
    ConnectionStatus& a_status
    )
{
    a_address = m_impl->m_remoteAddress;
    a_status = m_impl->m_connectionStatus;
}

void BluetoothA2DPSource::StartAudioStream()
{
    if( m_impl )
    {
        LogDebug() << "Request start audio stream";
    }
}

void BluetoothA2DPSource::SuspendAudioStream()
{
    if( m_impl )
    {
        LogDebug() << "Request suspend audio stream";
    }
}

void BluetoothA2DPSource::StopAudioStream()
{
    if( m_impl )
    {
        LogDebug() << "Request stop audio stream";
    }
}

BluetoothA2DPSource::StreamStatus BluetoothA2DPSource::GetStreamStatus()
{
    if( m_impl )
    {
        return m_impl->m_streamStatus;
    }
    return BluetoothA2DPSource::StreamStatus::Stopped;
}

boost_ns::signals2::connection BluetoothA2DPSource::ConnectToStreamStatusChanged
    (
    std::function<void()> a_fun
    )
{
    if( m_impl )
    {
        return m_impl->m_streamStatusChangedSignal.connect( a_fun );
    }
    return boost_ns::signals2::connection();
}
}

