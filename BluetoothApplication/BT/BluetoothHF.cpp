#include "BluetoothHF.h"
#include "Adaptor.h"
#include <BT/StackLayer/BluetoothBaseInterface.h>
#include <BT/StackLayer/BluetoothHFInterface.h>

#include "Utils.h"

#include <Zhen/ExecutbleEvent.h>
#include <Zhen/PageManager.h>
#include <Zhen/logging.h>

#include <iostream>
#include <vector>
#include <chrono>

#define HANDS_FREE_CLIENT_MODULE "HFP_HF_ROLE"

using namespace Bluetooth;

CallSession::CallSession()
{
    index = 0;
    dir = CallDirection::CallIncoming;
    state = CallState::INCOMING;
    mpty = CallType::SINGLE;
}

bool CallSession::operator==( CallSession const& right )const
{
    if( state != right.state )
    {
        return false;
    }

    if( mpty != right.mpty )
    {
        return false;
    }
    return true;
}

std::string CallSession::ToString()const
{
    std::string str;
    str.append( "Index = " ).append( std::to_string( index ) );
    str.append( " Number = " ).append( number );
    str.append( " State = " ).append( CallStateToString( state ) );
    return str;
}

class BluetoothHFImpl
{
    friend class BluetoothHF;
    bool m_initialized = false;
    BluetoothAddress m_address;
    ConnectionStatus m_status;

    bool m_inBandRingtoneSupported = false;
    bool m_callInProgress = false;
    bool m_callSetuping = false;
    bool m_networkAvailable = false;
    BluetoothAddress m_scoAddress;
    ConnectionStatus m_scoConnectionStatus = ConnectionStatus::ServiceDisconnected;
    int m_signalStrength = 0;
    int m_battery = 0;
    int m_speakerVolume = 0;
    int m_micVolume = 0;
    int m_queryCallListTime = 0;
    std::string m_operatorName;
    std::string m_localNumber;
    std::vector<BluetoothAddress> m_blockedDevices;
    std::vector<CallSession> m_callSessions;
};

BluetoothHF::BluetoothHF()
{
    SetModuleName( HANDS_FREE_CLIENT_MODULE );

    m_impl = std::make_shared<BluetoothHFImpl>();
}

std::shared_ptr<BluetoothHF> BluetoothHF::GetMoudle()
{
    auto hf = Adaptor::GetInstance().FindModule( HANDS_FREE_CLIENT_MODULE );
    std::shared_ptr< BluetoothHF > hfModule = std::dynamic_pointer_cast< BluetoothHF >( hf );
    return hfModule;
}

int BluetoothHF::Init()
{
    BluetoothHFInterface::GetInterface().Init();
    return 0;
}

bool BluetoothHF::Connect
    (
    BluetoothAddress a_address
    )
{
    return BluetoothHFInterface::GetInterface().Connect( a_address );
}

bool BluetoothHF::Disconnect
    (
    BluetoothAddress a_address
    )
{
    return BluetoothHFInterface::GetInterface().disconnect( a_address );
}

void BluetoothHF::GetConnectionStatus
    (
    BluetoothAddress& a_address,
    ConnectionStatus& a_status
    )
{
    a_address = m_impl->m_address;
    a_status = m_impl->m_status;
}

void BluetoothHF::GetScoConnectionStatus
    (
    BluetoothAddress& a_address,
    ConnectionStatus& a_status
    )
{
    a_address = m_impl->m_scoAddress;
    a_status = m_impl->m_scoConnectionStatus;
}

void BluetoothHF::GetInbandRingtoneSupported
    (
    BluetoothAddress& a_address,
    bool& a_supported
    )
{
    a_address = m_impl->m_address;
    a_supported = m_impl->m_inBandRingtoneSupported;
}

void BluetoothHF::BlockDevice
    (
    BluetoothAddress a_address
    )
{
    auto beg = m_impl->m_blockedDevices.begin();
    for (; beg != m_impl->m_blockedDevices.end(); ++beg )
    {
        if( a_address == *beg )
        {
            return;
        }
    }

    m_impl->m_blockedDevices.push_back( a_address );
}

void BluetoothHF::UnblockDevice
    (
    BluetoothAddress a_address
    )
{
    auto beg = m_impl->m_blockedDevices.begin();
    for( ; beg != m_impl->m_blockedDevices.end(); ++beg )
    {
        if( a_address == *beg )
        {
            beg = m_impl->m_blockedDevices.erase( beg );
            return;
        }
        else
        {
            ++beg;
        }
    }
}

bool BluetoothHF::DialNumber
    (
    BluetoothAddress a_address,
    std::string a_number
    )
{
    return BluetoothHFInterface::GetInterface().dialNumber( a_address, a_number );
}

void BluetoothHF::GetSignalLevel
    (
    BluetoothAddress& a_address,
    int& a_signalLevel
    )
{
    a_signalLevel = m_impl->m_signalStrength;
    a_address = m_impl->m_address;
}

void BluetoothHF::GetBatteryLevel
    (
    BluetoothAddress& a_address,
    int& a_batteryLevel
    )
{
    a_batteryLevel = m_impl->m_battery;
    a_address = m_impl->m_address;
}

void BluetoothHF::GetOperatorName
    (
    BluetoothAddress& a_address,
    std::string& a_operatorName
    )
{
    a_address = m_impl->m_address;
    a_operatorName = m_impl->m_operatorName;
}

void BluetoothHF::GetLocalNumber
    (
    BluetoothAddress& a_address,
    std::string& a_number
    )
{
    a_address = m_impl->m_address;
    a_number = m_impl->m_localNumber;
}

std::vector<CallSession>const& BluetoothHF::GetCallList()
{
    return m_impl->m_callSessions;
}

