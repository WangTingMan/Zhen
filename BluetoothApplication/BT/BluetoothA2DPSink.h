#pragma once
#include <memory>
#include <vector>
#include <string>

#include <Zhen/global.h>

#include "BluetoothAddress.h"
#include "BluetoothCommonDefs.h"
#include "ConnectableModule.h"


namespace Bluetooth
{

class BluetoothA2DPSinkImpl;

class BluetoothA2DPSink : public ConnectableModule
{
public:

    BluetoothA2DPSink();

    static std::shared_ptr<BluetoothA2DPSink> GetMoudle();

    bool Connect
        (
        BluetoothAddress a_address
        );

    bool Disconnect
        (
        BluetoothAddress a_address
        );

    void GetConnectionStatus
        (
        BluetoothAddress& a_address,
        ConnectionStatus& a_status
        );

    void GetAudioConfig
        (
        BluetoothAddress& a_address,
        uint32_t& sample_rate,
        uint8_t& channel_count
        );

    void SetAudioFocus
        (
        bool a_grant
        );

    int Init()override;

    void SetRecordA2DPAudio( bool a_record );

    //Connection signals functions for external modules
public:

    boost_ns::signals2::connection ConnectToConnectionStatusChanged
        (
        std::function<void( BluetoothAddress, ConnectionStatus )> a_fun
        )
    {
        return m_connectionStatusChangedSignal.connect( a_fun );
    }

    boost_ns::signals2::connection ConnectToAudioConfigChanged
        (
        std::function<void()> a_fun
        )
    {
        return m_audioConfigChangedSignal.connect( a_fun );
    }

    // Handle the call backs
public:

    void HandleConnectionStatusChangedCallback
        (
        BluetoothAddress a_address,
        ConnectionStatus a_status
        );

    void HandleAudioConfigChangedCallback
        (
        BluetoothAddress a_address,
        uint32_t sample_rate,
        uint8_t channel_count
        );

    void HandleAudioStateChangedCallback
        (
        BluetoothAddress a_address,
        AudioPathState a_state
        );

private:

    std::shared_ptr<BluetoothA2DPSinkImpl> m_impl;
    boost_ns::signals2::signal<void( BluetoothAddress, ConnectionStatus )> m_connectionStatusChangedSignal;
    boost_ns::signals2::signal<void()> m_audioConfigChangedSignal;
};

}
