#include "PageHandfree.h"
#include "PageHandfreeStatus.h"

#include <Bt/Adaptor.h>
#include <BT/BluetoothHF.h>

#include "Zhen/OptionContent.h"

#include <iostream>

using Bluetooth::BluetoothHF;

PageHandfree::PageHandfree()
{
    SetPageName( "PageHandfree" );
    SetTitle( "Handsfree" );
    m_content = std::make_shared<OptionContent>();
    m_content->AddOption( "Connect",
                        [this]()mutable
                        {
                             m_commandScheduler.SetExecutor( std::bind( 
                                 &PageHandfree::HandleConnectRequest,
                                 this,
                                 std::placeholders::_1 )
                                 );
                             m_commandScheduler.start();
                        } );

    m_content->AddOption( "Disconnect",
                        [this]()mutable
                        {
                             m_commandScheduler.SetExecutor( std::bind( 
                                 &PageHandfree::HandleDisconnectRequest,
                                 this,
                                 std::placeholders::_1 )
                                 );
                             m_commandScheduler.start();
                        } );

    m_content->AddOption( "Block Device" );
    m_content->AddOption( "UnBlock Device" );
    m_content->AddOption( "Answer Incoming Call",
                        [this]()mutable
                        {
                             m_commandScheduler.SetExecutor( std::bind( 
                                 &PageHandfree::HandleAnswerInComingCall,
                                 this,
                                 std::placeholders::_1 )
                                 );
                             m_commandScheduler.start();
                        } );
    m_content->AddOption( "Reject Incoming Call", std::bind( 
        &BluetoothHF::AnswerIncomingCall,
        BluetoothHF::GetMoudle(),
        std::ref( m_connectedDevice ),
        false ) );
    m_content->AddOption( "Dial Number",
                        [this]()mutable
                        {
                             m_commandScheduler.SetExecutor( std::bind( 
                                 &PageHandfree::HandleDialNumber,
                                 this,
                                 std::placeholders::_1 )
                                 );
                             m_commandScheduler.start();
                        } );
    m_content->AddOption( "Hold Call", std::bind(
        &BluetoothHF::SwapCalls,
        BluetoothHF::GetMoudle(),
        std::ref( m_connectedDevice ) ) );
    m_content->AddOption( "Resume Call", std::bind(
        &BluetoothHF::SwapCalls,
        BluetoothHF::GetMoudle(),
        std::ref( m_connectedDevice ) ) );
    m_content->AddOption( "Swap Calls", std::bind(
        &BluetoothHF::SwapCalls,
        BluetoothHF::GetMoudle(),
        std::ref( m_connectedDevice ) ) );
    m_content->AddOption( "Join Calls", std::bind(
        &BluetoothHF::JoinCalls,
        BluetoothHF::GetMoudle(),
        std::ref( m_connectedDevice ) ) );
    m_content->AddOption( "Split Call",
                        [this]()mutable
                        {
                             m_commandScheduler.SetExecutor( std::bind(
                                 &PageHandfree::HandleSplitCall,
                                 this,
                                 std::placeholders::_1 )
                                 );
                             m_commandScheduler.start();
                        } );
    m_content->AddOption( "Hang Up Call" );
    m_content->AddOption( "Hang Up All Calls" );
    m_content->AddOption( "Send DTMF" );
    m_content->AddOption( "Transfer Audio To HF" );
    m_content->AddOption( "Transfer Audio To Phone" );
    m_content->AddOption( "Show current status", std::bind( &PageHandfree::HandleShowStatusPage, this ) );

    m_statusContent = std::make_shared<StringContent>();
    m_statusContent->SetTitle( "Stauts" );

    m_connectionStatusChangedConnection =
        BluetoothHF::GetMoudle()->ConnectToConnectionStatusChanged
        (
            [ this ]( BluetoothAddress, ConnectionStatus )
            {
                HandleConnectionStatusChanged();
            }
        );

    m_connectionCallListChangedConnection = 
        BluetoothHF::GetMoudle()->ConnectToCallListChanged
        (
        std::bind( &PageHandfree::HandleCallListChanged, this )
        );

    m_connectionOperatorChangedConnection = 
        BluetoothHF::GetMoudle()->ConnectToOperatorChanged
            (
            [ this ]()
            {
                 PageNeedFresh();
            }
            );
}

