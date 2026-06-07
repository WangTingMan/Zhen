#pragma once

#include "BT/ConnectableModule.h"

#include <memory>
#include <Zhen/global.h>

class BluetoothA2DPSourceImpl;

namespace Bluetooth
{

// Since we want to handle multiple A2DP SINK connection then we cannot
// derive from connectable module.
class BluetoothA2DPSource : public AbstractModule
{

public:

    enum StreamStatus
    {
        Started,
        Stopped,
        RemoteSuspended
    };

    BluetoothA2DPSource();

    static std::shared_ptr<BluetoothA2DPSource> GetMoudle();

    int Init()override;

    bool Connect
        (
        BluetoothAddress a_address
        );

    bool disconnect
        (
        BluetoothAddress a_address
        );

    bool setActiveDevice
        (
        BluetoothAddress a_address
        );

    bool setSilenceDevice
        (
        BluetoothAddress a_address
        );

    void GetConnectionStatus
        (
        BluetoothAddress& a_address,
        ConnectionStatus& a_status
        );

    void StartAudioStream();

    void SuspendAudioStream();

    void StopAudioStream();

    StreamStatus GetStreamStatus();

public:

    boost_ns::signals2::connection ConnectToConnectionStatusChanged
        (
        std::function<void( BluetoothAddress, ConnectionStatus )> a_fun
        )
    {
        return m_connectionStatusChangedSignal.connect( a_fun );
    }

    boost_ns::signals2::connection ConnectToStreamStatusChanged
        (
        std::function<void()> a_fun
        );

    boost_ns::signals2::connection ConnectToActiveDeviceChanged
        (
        std::function<void( BluetoothAddress )> a_fun
        )
    {
        return m_activeDeviceChangedSignal.connect( a_fun );
    }

public:

    bool handle_btav_mandatory_codec_preferred_callback(
        const BluetoothAddress& bd_addr );

    void HandleConnectStatusChanged
        (
        BluetoothAddress a_address,
        ConnectionStatus a_status
        );

    std::shared_ptr<BluetoothA2DPSourceImpl> GetImpl()
    {
        return m_impl;
    }

private:

    std::shared_ptr<BluetoothA2DPSourceImpl> m_impl;
    boost_ns::signals2::signal<void(BluetoothAddress, ConnectionStatus)> m_connectionStatusChangedSignal;
    boost_ns::signals2::signal<void(BluetoothAddress)> m_activeDeviceChangedSignal;
};

}

