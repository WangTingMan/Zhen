#include "PageHandfreeStatus.h"

#include <BT/BluetoothHF.h>

using Bluetooth::BluetoothHF;

PageHandfreeStatus::PageHandfreeStatus()
{
    SetTitle( "Hand free status" );
    RefreshHandfreeStatus();

    m_connectStatusConnection = BluetoothHF::GetMoudle()->ConnectToConnectionStatusChanged
        (
        [this]( BluetoothAddress a_address, ConnectionStatus a_status )
        {
            RefreshHandfreeStatus();
            PageNeedFresh();
        }
        );
    m_connectCallListChangeConnection = BluetoothHF::GetMoudle()->ConnectToCallListChanged
        (
        [this]()
        {
            RefreshHandfreeStatus();
            PageNeedFresh();
        }
        );
}

PageHandfreeStatus::~PageHandfreeStatus()
{
    m_connectStatusConnection.disconnect();
    m_connectCallListChangeConnection.disconnect();
}

void PageHandfreeStatus::RefreshHandfreeStatus()
{
    std::string statusString;
    BluetoothAddress address;
    ConnectionStatus status;
    auto hfModule = BluetoothHF::GetMoudle();
    hfModule->GetConnectionStatus( address, status );
    statusString.append( "Phone: " ).append( ConnectionStatusToString( status ) ).append( ". " );
    if( status == ConnectionStatus::ServiceConnected )
    {
        bool inbandRingtoneSupported = false;
        hfModule->GetInbandRingtoneSupported( address, inbandRingtoneSupported );
        statusString.append( "\nIn-band ringtone: " ).append( ( inbandRingtoneSupported ? "supported. " : "not supported. " ) );
        int signalLevel = 0;
        hfModule->GetSignalLevel( address, signalLevel );
        statusString.append( "\nSingnal level: " ).append( std::to_string( signalLevel ) ).append( ". " );
        int batteryLevel = 0;
        hfModule->GetBatteryLevel( address, batteryLevel );
        statusString.append( "\nBattery level: " ).append( std::to_string( batteryLevel ) ).append( ". " );
        std::string operatorName;
        hfModule->GetOperatorName( address, operatorName );
        statusString.append( "\nOperator name: " ).append( operatorName ).append( ". " );
        std::string localNumber;
        hfModule->GetLocalNumber( address, localNumber );
        statusString.append( "\nLocal number: " ).append( localNumber ).append( ". " );
        auto& callList = hfModule->GetCallList();
        if( !callList.empty() )
        {
            statusString.append( "\nCall list:" );
            for( auto it = callList.begin(); it != callList.end(); ++it )
            {
                statusString.append( "\nCall index: " ).append( std::to_string( it->index ) );
                statusString.append( "\ncall number: " ).append( it->number );
                statusString.append( "\ncall state: " ).append( CallStateToString( it->state ) );
            }
        }
    }

    statusString.push_back( '\n' );
    SetContentString( statusString );
}