PageHandfree::~PageHandfree()
{
    m_connectionStatusChangedConnection.disconnect();
    m_connectionCallListChangedConnection.disconnect();
    m_connectionOperatorChangedConnection.disconnect();
}

std::string PageHandfree::GetPrintableString()const
{
    PageHandfree* page = const_cast< PageHandfree* > ( this );
    page->UpdateStatusContent();

    std::string ret;
    ret = BasePage::GetPrintableString();
    ret.append( m_content->GetPrintableString() );
    ret.append( m_statusContent->GetPrintableString() );
    return ret;
}

bool PageHandfree::OnEvent( std::shared_ptr<Event> a_event )
{
    bool ret = false;
    ret = m_commandScheduler.OnEvent( a_event );

    if( !ret )
    {
        ret = m_content->OnEvent( a_event );
    }

    if( !ret )
    {
        ret = BasePage::OnEvent( a_event );
    }

    return ret;
}

bool PageHandfree::HandleConnectRequest
    (
    std::string const& a_input
    )
{
    bool done = false;

    if( 0 == m_commandScheduler.GetStepNumber() )
    {
        std::list<RemoteDevice>const &pairedDevices = Adaptor::GetInstance().GetPairedDevices();
        if( pairedDevices.empty() )
        {
            done = true;
            std::cout << "There is no paired device to connect\n";
        }
        else
        {
            int i = 1;
            for( auto it = pairedDevices.begin(); it != pairedDevices.end(); ++it, ++i )
            {
                std::cout << i << ". " << it->name << std::endl;
            }
            std::cout << "Enter the number of remote device:\n";
            m_commandScheduler.SetStepNumber( 1 );
        }
    }
    else if( 1 == m_commandScheduler.GetStepNumber() )
    {
        m_commandScheduler.AddParameter( a_input );
        m_commandScheduler.SetStepNumber( 2 );
    }

    if( !done && 1 == m_commandScheduler.GetNumParameters() )
    {
        std::string numberString = std::any_cast< std::string >( m_commandScheduler.GetParameter( 0 ) );
        int number = 0;
        try
        {
            number = std::stoi( numberString );
            if( number < 1 )
            {
                std::cout << "Wrong index, enter again:\n";
                m_commandScheduler.RemoveLastParameter();
                m_commandScheduler.SetStepNumber( 1 );
                done = false;
                return done;
            }

            BluetoothAddress address;
            std::list<RemoteDevice>const &pairedDevices = Adaptor::GetInstance().GetPairedDevices();
            if( pairedDevices.size() < number )
            {
                std::cout << "Wrong index. Enter again:\n";
                m_commandScheduler.RemoveLastParameter();
                m_commandScheduler.SetStepNumber( 1 );
                done = false;
                return done;
            }
            std::list<RemoteDevice>::const_iterator it = pairedDevices.begin();
            std::advance( it, number - 1 );
            address = it->address;
            BluetoothHF::GetMoudle()->Connect( address );
            done = true;
        }
        catch( std::exception& e )
        {
            std::cout << "Error: " << e.what() << ", enter again:";
            m_commandScheduler.RemoveLastParameter();
            m_commandScheduler.SetStepNumber( 1 );
            done = false;
        }
    }
    return done;
}

bool PageHandfree::HandleDisconnectRequest
    (
    std::string const& a_input
    )
{
    bool done = false;
    ConnectionStatus a_status = ConnectionStatus::ServiceDisconnected;
    BluetoothHF::GetMoudle()->GetConnectionStatus( m_connectedDevice, a_status );

    if( ConnectionStatus::ServiceConnected == a_status )
    {
        BluetoothHF::GetMoudle()->Disconnect( m_connectedDevice );
    }
    done = true;
    return done;
}

