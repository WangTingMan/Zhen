#include "BluetoothAvrcpController.h"
#include "../Adaptor.h"
#include "BluetoothAvrcpInterface.h"

#include <Zhen/PageManager.h>
#include <Zhen/ExecutbleEvent.h>
#include <base/strings/sys_string_conversions.h>

#define AVRCP_CONTROLLER_ROLE "AVRCP_CONTROLLER_ROLE"

namespace Bluetooth
{

class BluetoothAvrcpControllerImpl
{

public:
    BluetoothAvrcpControllerImpl()
    {
        m_avrcp_interface = BluetoothAvrcpInterface::CreateAvrcpInterface();
        m_address = BluetoothAddress::kEmpty;
    }

    std::shared_ptr<BluetoothAvrcpInterface> m_avrcp_interface;
    bool m_inited = false;
    ConnectionStatus m_controlChannelStatus = ConnectionStatus::ServiceDisconnected;
    ConnectionStatus m_browseChannelStatus = ConnectionStatus::ServiceDisconnected;
    std::vector<AvrcpRemoteFeature> m_avrcpFeatures;
    std::size_t m_currentSongPos = 0;
    AvrcpPlayStatus m_playStatus = AvrcpPlayStatus::STOPPED;
    std::size_t m_songLength = 0;
    std::vector<std::pair< AvrcpMediaAttributeType, std::string> > m_playTrack;
    BluetoothAddress m_address;
    AvrcpPassThroughKeyCode m_currentHoldKey = AvrcpPassThroughKeyCode::PASS_THRU_CMD_ID_INVALID_KEY;
};

BluetoothAvrcpController::BluetoothAvrcpController()
{
    SetModuleName( AVRCP_CONTROLLER_ROLE );
}

std::shared_ptr<BluetoothAvrcpController> BluetoothAvrcpController::GetMoudle()
{
    auto hf = Adaptor::GetInstance().FindModule( AVRCP_CONTROLLER_ROLE );
    std::shared_ptr< BluetoothAvrcpController > hfModule = std::dynamic_pointer_cast< BluetoothAvrcpController >( hf );
    return hfModule;
}

void BluetoothAvrcpController::HandleAvrcpConnectionStatusChangedCallback
    (
    BluetoothAddress a_address,
    ConnectionStatus a_controlChannelStatus,
    ConnectionStatus a_browseChannelStatus
    )
{
    m_impl->m_controlChannelStatus = a_controlChannelStatus;
    m_impl->m_browseChannelStatus = a_browseChannelStatus;
    m_impl->m_address = a_address;

    if( a_controlChannelStatus == ConnectionStatus::ServiceConnected )
    {
        m_getPosTimerConnection.disconnect();
        PageManager::GetInstance().connectTimerTo
            (
            std::bind( &BluetoothAvrcpController::HandleGetPosTimerEvent, this ),
            1000,
            true
            );
    }
    else if( a_controlChannelStatus == ConnectionStatus::ServiceDisconnected )
    {
        m_getPosTimerConnection.disconnect();
    }
    m_avrcpConnectionStatusChangedSignal();
}

void BluetoothAvrcpController::HandleAvrcpFeaturesChangedCallback
    (
    BluetoothAddress a_address,
    std::vector<AvrcpRemoteFeature> a_features
    )
{
    m_impl->m_avrcpFeatures = a_features;
}

void BluetoothAvrcpController::HandlePlayTrackChangedCallback
    (
    BluetoothAddress a_address,
    std::vector<std::pair< AvrcpMediaAttributeType, std::string> > a_playTrack
    )
{
    for( auto ele : a_playTrack )
    {
        AvrcpMediaAttributeType type = ele.first;
        auto it = std::find_if
        (
            m_impl->m_playTrack.begin(),
            m_impl->m_playTrack.end(),
            [type]( auto& previous )
        {
            if( type == previous.first )
            {
                return true;
            }
            return false;
        }
        );
        if( it != m_impl->m_playTrack.end() )
        {
            it->second = ele.second;
        }
        else
        {
            m_impl->m_playTrack.push_back( ele );
        }
    }
    m_playTrackChangedSignal();
}

void BluetoothAvrcpController::HandlePlayPosChangedCallback
    (
    BluetoothAddress a_address,
    std::size_t a_length,
    std::size_t a_currentPos
    )
{
    bool changed = false;
    changed = ( a_currentPos != m_impl->m_currentSongPos ) ||
        ( a_length != m_impl->m_songLength );
    if( changed )
    {
        m_impl->m_currentSongPos = a_currentPos;
        if( a_length >= a_currentPos )
        {
            m_impl->m_songLength = a_length;
        }
        m_playPosChangedSignal();
    }
}

void BluetoothAvrcpController::HandlePlayStatusChangedCallback
    (
    BluetoothAddress a_address,
    AvrcpPlayStatus a_status
    )
{
    m_impl->m_playStatus = a_status;
    m_playStatusChangedSignal();
}

void BluetoothAvrcpController::HandleAvailablePlayerChangedCallback
    (
    BluetoothAddress a_address
    )
{

}

void BluetoothAvrcpController::HandleAddressedPlayerChangedCallback
    (
    BluetoothAddress a_address,
    uint16_t a_id
    )
{

}

void BluetoothAvrcpController::HandleNowPlayingContentChangedCallback
    (
    BluetoothAddress a_address
    )
{

}

bool BluetoothAvrcpController::HandleGetPosTimerEvent()
{
    return m_impl->m_avrcp_interface->SendGetPlayBackStateCmd(m_impl->m_address);
}

int BluetoothAvrcpController::Init()
{
    if( m_impl && m_impl->m_inited )
    {
        return 0;
    }

    m_impl = std::make_shared<BluetoothAvrcpControllerImpl>();
    m_impl->m_avrcp_interface->init();

    return 0;
}

bool BluetoothAvrcpController::Connect
    (
    BluetoothAddress a_address
    )
{
    return 0;
}

void BluetoothAvrcpController::NextTrack
    (
    bool a_toNextTrack
    )
{
    if( AvrcpPassThroughKeyCode::PASS_THRU_CMD_ID_INVALID_KEY != m_impl->m_currentHoldKey )
    {
        m_impl->m_avrcp_interface->SendPassThroughCmd(
            m_impl->m_address, AvrcpPassThroughKeyState::KEY_STATE_RELEASED, m_impl->m_currentHoldKey);
        m_impl->m_currentHoldKey = AvrcpPassThroughKeyCode::PASS_THRU_CMD_ID_INVALID_KEY;
    }

    AvrcpPassThroughKeyCode keyCode = a_toNextTrack ? AvrcpPassThroughKeyCode::PASS_THRU_CMD_ID_FORWARD 
        : AvrcpPassThroughKeyCode::PASS_THRU_CMD_ID_BACKWARD;
    m_impl->m_avrcp_interface->SendPassThroughCmd(
        m_impl->m_address, AvrcpPassThroughKeyState::KEY_STATE_PRESSED, keyCode);
    m_impl->m_avrcp_interface->SendPassThroughCmd(
        m_impl->m_address, AvrcpPassThroughKeyState::KEY_STATE_RELEASED, keyCode);
}

void BluetoothAvrcpController::SendStop()
{
    if( AvrcpPassThroughKeyCode::PASS_THRU_CMD_ID_INVALID_KEY != m_impl->m_currentHoldKey )
    {
        m_impl->m_avrcp_interface->SendPassThroughCmd(
            m_impl->m_address, AvrcpPassThroughKeyState::KEY_STATE_RELEASED, m_impl->m_currentHoldKey);
        m_impl->m_currentHoldKey = AvrcpPassThroughKeyCode::PASS_THRU_CMD_ID_INVALID_KEY;
    }

    if( AvrcpPlayStatus::PLAYING == m_impl->m_playStatus )
    {
        m_impl->m_avrcp_interface->SendPassThroughCmd(
            m_impl->m_address, AvrcpPassThroughKeyState::KEY_STATE_PRESSED,
            AvrcpPassThroughKeyCode::PASS_THRU_CMD_ID_STOP);
        m_impl->m_avrcp_interface->SendPassThroughCmd(
            m_impl->m_address, AvrcpPassThroughKeyState::KEY_STATE_RELEASED,
            AvrcpPassThroughKeyCode::PASS_THRU_CMD_ID_STOP);
    }
}

void BluetoothAvrcpController::SendPause()
{
    if( AvrcpPassThroughKeyCode::PASS_THRU_CMD_ID_INVALID_KEY != m_impl->m_currentHoldKey )
    {
        m_impl->m_avrcp_interface->SendPassThroughCmd(
            m_impl->m_address, AvrcpPassThroughKeyState::KEY_STATE_RELEASED, m_impl->m_currentHoldKey);
        m_impl->m_currentHoldKey = AvrcpPassThroughKeyCode::PASS_THRU_CMD_ID_INVALID_KEY;
    }

    if( AvrcpPlayStatus::PLAYING == m_impl->m_playStatus )
    {
        m_impl->m_avrcp_interface->SendPassThroughCmd(
            m_impl->m_address, AvrcpPassThroughKeyState::KEY_STATE_PRESSED,
            AvrcpPassThroughKeyCode::PASS_THRU_CMD_ID_PAUSE);
        m_impl->m_avrcp_interface->SendPassThroughCmd(
            m_impl->m_address, AvrcpPassThroughKeyState::KEY_STATE_RELEASED,
            AvrcpPassThroughKeyCode::PASS_THRU_CMD_ID_PAUSE);
    }
}

void BluetoothAvrcpController::SendPlay()
{
    if( AvrcpPassThroughKeyCode::PASS_THRU_CMD_ID_INVALID_KEY != m_impl->m_currentHoldKey )
    {
        m_impl->m_avrcp_interface->SendPassThroughCmd(
            m_impl->m_address, AvrcpPassThroughKeyState::KEY_STATE_RELEASED, m_impl->m_currentHoldKey);
        m_impl->m_currentHoldKey = AvrcpPassThroughKeyCode::PASS_THRU_CMD_ID_INVALID_KEY;
    }

    if( AvrcpPlayStatus::PLAYING != m_impl->m_playStatus )
    {
        m_impl->m_avrcp_interface->SendPassThroughCmd(
            m_impl->m_address, AvrcpPassThroughKeyState::KEY_STATE_PRESSED,
            AvrcpPassThroughKeyCode::PASS_THRU_CMD_ID_PLAY);
        m_impl->m_avrcp_interface->SendPassThroughCmd(
            m_impl->m_address, AvrcpPassThroughKeyState::KEY_STATE_RELEASED,
            AvrcpPassThroughKeyCode::PASS_THRU_CMD_ID_PLAY);
    }
}

void BluetoothAvrcpController::GetPlayTrack
    (
    BluetoothAddress& a_address,
    std::vector<std::pair< AvrcpMediaAttributeType, std::string> >& a_playTrack
    )
{
    a_address = m_impl->m_address;
    a_playTrack = m_impl->m_playTrack;
}

void BluetoothAvrcpController::GetPlayPos
    (
    std::size_t& a_length,
    std::size_t& a_currentPos
    )
{
    a_length = m_impl->m_songLength;
    a_currentPos = m_impl->m_currentSongPos;
}

AvrcpPlayStatus BluetoothAvrcpController::GetPlayStatus()
{
    return m_impl->m_playStatus;
}

void BluetoothAvrcpController::SetActiveDevice( BluetoothAddress a_address )
{
}

}
