#include "PageRfcomm.h"

#include "Zhen/OptionContent.h"

#include <BT/rfcomm/BluetoothRfcomm.h>

PageRfcomm::PageRfcomm()
{
    SetPageName( "PageRfcomm" );
    SetTitle( "RFCOMM" );

    m_content = std::make_shared<OptionContent>();
    m_content->AddOption( "Connect",
        [ this ]()mutable
        {
            m_commandScheduler.SetExecutor( std::bind(
                &PageRfcomm::HandleConnectRequest,
                this,
                std::placeholders::_1 )
            );
            m_commandScheduler.start();
        } );
}

PageRfcomm::~PageRfcomm()
{

}

std::string PageRfcomm::GetPrintableString()const
{
    std::string str = BasePage::GetPrintableString();
    str.append( m_content->GetPrintableString() );
    return str;
}

bool PageRfcomm::OnEvent( std::shared_ptr<Event> a_event )
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

bool PageRfcomm::HandleConnectRequest
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
            Bluetooth::BluetoothRfcomm::GetMoudle()->Connect( address );
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

void PageRfcomm::UpdateStatusContent()
{

}
