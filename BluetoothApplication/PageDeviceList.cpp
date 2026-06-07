#include "PageDeviceList.h"

#include "Zhen/OptionContent.h"
#include "Zhen/ExecutbleEvent.h"
#include "Zhen/ConfirmPage.h"

#include "BT/Adaptor.h"

#include <iostream>
#include <string>

PageDeviceList::PageDeviceList()
{
    SetPageName( "PageDeviceList" );
    m_content = std::make_shared<OptionContent>();
    m_content->SetTitle( "Command List" );
    m_content->AddOption( "Start device searching", std::bind( &PageDeviceList::StartSearchDevice, this, true ) );
    m_content->AddOption( "Stop device searching", std::bind( &PageDeviceList::StartSearchDevice, this, false ) );
    m_content->AddOption( "Pair with discovered device",
                        [this]()mutable
                        {
                            m_commandScheduler.clear();
                            m_commandScheduler.SetExecutor( std::bind(
                                &PageDeviceList::HanldePairWithRemoteDevice,
                                this,
                                std::placeholders::_1 )
                                );
                            m_commandScheduler.start();
                        } );
    m_content->AddOption( "Respond to incoming pairing request" );
    m_content->AddOption( "Cancel device pairing" );
    m_content->AddOption( "Delete paired device",
                        [this]()mutable
                        {
                            m_commandScheduler.clear();
                            m_commandScheduler.SetExecutor( std::bind(
                                &PageDeviceList::HanldeDeletePairedDevice,
                                this,
                                std::placeholders::_1 )
                                );
                            m_commandScheduler.start();
                        } );
    m_content->AddOption( "Generate OOB data", std::bind( &PageDeviceList::GenerateOobData, this ) );
    m_content->AddOption( "Submit OOB data" );

    m_searchedDeviceContent = std::make_shared<OptionContent>();
    m_searchedDeviceContent->SetTitle( "Device searched" );
    m_searchedDeviceContent->SetExecutable( false );

    m_PairedDeviceContent = std::make_shared<OptionContent>();
    m_PairedDeviceContent->SetTitle( "Device paired" );
    m_PairedDeviceContent->SetExecutable( false );

    m_StatusContent = std::make_shared<StringContent>();
    m_StatusContent->SetTitle( "Current Status" );

    SetTitle( "Device List" );

    m_deviceFoundSignalConnection = 
        Adaptor::GetInstance().ConnectDeviceFound
            (
            [this]()mutable
            {
                std::shared_ptr< ExecutbleEvent > event = 
                    std::make_shared<ExecutbleEvent>( std::bind( &PageDeviceList::HanldeNewDeviceFound, this ) );
                PageManager::GetInstance().PostEvent( event );
            }
            );
    m_sspPairingRequestConnection = 
        Adaptor::GetInstance().ConnectPairingRequest
            (
                std::bind
                    (
                    &PageDeviceList::HandleIncomingSspRequest,
                    this,
                    std::placeholders::_1,
                    std::placeholders::_2,
                    std::placeholders::_3
                    )
            );
    m_pincodeRequestConnection =
        Adaptor::GetInstance().ConnectPincodeRequest
        (
            std::bind
                (
                &PageDeviceList::HandleIncomingPincodeRequest,
                this,
                std::placeholders::_1,
                std::placeholders::_2,
                std::placeholders::_3
                )
        );
    m_pairedDeviceChangedConnection = 
        Adaptor::GetInstance().ConnectPairedDeviceChanged( std::bind( &PageDeviceList::HanldePairedDeviceChanged, this ) );
    m_pairingStateChangedConnection = Adaptor::GetInstance().ConnectPairingStateChanged( std::bind( &PageDeviceList::HandlePairingStateChanged, this ) );
    m_searchStateChangedConnection = Adaptor::GetInstance().ConnectDiscoveryStateChanged( std::bind( &PageDeviceList::HandleSearchStateChanged, this ) );
}

PageDeviceList::~PageDeviceList()
{
    m_deviceFoundSignalConnection.disconnect();
    m_sspPairingRequestConnection.disconnect();
    m_pairedDeviceChangedConnection.disconnect();
}

