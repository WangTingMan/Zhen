#include "PageHandfreeAg.h"
#include "PageHandfreeStatus.h"

#include <Bt/Adaptor.h>
#include <BT/BluetoothHFAg.h>

#include "Zhen/OptionContent.h"

#include <iostream>

using Bluetooth::BluetoothHFAG;

PageHandfreeAg::PageHandfreeAg()
{
    SetPageName( "PageHandfree" );
    SetTitle( "Handsfree" );
    m_content = std::make_shared<OptionContent>();
    m_content->AddOption( "Connect",
                          [ this ]()mutable
                          {
                              m_commandScheduler.SetExecutor( std::bind(
                                  &PageHandfreeAg::HandleConnectRequest,
                                  this,
                                  std::placeholders::_1 )
                              );
                              m_commandScheduler.start();
                          } );

    m_content->AddOption( "Disconnect",
                          [ this ]()mutable
                          {
                              m_commandScheduler.SetExecutor( std::bind(
                                  &PageHandfreeAg::HandleDisconnectRequest,
                                  this,
                                  std::placeholders::_1 )
                              );
                              m_commandScheduler.start();
                          } );

    m_content->AddOption( "Answer Incoming Call",
                          [ this ]()mutable
                          {
                              m_commandScheduler.SetExecutor( std::bind(
                                  &PageHandfreeAg::HandleAnswerInComingCall,
                                  this,
                                  std::placeholders::_1 )
                              );
                              m_commandScheduler.start();
                          } );

    m_content->AddOption( "Dial Number",
                          [ this ]()mutable
                          {
                              m_commandScheduler.SetExecutor( std::bind(
                                  &PageHandfreeAg::HandleDialNumber,
                                  this,
                                  std::placeholders::_1 )
                              );
                              m_commandScheduler.start();
                          } );

    m_content->AddOption( "Split Call",
                          [ this ]()mutable
                          {
                              m_commandScheduler.SetExecutor( std::bind(
                                  &PageHandfreeAg::HandleSplitCall,
                                  this,
                                  std::placeholders::_1 )
                              );
                              m_commandScheduler.start();
                          } );

    m_content->AddOption( "Show current status", std::bind( &PageHandfreeAg::HandleShowStatusPage, this ) );

    m_statusContent = std::make_shared<StringContent>();
    m_statusContent->SetTitle( "Stauts" );
}

PageHandfreeAg::~PageHandfreeAg()
{
    m_connectionStatusChangedConnection.disconnect();
    m_connectionCallListChangedConnection.disconnect();
    m_connectionOperatorChangedConnection.disconnect();
}

std::string PageHandfreeAg::GetPrintableString()const
{
    PageHandfreeAg* page = const_cast<PageHandfreeAg*> ( this );
    page->UpdateStatusContent();

    std::string ret;
    ret = BasePage::GetPrintableString();
    ret.append( m_content->GetPrintableString() );
    ret.append( m_statusContent->GetPrintableString() );
    return ret;
}

bool PageHandfreeAg::OnEvent( std::shared_ptr<Event> a_event )
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

bool PageHandfreeAg::HandleConnectRequest
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
        std::string numberString = std::any_cast<std::string>( m_commandScheduler.GetParameter( 0 ) );
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
            BluetoothHFAG::GetMoudle()->Connect( address );
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

bool PageHandfreeAg::HandleDisconnectRequest
    (
    std::string const& a_input
    )
{
    bool done = false;
    ConnectionStatus a_status = ConnectionStatus::ServiceDisconnected;

    if( ConnectionStatus::ServiceConnected == a_status )
    {
        BluetoothHFAG::GetMoudle()->Disconnect( m_connectedDevice );
    }
    done = true;
    return done;
}

bool PageHandfreeAg::HandleDialNumber
    (
    std::string const& a_input
    )
{
    bool done = false;
    ConnectionStatus a_status = ConnectionStatus::ServiceDisconnected;

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
        std::string numberString = std::any_cast<std::string>( m_commandScheduler.GetParameter( 0 ) );
        BluetoothHFAG::GetMoudle()->DialNewNumber( numberString );
        done = true;
    }
    return done;
}

bool PageHandfreeAg::HandleAnswerInComingCall
    (
    std::string const& a_input
    )
{
    bool done = false;
    ConnectionStatus a_status = ConnectionStatus::ServiceDisconnected;
    done = true;
    return done;
}

bool PageHandfreeAg::HandleSplitCall
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

void PageHandfreeAg::UpdateStatusContent()
{
    ConnectionStatus status = ConnectionStatus::ServiceDisconnected;
    auto hf = BluetoothHFAG::GetMoudle();
    status = hf->GetConnectionState();

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
        str.append( "; Sco connection: " ).append( ConnectionStatusToString( status ) );
    }
    str.push_back( '\n' );
    m_statusContent->SetString( std::move( str ) );
}

void PageHandfreeAg::HandleConnectionStatusChanged()
{
    PageNeedFresh();
}

void PageHandfreeAg::HandleCallListChanged()
{
    PageNeedFresh();
}

void PageHandfreeAg::HandleShowStatusPage()
{
    auto page = std::make_shared<PageHandfreeStatus>();
    PageManager::GetInstance().PushPage( page );
}
