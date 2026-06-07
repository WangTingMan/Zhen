#include "BluetoothA2DPSink.h"
#include "Adaptor.h"
#include "StackLayer/BluetoothA2DPSinkInterface.h"

#include "Zhen\PageManager.h"
#include "Zhen\ExecutbleEvent.h"
#include "Zhen\logging.h"

#include <iostream>

#include <base/strings/sys_string_conversions.h>

#define A2DP_SINK_MODULE "A2DP_SINK_MODULE"

namespace Bluetooth
{

class BluetoothA2DPSinkImpl
{
    friend class BluetoothA2DPSink;

    bool m_inited = false;

    BluetoothAddress m_address;
    ConnectionStatus m_status;
    uint32_t m_sampleRate;
    uint8_t m_channelCount;
    AudioPathState m_audioState = AudioPathState::STOPPED;
};

BluetoothA2DPSink::BluetoothA2DPSink()
{
    SetModuleName( A2DP_SINK_MODULE );
}

std::shared_ptr<BluetoothA2DPSink> BluetoothA2DPSink::GetMoudle()
{
    auto hf = Adaptor::GetInstance().FindModule( A2DP_SINK_MODULE );
    std::shared_ptr< BluetoothA2DPSink > hfModule = std::dynamic_pointer_cast< BluetoothA2DPSink >( hf );
    return hfModule;
}

bool BluetoothA2DPSink::Connect
    (
    BluetoothAddress a_address
    )
{
    return BluetoothA2DPSinkInterface::GetInterface().Connect(a_address);
}

bool BluetoothA2DPSink::Disconnect
    (
    BluetoothAddress a_address
    )
{
    return BluetoothA2DPSinkInterface::GetInterface().disconnect(a_address);
}

void BluetoothA2DPSink::GetConnectionStatus
(
    BluetoothAddress& a_address,
    ConnectionStatus& a_status
)
{
    a_address = m_impl->m_address;
    a_status = m_impl->m_status;
}

void BluetoothA2DPSink::GetAudioConfig
(
    BluetoothAddress& a_address,
    uint32_t& sample_rate,
    uint8_t& channel_count
)
{
    a_address = m_impl->m_address;
    sample_rate = m_impl->m_sampleRate;
    channel_count = m_impl->m_channelCount;
}

void BluetoothA2DPSink::SetAudioFocus
    (
    bool a_grant
    )
{
    BluetoothA2DPSinkInterface::GetInterface().SetAudioFocus(a_grant);
}

int BluetoothA2DPSink::Init()
{
    if( m_impl && m_impl->m_inited )
    {
        return 0;
    }

    SetModuleName( A2DP_SINK_MODULE );

    BluetoothA2DPSinkInterface::GetInterface().Init();
    m_impl = std::make_shared<BluetoothA2DPSinkImpl>();
    m_impl->m_inited = true;

    return 0;
}

void BluetoothA2DPSink::SetRecordA2DPAudio( bool a_record )
{
    //m_impl->m_a2dpSinkInterface->record_a2dp_pcm( PCM_RECORD_PATH, a_record );
}

void BluetoothA2DPSink::HandleConnectionStatusChangedCallback
(
    BluetoothAddress a_address,
    ConnectionStatus a_status
)
{
    m_impl->m_address = a_address;
    m_impl->m_status = a_status;

    LogDebug() << "A2DP connection status changed. Address: " << a_address.ToString() << " connection status: " << ConnectionStatusToString( a_status );

    m_connectionStatusChangedSignal( a_address, a_status );

    std::function<void()> fun;
    fun = [this, a_address, a_status]()
    {
        if( ConnectionStatus::ServiceConnected == a_status )
        {
            Disconnect( a_address );
        }
        else if( ConnectionStatus::ServiceDisconnected == a_status )
        {
            Connect( a_address );
        }
    };

    if( ConnectionStatus::ServiceConnected == a_status ||
        ConnectionStatus::ServiceDisconnected == a_status )
    {
        //sPageManager::GetInstance().connectOneShotTimerTo( fun, 5000, true );
    }
}

void BluetoothA2DPSink::HandleAudioConfigChangedCallback
(
    BluetoothAddress a_address,
    uint32_t sample_rate,
    uint8_t channel_count
)
{
    m_impl->m_sampleRate = sample_rate;
    m_impl->m_channelCount = channel_count;
    m_audioConfigChangedSignal();
}

void BluetoothA2DPSink::HandleAudioStateChangedCallback
(
    BluetoothAddress a_address,
    AudioPathState a_state
)
{
    m_impl->m_audioState = a_state;
}

}