std::string PageDeviceList::GetPrintableString()const
{
    std::list<RemoteDevice>const &devices = Adaptor::GetInstance().GetFoundDevices();
    m_searchedDeviceContent->ClearOption();
    std::string foundDeviceStr;
    for( auto & ele : devices )
    {
        foundDeviceStr.clear();
        foundDeviceStr.append( ele.address.ToString() );
        foundDeviceStr.append( ": " );
        foundDeviceStr.append( ele.name );
        m_searchedDeviceContent->AddOption( foundDeviceStr );
    }

    std::list<RemoteDevice>const &pairedDevices = Adaptor::GetInstance().GetPairedDevices();
    m_PairedDeviceContent->ClearOption();
    std::string pairedDeviceStr;
    for( auto & ele : pairedDevices )
    {
        pairedDeviceStr.clear();
        pairedDeviceStr.append( ele.address.ToString() );
        pairedDeviceStr.append( ": " );
        pairedDeviceStr.append( ele.name );
        m_PairedDeviceContent->AddOption( pairedDeviceStr );
    }

    std::string ret = BasePage::GetPrintableString();
    ret.append( m_searchedDeviceContent->GetPrintableString() );
    ret.append( m_PairedDeviceContent->GetPrintableString() );
    ret.append( m_content->GetPrintableString() );
    ret.append( m_StatusContent->GetPrintableString() );
    return ret;
}

bool PageDeviceList::OnEvent( std::shared_ptr<Event> a_event )
{
    bool r = false;
    r = m_commandScheduler.OnEvent( a_event );
    if( r )
    {
        return true;
    }

    r = m_content->OnEvent( a_event );
    if( r )
    {
        return true;
    }

    r = BasePage::OnEvent( a_event );
    return r;
}

void PageDeviceList::StartSearchDevice( bool a_start )
{
    if( a_start )
    {
        Adaptor::GetInstance().StartSearch();
    }
    else
    {
        Adaptor::GetInstance().StopSearch();
    }
}

void PageDeviceList::GenerateOobData()
{
    Adaptor::GetInstance().GenerateOobData();
}

void PageDeviceList::HanldeNewDeviceFound()
{
    PageNeedFresh();
}

void PageDeviceList::HanldePairedDeviceChanged()
{
    PageNeedFresh();
}

bool PageDeviceList::HanldePairWithRemoteDevice( std::string const& a_input )
{
    bool done = false;
    if( 0 == m_searchedDeviceContent->GetOptionCount() )
    {
        done = true;
        std::cout << "No found remote devices.\n";
        return done;
    }

    if( 0 == m_commandScheduler.GetStepNumber() )
    {
        std::cout << "Enter the number of remote device:";
        m_commandScheduler.SetStepNumber( 1 );
    }
    else if( 1 == m_commandScheduler.GetStepNumber() )
    {
        m_commandScheduler.AddParameter( a_input );
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
                std::cout << "Wrong index, enter again:";
                m_commandScheduler.RemoveLastParameter();
                done = false;
                return done;
            }

            BluetoothAddress address;
            std::list<RemoteDevice>const &devices = Adaptor::GetInstance().GetFoundDevices();
            if( devices.size() < number )
            {
                std::cout << "Wrong index. Enter again:";
                m_commandScheduler.RemoveLastParameter();
                done = false;
                return done;
            }
            std::list<RemoteDevice>::const_iterator it = devices.begin();
            std::advance( it, number - 1 );
            address = it->address;
            Adaptor::GetInstance().PairWithDevice( address );
            done = true;
        }
        catch (std::exception& e)
        {
            std::cout << "Error: " << e.what() << ", enter again:";
            m_commandScheduler.RemoveLastParameter();
            done = false;
        }
    }
    return done;
}

