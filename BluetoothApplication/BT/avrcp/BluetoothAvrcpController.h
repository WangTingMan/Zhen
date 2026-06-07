#pragma once

#include "BT/ConnectableModule.h"

#include <memory>
#include <Zhen/global.h>

namespace Bluetooth
{

class BluetoothAvrcpControllerImpl;

class BluetoothAvrcpController : public AbstractModule
{

public:

    BluetoothAvrcpController();

    static std::shared_ptr<BluetoothAvrcpController> GetMoudle();

    int Init()override;

    bool Connect
        (
        BluetoothAddress a_address
        );

    void NextTrack
        (
        bool a_toNextTrack = true
        );

    void SendStop();

    void SendPause();

    void SendPlay();

    void GetPlayTrack
        (
        BluetoothAddress& a_address,
        std::vector<std::pair< AvrcpMediaAttributeType, std::string> >& a_playTrack
        );

    void GetPlayPos
        (
        std::size_t& a_length,
        std::size_t& a_currentPos
        );

    AvrcpPlayStatus GetPlayStatus();

    void SetActiveDevice( BluetoothAddress a_address );

public:

    boost_ns::signals2::connection ConnectToPlayTrackChanged
        (
        std::function<void()> a_fun
        )
    {
        return m_playTrackChangedSignal.connect( a_fun );
    }

    boost_ns::signals2::connection ConnectToPlayStatusChanged
        (
        std::function<void()> a_fun
        )
    {
        return m_playStatusChangedSignal.connect( a_fun );
    }

    boost_ns::signals2::connection ConnectToPlayposChanged
        (
        std::function<void()> a_fun
        )
    {
        return m_playPosChangedSignal.connect( a_fun );
    }

    boost_ns::signals2::connection ConnectToAvrcpConnectionStatusChanged
        (
        std::function<void()> a_fun
        )
    {
        return m_avrcpConnectionStatusChangedSignal.connect( a_fun );
    }

public:

    void HandleAvrcpConnectionStatusChangedCallback
        (
        BluetoothAddress a_address,
        ConnectionStatus a_controlChannelStatus,
        ConnectionStatus a_browseChannelStatus
        );

    void HandleAvrcpFeaturesChangedCallback
        (
        BluetoothAddress a_address,
        std::vector<AvrcpRemoteFeature> a_features
        );

    void HandlePlayTrackChangedCallback
        (
        BluetoothAddress a_address,
        std::vector<std::pair< AvrcpMediaAttributeType, std::string> > a_playTrack
        );

    void HandlePlayPosChangedCallback
        (
        BluetoothAddress a_address,
        std::size_t a_length,
        std::size_t a_currentPos
        );

    void HandlePlayStatusChangedCallback
        (
        BluetoothAddress a_address,
        AvrcpPlayStatus a_status
        );

    void HandleAvailablePlayerChangedCallback
        (
        BluetoothAddress a_address
        );

    void HandleAddressedPlayerChangedCallback
        (
        BluetoothAddress a_address,
        uint16_t a_id
        );

    void HandleNowPlayingContentChangedCallback
        (
        BluetoothAddress a_address
        );

    bool HandleGetPosTimerEvent();

private:

    std::shared_ptr<BluetoothAvrcpControllerImpl> m_impl;
    boost_ns::signals2::signal<void()> m_avrcpConnectionStatusChangedSignal;
    boost_ns::signals2::signal<void()> m_playTrackChangedSignal;
    boost_ns::signals2::signal<void()> m_playPosChangedSignal;
    boost_ns::signals2::signal<void()> m_playStatusChangedSignal;
    boost_ns::signals2::connection m_getPosTimerConnection;
};

}