void BluetoothHF::AnswerIncomingCall
    (
    BluetoothAddress& a_address,
    bool a_answered
    )
{
    BluetoothHFInterface::GetInterface().answerIncomingCall( a_address, a_answered );
}

void BluetoothHF::SwapCalls
    (
    BluetoothAddress a_address
    )
{
    BluetoothHFInterface::GetInterface().swapCalls( a_address );
}

void BluetoothHF::JoinCalls
    (
    BluetoothAddress a_address
    )
{
    BluetoothHFInterface::GetInterface().joinCalls( a_address );
}

void BluetoothHF::SplitCall
    (
    BluetoothAddress a_address,
    int a_index
    )
{
    BluetoothHFInterface::GetInterface().swapCalls( a_address );
}

boost_ns::signals2::connection BluetoothHF::ConnectToConnectionStatusChanged
    (
    std::function<void( BluetoothAddress, ConnectionStatus )> a_fun
    )
{
    return m_connectionStatusChangedSignal.connect( a_fun );
}

void BluetoothHF::HandleConnectionStatusChangedFromStack
    (
    BluetoothAddress a_address,
    ConnectionStatus a_status
    )
{
    m_impl->m_address = a_address;
    m_impl->m_status = a_status;
    m_connectionStatusChangedSignal( a_address, a_status );

    LogDebug() << "HF connection status changed. Address: " << a_address.ToString()
        << " connection status: " << ConnectionStatusToString( a_status );
    if( ConnectionStatus::ServiceConnected == a_status )
    {
        StartQueryCallList( a_address );
    }
}

void BluetoothHF::HanldeInBandRingtongSupportChangedCallback
    (
    BluetoothAddress a_address,
    bool a_supported
    )
{
    m_impl->m_inBandRingtoneSupported = a_supported;
}

void BluetoothHF::HandleSCOConnectionStatusChangedFromStack
    (
    BluetoothAddress a_address,
    ConnectionStatus a_status
    )
{
    m_impl->m_scoAddress = a_address;
    m_impl->m_scoConnectionStatus = a_status;
}

void BluetoothHF::HandleSpeakerVolumeChangedFromStack
    (
    BluetoothAddress a_address,
    int a_volume
    )
{
    m_impl->m_speakerVolume = a_volume;
}

void BluetoothHF::HandleMicVolumeChangedFromStack
    (
    BluetoothAddress a_address,
    int a_volume
    )
{
    m_impl->m_micVolume = a_volume;
}

void BluetoothHF::HandleSignalStrengthChangedFromStack
    (
    BluetoothAddress a_address,
    int a_signalStrength
    )
{
    m_impl->m_signalStrength = a_signalStrength;
}

void BluetoothHF::HandleBatteryLevelChangedFromStack
    (
    BluetoothAddress a_address,
    int a_battery
    )
{
    m_impl->m_battery = a_battery;
}

void BluetoothHF::HandleCallSetupChangedFromStack
    (
    BluetoothAddress a_address,
    bool a_setup
    )
{
    m_impl->m_callSetuping = a_setup;
    m_impl->m_callSessions.clear();
    m_calllistChangedSignal();
    StartQueryCallList( a_address );
}

void BluetoothHF::HandleCallProgressChangedFromStack
    (
    BluetoothAddress a_address,
    bool a_inProgress
    )
{
    m_impl->m_callInProgress = a_inProgress;
    StartQueryCallList( a_address );
}

void BluetoothHF::HandleCallListChangedFromStack
    (
    BluetoothAddress a_address,
    CallSession a_session
    )
{
    bool found = false;
    bool changed = false;
    auto& currentList = m_impl->m_callSessions;
    LogDebug( a_session.ToString().c_str() );

    for( auto it = currentList.begin(); it != currentList.end(); ++it )
    {
        if( it->index == a_session.index )
        {
            found = true;
            bool re = *it == a_session;
            if( !re )
            {
                *it = a_session;
                changed = true;
            }
            break;
        }
    }

    if( !found )
    {
        currentList.push_back( a_session );
        changed = true;
    }

    if( changed )
    {
        LogDebug( "emit call list changed" );
        m_calllistChangedSignal();
    }
}

void BluetoothHF::HandleOperatorChangedFromStack
    (
    BluetoothAddress a_address,
    std::string a_operator
    )
{
    m_impl->m_operatorName = UTF8_To_string( a_operator );
    m_operatorChangedSignal();
}

void BluetoothHF::HandleLocalNumberChangedFromStack
    (
    BluetoothAddress a_address,
    std::string a_number
    )
{
    m_impl->m_localNumber = a_number;
    m_operatorChangedSignal();
}

void BluetoothHF::HandleNetworkAvailableFromStack
    (
    BluetoothAddress a_address,
    bool a_available
    )
{
    m_impl->m_networkAvailable = a_available;
}

bool BluetoothHF::QueryCallList
    (
    BluetoothAddress a_address
    )
{
    bool ret = true;
    LogDebug( "to query the call list" );
    BluetoothHFInterface::GetInterface().QueryCallList( a_address );
    m_impl->m_queryCallListTime++;
    if( m_impl->m_queryCallListTime > 5 )
    {
        m_impl->m_queryCallListTime = 0;
        ret = false;
        LogDebug( "Stop query due to more times with no call list" );
        m_queryCallListConnection.disconnect();
    }
    return ret;
}

void BluetoothHF::StartQueryCallList
    (
    BluetoothAddress a_address
    )
{
    m_impl->m_queryCallListTime = 0;
    if( !m_queryCallListConnection.connected() )
    {
        m_queryCallListConnection = PageManager::GetInstance().connectTimerTo
            (
            std::bind( &BluetoothHF::QueryCallList, this, a_address ),
            1000
            );
    }
}