bool PageDeviceList::HanldeDeletePairedDevice( std::string const& a_input )
{
    bool done = false;

    if( Adaptor::GetInstance().GetPairedDevices().empty() )
    {
        done = true;
        std::cout << "No found remote devices.\n";
        return done;
    }

    if( 0 == m_commandScheduler.GetStepNumber() )
    {
        std::cout << "Enter the number of paired device to delete:";
        m_commandScheduler.SetStepNumber( 1 );
    }
    else if( 1 == m_commandScheduler.GetStepNumber() )
    {
        m_commandScheduler.AddParameter( a_input );
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
                std::cout << "Wrong index, enter again:";
                m_commandScheduler.RemoveLastParameter();
                done = false;
                return done;
            }

            BluetoothAddress address;
            std::list<RemoteDevice>const& devices = Adaptor::GetInstance().GetPairedDevices();
            if( devices.size() < number )
            {
                std::cout << "Wrong index. Enter again:";
                m_commandScheduler.RemoveLastParameter();
                done = false;
                return done;
            }
            std::list<RemoteDevice>::const_iterator it = devices.begin();
            std::advance( it, number - 1 );
            address = it->address;
            Adaptor::GetInstance().DeletePairedDevice( address );
            done = true;
        }
        catch( std::exception& e )
        {
            std::cout << "Error: " << e.what() << ", enter again:";
            m_commandScheduler.RemoveLastParameter();
            done = false;
        }
    }

    return done;
}

void PageDeviceList::HandleIncomingSspRequest
    (
    BluetoothAddress    a_address,
    std::string         a_deviceName,
    uint32_t            a_passkey
    )
{
    std::shared_ptr<ConfirmPage> confirmPage;
    if( m_confirmPage )
    {
        confirmPage = std::dynamic_pointer_cast<ConfirmPage>( m_confirmPage );
    }
    else
    {
        confirmPage = std::make_shared<ConfirmPage>();
        m_confirmPage = confirmPage;
    }

    std::weak_ptr<ConfirmPage> watcherPage( confirmPage );
    confirmPage->SetTitle( "SSP Pairing confirm" );
    confirmPage->SetEnable();
    confirmPage->ConnectToYesChoosed
        (
        [this, a_address, watcherPage ]()mutable
        {
            if( !watcherPage.expired() )
            {
                auto page = watcherPage.lock();
                page->SetEnable( false );
                PageManager::GetInstance().PopPage( page );
            }
            Adaptor::GetInstance().SspPairingReply( a_address, true );
        }
        );

    confirmPage->ConnectToNoChoosed
        (
        [this, a_address, watcherPage ]()mutable
        {
            if( !watcherPage.expired() )
            {
                auto page = watcherPage.lock();
                page->SetEnable( false );
                PageManager::GetInstance().PopPage( page );
            }
            Adaptor::GetInstance().SspPairingReply( a_address, false );
        }
        );

    std::string number( std::to_string( a_passkey ) );
    if( number.size() != 6 )
    {
        int numberCnt = number.size();
        int diff = 6 - numberCnt;
        std::string modStr( diff, '0' );
        modStr.append( number );
        std::swap( number, modStr );
    }

    std::string content;
    content.append( "Received one SSP paring request. Please confirm the number showed is shown on the remote device\n" );
    content.append( "Address: " ).append( a_address.ToString() ).append( "\n" );
    content.append( "Device Name: " ).append( a_deviceName ).append( "\n" );
    content.append( "Number: " ).append( number ).append( "\n" );

    confirmPage->SetContentString( content );
    confirmPage->SetDefaultOption( true, std::chrono::seconds( 10 ) );
    PageManager::GetInstance().PushPage( std::move( confirmPage ) );
}

void PageDeviceList::HandleIncomingPincodeRequest
    (
    BluetoothAddress    a_address,
    std::string         a_deviceName,
    bool                a_limit
    )
{

}

void PageDeviceList::HandlePairingStateChanged()
{
    UpdateStatusContent();
}

void PageDeviceList::HandleSearchStateChanged()
{
    UpdateStatusContent();
}

void PageDeviceList::UpdateStatusContent()
{
    std::string status;
    BondState pairingState = Adaptor::GetInstance().GetPairingState();
    if( BondState::Bonding == pairingState )
    {
        status.append( "Pairing state: " ).append( "pairing. " );
    }
    else
    {
        if( m_confirmPage )
        {
            PageManager::GetInstance().PopPage( m_confirmPage );
        }
    }

    DiscoveryState searchState = Adaptor::GetInstance().GetSearchState();
    if( searchState == DiscoveryState::BT_DISCOVERY_STARTED )
    {
        status.append( "Search state: " ).append( "Searching. " );
    }
    else if( searchState == DiscoveryState::BT_DISCOVERY_STOPPED )
    {
        status.append( "Search state: " ).append( "Stopped. " );
    }

    if( !status.empty() )
    {
        status.push_back( '\n' );
    }
    m_StatusContent->SetString( status );
    PageNeedFresh();
}
