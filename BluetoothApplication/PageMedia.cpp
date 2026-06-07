#include "PageMedia.h"
#include "PageMediaStatus.h"

#include "BT/BluetoothA2DPSink.h"
#include "BT/avrcp/BluetoothAvrcpController.h"

#include "Zhen/OptionContent.h"
#include "Zhen/StringContent.h"
#include "Zhen/BasePromptedPage.h"

#include <iostream>

PageMedia::PageMedia()
{
    m_content = std::make_shared<OptionContent>();

    SetTitle( "A2DP SINK" );
    m_content->AddOption( "Connect",
                        [this]()mutable
                        {
                             m_commandScheduler.SetExecutor( std::bind( 
                                 &PageMedia::HandleConnectRequest,
                                 this,
                                 std::placeholders::_1 )
                                 );
                             m_commandScheduler.start();
                        } );
    m_content->AddOption( "Disconnect",
                        [this]()mutable
                        {
                             m_commandScheduler.SetExecutor( std::bind( 
                                 &PageMedia::HandleDisconnectRequest,
                                 this,
                                 std::placeholders::_1 )
                                 );
                             m_commandScheduler.start();
                        } );
    m_content->AddOption( "Block Device           " );
    m_content->AddOption( "UnBlock Device         " );
    m_content->AddOption( "Play",
                        [this]()mutable
                        {
                            Bluetooth::BluetoothAvrcpController::GetMoudle()->SendPlay();
                        } );
    m_content->AddOption( "Pause",
                        [this]()mutable
                        {
                            Bluetooth::BluetoothAvrcpController::GetMoudle()->SendPause();
                        } );
    m_content->AddOption( "Stop",
                        [this]()mutable
                        {
                            Bluetooth::BluetoothAvrcpController::GetMoudle()->SendStop();
                        } );
    m_content->AddOption( "Next Track",
                        [this]()mutable
                        {
                            Bluetooth::BluetoothAvrcpController::GetMoudle()->NextTrack();
                        } );
    m_content->AddOption( "Previous Track",
                        [this]()mutable
                        {
                            Bluetooth::BluetoothAvrcpController::GetMoudle()->NextTrack( false );
                        } );
    m_content->AddOption( "Start Fast Forward     " );
    m_content->AddOption( "Stop Fast Forward      " );
    m_content->AddOption( "Start Rewind           " );
    m_content->AddOption( "Stop Rewind            " );
    m_content->AddOption( "Set Repeade Mode       " );
    m_content->AddOption( "Set Shuffle Mode       " );
    m_content->AddOption( "Request Track Info     " );
    m_content->AddOption( "Browse                 " );
    m_content->AddOption( "Folder Level Down      " );
    m_content->AddOption( "Folder Level Up        " );
    m_content->AddOption( "Folder Level To Root   " );
    m_content->AddOption( "Search Item            " );
    m_content->AddOption( "Play Item              " );
    m_content->AddOption( "Set audio focus",
                        [this]()mutable
                        {
                             m_commandScheduler.SetExecutor( std::bind( 
                                 &PageMedia::HandleAudioFocusRequest,
                                 this,
                                 std::placeholders::_1 )
                                 );
                             m_commandScheduler.start();
                        } );
    m_content->AddOption( "Show Play status", std::bind( &PageMedia::HandleShowPlayStatus, this ) );
    m_content->AddOption( "Start record PCM", std::bind( &Bluetooth::BluetoothA2DPSink::SetRecordA2DPAudio, Bluetooth::BluetoothA2DPSink::GetMoudle(), true ) );
    m_content->AddOption( "Stop record PCM", std::bind( &Bluetooth::BluetoothA2DPSink::SetRecordA2DPAudio, Bluetooth::BluetoothA2DPSink::GetMoudle(), false ) );

    m_statusContent = std::make_shared<StringContent>();
    m_statusContent->SetTitle( "Stauts" );

    std::function<void( BluetoothAddress, ConnectionStatus )> fun =
        std::bind( &PageMedia::HandleConnectionStatusChanged, this, std::placeholders::_1, std::placeholders::_2 );
    m_connectionStatusChangedConnection =
        Bluetooth::BluetoothA2DPSink::GetMoudle()->ConnectToConnectionStatusChanged( fun );

    m_audioCondigChangedConnection =
        Bluetooth::BluetoothA2DPSink::GetMoudle()->ConnectToAudioConfigChanged
        (
            [ this ]()mutable
            {
                PageNeedFresh();
            }
        );
}

PageMedia::~PageMedia()
{
    m_connectionStatusChangedConnection.disconnect();
}

