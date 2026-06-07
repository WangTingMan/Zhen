#pragma once
#include <memory>

#include <Zhen/global.h>

#include "ConnectableModule.h"
#include "BluetoothAddress.h"
#include "BluetoothCommonDefs.h"

class BluetoothHFImpl;

struct CallSession
{
    int index;
    CallDirection dir;
    CallState state;
    CallType mpty;
    std::string number;

    CallSession();
    bool operator==( CallSession const & right )const;
    std::string ToString()const;
};

namespace Bluetooth
{
class BluetoothHF : public ConnectableModule
{

public:

    BluetoothHF();

    static std::shared_ptr<BluetoothHF> GetMoudle();

    int Init()override;

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

    void GetScoConnectionStatus
        (
        BluetoothAddress& a_address,
        ConnectionStatus& a_status
        );

    void GetInbandRingtoneSupported
        (
        BluetoothAddress& a_address,
        bool& a_supported
        );

    void BlockDevice
        (
        BluetoothAddress a_address
        );

    void UnblockDevice
        (
        BluetoothAddress a_address
        );

    bool DialNumber
        (
        BluetoothAddress a_address,
        std::string a_number
        );

    void GetSignalLevel
        (
        BluetoothAddress& a_address,
        int& a_signalLevel
        );

    void GetBatteryLevel
        (
        BluetoothAddress& a_address,
        int& a_batteryLevel
        );

    void GetOperatorName
        (
        BluetoothAddress& a_address,
        std::string& a_operatorName
        );

    void GetLocalNumber
        (
        BluetoothAddress& a_address,
        std::string& a_number
        );

    std::vector<CallSession>const& GetCallList();

    void AnswerIncomingCall
        (
        BluetoothAddress& a_address,
        bool a_answered = true
        );

    void StartQueryCallList
        (
        BluetoothAddress a_address
        );

    // Send AT+CHLD=2 to phone.
    void SwapCalls
        (
        BluetoothAddress a_address
        );

    void JoinCalls
        (
        BluetoothAddress a_address
        );

    void SplitCall
        (
        BluetoothAddress a_address,
        int a_index
        );

    //Connection signals functions for external modules
public:

    boost_ns::signals2::connection ConnectToConnectionStatusChanged
        (
        std::function<void( BluetoothAddress, ConnectionStatus )> a_fun
        );

    boost_ns::signals2::connection ConnectToScoConnectionStatusChanged
        (
        std::function<void( BluetoothAddress, ConnectionStatus )> a_fun
        )
    {
        return m_scoConnectionStatusChangedSignal.connect( a_fun );
    }

    boost_ns::signals2::connection ConnectToSignalLevelChanged
        (
        std::function<void()> a_fun
        )
    {
        return m_signalLevelChangedSignal.connect( a_fun );
    }

    boost_ns::signals2::connection ConnectToCallListChanged
        (
        std::function<void()> a_fun
        )
    {
        return m_calllistChangedSignal.connect( a_fun );
    }

    boost_ns::signals2::connection ConnectToOperatorChanged
        (
        std::function<void()> a_fun
        )
    {
        return m_operatorChangedSignal.connect( a_fun );
    }

    //Handle the event of stack callbacks
public:

    void HandleConnectionStatusChangedFromStack
        (
        BluetoothAddress a_address,
        ConnectionStatus a_status
        );

    void HanldeInBandRingtongSupportChangedCallback
        (
        BluetoothAddress a_address,
        bool a_supported
        );

    void HandleSCOConnectionStatusChangedFromStack
        (
        BluetoothAddress a_address,
        ConnectionStatus a_status
        );

    void HandleSpeakerVolumeChangedFromStack
        (
        BluetoothAddress a_address,
        int a_volume
        );

    void HandleMicVolumeChangedFromStack
        (
        BluetoothAddress a_address,
        int a_volume
        );

    void HandleSignalStrengthChangedFromStack
        (
        BluetoothAddress a_address,
        int a_signalStrength
        );

    void HandleBatteryLevelChangedFromStack
        (
        BluetoothAddress a_address,
        int a_battery
        );

    void HandleCallSetupChangedFromStack
        (
        BluetoothAddress a_address,
        bool a_hasCall
        );

    void HandleCallProgressChangedFromStack
        (
        BluetoothAddress a_address,
        bool a_inProgress
        );

    void HandleCallListChangedFromStack
        (
        BluetoothAddress a_address,
        CallSession a_session
        );

    // Handle the operator name( china modile, china unicom... )
    void HandleOperatorChangedFromStack
        (
        BluetoothAddress a_address,
        std::string a_operator
        );

    void HandleLocalNumberChangedFromStack
        (
        BluetoothAddress a_address,
        std::string a_number
        );

    void HandleNetworkAvailableFromStack
        (
        BluetoothAddress a_address,
        bool a_available
        );

private:

    bool QueryCallList
        (
        BluetoothAddress a_address
        );

private:

    std::shared_ptr< BluetoothHFImpl > m_impl;
    boost_ns::signals2::signal<void( BluetoothAddress, ConnectionStatus )> m_connectionStatusChangedSignal;
    boost_ns::signals2::signal<void( BluetoothAddress, ConnectionStatus )> m_scoConnectionStatusChangedSignal;
    boost_ns::signals2::signal<void()> m_signalLevelChangedSignal;
    boost_ns::signals2::signal<void()> m_calllistChangedSignal;
    boost_ns::signals2::signal<void()> m_operatorChangedSignal;
    boost_ns::signals2::connection m_queryCallListConnection;
};

}