bool PageHandfree::HandleDialNumber
    (
    std::string const& a_input
    )
{
    bool done = false;
    ConnectionStatus a_status = ConnectionStatus::ServiceDisconnected;
    BluetoothHF::GetMoudle()->GetConnectionStatus( m_connectedDevice, a_status );

    if( a_status != ConnectionStatus::ServiceConnected )
    {
        std::cout << "No connected device. Cannot dial a number.\n";
        m_commandScheduler.clear();
        done = true;
        return done;
    }

    if( 0 == m_commandScheduler.GetStepNumber() )
    {
        std::cout << "Input the number to dial:\n";
        m_commandScheduler.SetStepNumber( 1 );
    }
    else if( 1 == m_commandScheduler.GetStepNumber() )
    {
        m_commandScheduler.AddParameter( a_input );
        m_commandScheduler.SetStepNumber( 2 );
    }

    if( !done && 2 == m_commandScheduler.GetStepNumber() )
    {
        std::string numberString = std::any_cast< std::string >( m_commandScheduler.GetParameter( 0 ) );
        done = true;
        BluetoothHF::GetMoudle()->DialNumber( m_connectedDevice, numberString );
    }
    return done;
}

bool PageHandfree::HandleAnswerInComingCall
    (
    std::string const& a_input
    )
{
    bool done = false;
    ConnectionStatus a_status = ConnectionStatus::ServiceDisconnected;
    BluetoothHF::GetMoudle()->GetConnectionStatus( m_connectedDevice, a_status );
    BluetoothHF::GetMoudle()->AnswerIncomingCall( m_connectedDevice );
    done = true;
    return done;
}

bool PageHandfree::HandleSplitCall
    (
    std::string const& a_input
    )
{
    bool done = false;
    if( 0 == m_commandScheduler.GetStepNumber() )
    {
        std::cout << "Input the number to split:\n";
        m_commandScheduler.SetStepNumber( 1 );
    }
    else if( 1 == m_commandScheduler.GetStepNumber() )
    {
        m_commandScheduler.AddParameter( a_input );
        m_commandScheduler.SetStepNumber( 2 );
    }

    if( !done && 2 == m_commandScheduler.GetStepNumber() )
    {
        int number = 0;
        try
        {
            number = std::stoi( a_input );
            BluetoothHF::GetMoudle()->SplitCall( m_connectedDevice, number );
        }
        catch( std::exception& e )
        {
            std::cout << "Error: " << e.what();
        }
        m_commandScheduler.clear();
        done = true;
    }

    return done;
}

void PageHandfree::UpdateStatusContent()
{
    ConnectionStatus status;
    auto hfClient = BluetoothHF::GetMoudle();
    hfClient->GetConnectionStatus
        (
        m_connectedDevice,
        status
        );

    std::string str;
    str.append( "Connect status: " ).append( ConnectionStatusToString( status ) );
    if( ConnectionStatus::ServiceDisconnected != status )
    {
        str.append( "; Remote address: " ).append( m_connectedDevice.ToString() );
        bool inband = false;
        int signal = 0;
        int battery = 0;
        std::string operatorName;
        std::string localNumber;
        hfClient->GetInbandRingtoneSupported
            (
            m_connectedDevice,
            inband
            );
        hfClient->GetScoConnectionStatus
            (
            m_connectedDevice,
            status
            );
        hfClient->GetSignalLevel
            (
            m_connectedDevice,
            signal
            );
        hfClient->GetBatteryLevel
            (
            m_connectedDevice,
            battery
            );
        hfClient->GetOperatorName
            (
            m_connectedDevice,
            operatorName
            );
        hfClient->GetLocalNumber
            (
            m_connectedDevice,
            localNumber
            );
        str.append( "; Sco connection: " ).append( ConnectionStatusToString( status ) );
    }
    str.push_back( '\n' );
    m_statusContent->SetString( std::move( str ) );
}

void PageHandfree::HandleConnectionStatusChanged()
{
    PageNeedFresh();
}

void PageHandfree::HandleCallListChanged()
{
    PageNeedFresh();
}

void PageHandfree::HandleShowStatusPage()
{
    auto page = std::make_shared<PageHandfreeStatus>();
    PageManager::GetInstance().PushPage( page );
}