std::string PageMedia::GetPrintableString()const
{
    PageMedia* page = const_cast< PageMedia* > ( this );
    page->UpdateStatusContent();

    std::string str = BasePage::GetPrintableString();
    str.append( m_content->GetPrintableString() );
    str.append( m_statusContent->GetPrintableString() );
    return str;
}

bool PageMedia::OnEvent( std::shared_ptr<Event> a_event )
{
    bool r = m_commandScheduler.OnEvent( a_event );

    if( !r )
    {
        r = m_content->OnEvent( a_event );
    }

    if( !r )
    {
        BasePage::OnEvent( a_event );
    }

    return r;
}

bool PageMedia::HandleConnectRequest
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
            Bluetooth::BluetoothA2DPSink::GetMoudle()->Connect( address );
            m_commandScheduler.BackToBeginning();
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

bool PageMedia::HandleDisconnectRequest
    (
    std::string const& a_input
    )
{
    bool done = false;
    BluetoothAddress a_address;
    ConnectionStatus a_status = ConnectionStatus::ServiceDisconnected;
    Bluetooth::BluetoothA2DPSink::GetMoudle()->GetConnectionStatus( a_address, a_status );

    if( ConnectionStatus::ServiceConnected == a_status )
    {
        Bluetooth::BluetoothA2DPSink::GetMoudle()->Disconnect( a_address );
    }
    done = true;
    return done;
}

bool PageMedia::HandleAudioFocusRequest
    (
    std::string const& a_input
    )
{
    bool done = false;
    BluetoothAddress a_address;
    ConnectionStatus a_status = ConnectionStatus::ServiceDisconnected;
    Bluetooth::BluetoothA2DPSink::GetMoudle()->GetConnectionStatus( a_address, a_status );

    if( ConnectionStatus::ServiceConnected != a_status )
    {
        done = true;
        std::cout << "A2DP is disconnected.\n";
        return done;
    }

    if( 0 == m_commandScheduler.GetStepNumber() )
    {
        std::cout << "To grant audio focus( Y\\N )?:\n";
        m_commandScheduler.SetStepNumber( 1 );
    }
    else if( 1 == m_commandScheduler.GetStepNumber() )
    {
        m_commandScheduler.AddParameter( a_input );
        m_commandScheduler.SetStepNumber( 2 );
    }

    if( !done && 2 == m_commandScheduler.GetStepNumber() )
    {
        std::string input = std::any_cast< std::string >( m_commandScheduler.GetParameter( 0 ) );
        bool valid = true;
        do 
        {
            if( 1 != input.size() )
            {
                valid = false;
                break;
            }

            char ch = std::toupper( input[0] );
            if( ch != 'Y' && ch != 'N' )
            {
                valid = false;
            }
        } while (false);
        
        if( !valid )
        {
            std::cout << "Cannot understand the input content. Please input Y\\N :";
            done = false;
            m_commandScheduler.SetStepNumber( 1 );
            m_commandScheduler.RemoveLastParameter();
        }
        else
        {
            char ch = std::toupper( input[0] );
            Bluetooth::BluetoothA2DPSink::GetMoudle()->SetAudioFocus( 'Y' == ch );
            Bluetooth::BluetoothAvrcpController::GetMoudle()->SetActiveDevice( a_address );
            done = true;
        }
    }

    return done;
}

void PageMedia::HandleShowPlayStatus()
{
    auto page = std::make_shared<PageMediaStatus>();
    PageManager::GetInstance().PushPage( page );
}

void PageMedia::HandleConnectionStatusChanged
    (
    BluetoothAddress a_address,
    ConnectionStatus a_status
    )
{
    PageNeedFresh();
}

void PageMedia::UpdateStatusContent()
{
    std::string str;
    BluetoothAddress address;
    ConnectionStatus status;

    Bluetooth::BluetoothA2DPSink::GetMoudle()->GetConnectionStatus( address, status );
    str.append( "Connection status: " ).append( ConnectionStatusToString( status ) );
    if( status == ConnectionStatus::ServiceConnected )
    {
        uint32_t sample;
        uint8_t channel;
        Bluetooth::BluetoothA2DPSink::GetMoudle()->GetAudioConfig( address, sample, channel );
        str.append( "; Address: " ).append( address.ToString() );
        str.append( "; Sample rate: " ).append( std::to_string( sample ) );
        str.append( "; Channel count: " ).append( std::to_string( channel ) );
    }

    str.append( "\n" );
    m_statusContent->SetString( std::move( str ) );
}

