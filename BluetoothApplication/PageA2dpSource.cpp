#include "PageA2dpSource.h"

#include "Zhen/OptionContent.h"
#include "Zhen/ExecutbleEvent.h"

#include <BT/a2dp/BluetoothA2DPSource.h>

#include <iostream>

PageA2dpSource::PageA2dpSource()
{
    SetPageName( "PageA2DPSource" );
    SetTitle( "A2DPSource" );
    m_content = std::make_shared<OptionContent>();
    m_content->AddOption( "Connect",
        [this]()mutable
        {
            m_commandScheduler.SetExecutor( std::bind(
                &PageA2dpSource::HandleConnectRequest,
                this,
                std::placeholders::_1 )
        );
        m_commandScheduler.start();
        } );

    m_content->AddOption( "Disconnect",
        [this]()mutable
        {
            m_commandScheduler.SetExecutor( std::bind(
                &PageA2dpSource::HandleDisconnectRequest,
                this,
                std::placeholders::_1 )
        );
        m_commandScheduler.start();
        } );

    m_content->AddOption( "Set Active Device",
        std::bind( &PageA2dpSource::HandleSetActiveDevice, this ) );

    m_content->AddOption( "Set Silence Device",
        std::bind( &PageA2dpSource::HandleSetSilenceDevice, this ) );

    m_content->AddOption( "Request start stream",
        std::bind( &PageA2dpSource::HandleRequestStartStream, this ) );

    m_content->AddOption( "Request suspend stream",
        std::bind( &PageA2dpSource::HandleRequestSuspendStream, this ) );

    m_content->AddOption( "Request stop stream",
        std::bind( &PageA2dpSource::HandleRequestStopStream, this ) );

    m_statusContent = std::make_shared<StringContent>();
    m_statusContent->SetTitle( "Stauts" );

    auto source = Bluetooth::BluetoothA2DPSource::GetMoudle();
    boost_ns::signals2::connection connection;
    connection = source->ConnectToConnectionStatusChanged( [this]( BluetoothAddress, ConnectionStatus )
        {
        HandleConnectionStatusChanged();
        } );
    m_signalConnections.push_back( connection );

    connection = source->ConnectToStreamStatusChanged( [ this ]()mutable
                                                       {
                                                           PageNeedFresh();
                                                       } );
    m_signalConnections.push_back( connection );
}

PageA2dpSource::~PageA2dpSource()
{
    for( auto ele : m_signalConnections )
    {
        ele.disconnect();
    }
}

std::string PageA2dpSource::GetPrintableString()const
{
    PageA2dpSource* page = const_cast< PageA2dpSource* > ( this );
    page->UpdateStatusContent();

    std::string ret;
    ret = BasePage::GetPrintableString();
    ret.append( m_content->GetPrintableString() );
    ret.append( m_statusContent->GetPrintableString() );
    return ret;
}

bool PageA2dpSource::OnEvent( std::shared_ptr<Event> a_event )
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

void PageA2dpSource::UpdateStatusContent()
{
    BluetoothAddress address;
    ConnectionStatus status;
    auto source = Bluetooth::BluetoothA2DPSource::GetMoudle();
    source->GetConnectionStatus( address, status );

    std::string statusString( "Device: " );
    statusString.append( address.ToString() ).append( "; status: " ).append( ConnectionStatusToString( status ) );

    auto stramStatus = source->GetStreamStatus();
    switch( stramStatus )
    {
    case Bluetooth::BluetoothA2DPSource::Started:
        statusString.append( ", stream started." );
        break;
    case Bluetooth::BluetoothA2DPSource::Stopped:
        statusString.append( ", stream stopped." );
        break;
    case Bluetooth::BluetoothA2DPSource::RemoteSuspended:
        statusString.append( ", stream suspended." );
        break;
    default:
        break;
    }
    statusString.append( "\n" );
    m_statusContent->SetString( std::move( statusString ) );
}

bool PageA2dpSource::HandleConnectRequest
    (
    std::string const& a_input
    )
{
    bool done = false;

    if( 0 == m_commandScheduler.GetStepNumber() )
    {
        std::list<RemoteDevice>const& pairedDevices = Adaptor::GetInstance().GetPairedDevices();
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
            std::list<RemoteDevice>const& pairedDevices = Adaptor::GetInstance().GetPairedDevices();
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
            Bluetooth::BluetoothA2DPSource::GetMoudle()->Connect( address );
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

bool PageA2dpSource::HandleDisconnectRequest
    (
    std::string const& a_input
    )
{
    BluetoothAddress address;
    ConnectionStatus status;
    auto source = Bluetooth::BluetoothA2DPSource::GetMoudle();
    source->GetConnectionStatus( address, status );
    source->disconnect( address );
    return true;
}

bool PageA2dpSource::HandleSetActiveDevice()
{
    BluetoothAddress address;
    ConnectionStatus status;
    auto source = Bluetooth::BluetoothA2DPSource::GetMoudle();
    source->GetConnectionStatus( address, status );
    if( status == ConnectionStatus::ServiceConnected )
    {
        source->setActiveDevice( address );
    }
    else
    {
        std::cout << "No connected device.\n";
    }
    return true;
}

bool PageA2dpSource::HandleSetSilenceDevice()
{
    BluetoothAddress address;
    ConnectionStatus status;
    auto source = Bluetooth::BluetoothA2DPSource::GetMoudle();
    source->GetConnectionStatus( address, status );
    if( status == ConnectionStatus::ServiceConnected )
    {
        source->setSilenceDevice( address );
    }
    else
    {
        std::cout << "No connected device.\n";
    }
    return true;
}


bool PageA2dpSource::HandleRequestStartStream()
{
    auto source = Bluetooth::BluetoothA2DPSource::GetMoudle();
    source->StartAudioStream();
    return true;
}

bool PageA2dpSource::HandleRequestSuspendStream()
{
    auto source = Bluetooth::BluetoothA2DPSource::GetMoudle();
    source->SuspendAudioStream();
    return true;
}

bool PageA2dpSource::HandleRequestStopStream()
{
    auto source = Bluetooth::BluetoothA2DPSource::GetMoudle();
    source->StopAudioStream();
    return true;
}

void PageA2dpSource::HandleConnectionStatusChanged()
{
    this->PageNeedFresh();
}
