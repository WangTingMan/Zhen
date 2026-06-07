#include "PageKanbanClient.h"

#include <Bt/Adaptor.h>

PageKanbanClient::PageKanbanClient()
{
    SetTitle( "Kanban Client" );

    m_content = std::make_shared<OptionContent>();
    m_content->AddOption( "Connect",
        [ this ]()mutable
        {
            m_commandScheduler.SetExecutor( std::bind(
                &PageKanbanClient::HandleConnectRequest,
                this,
                std::placeholders::_1 )
                );
            m_commandScheduler.start();
        } );

}

PageKanbanClient::~PageKanbanClient()
{

}

std::string PageKanbanClient::GetPrintableString()const
{
    std::string str = BasePage::GetPrintableString();
    str.append( m_content->GetPrintableString() );
    return str;
}

bool PageKanbanClient::OnEvent( std::shared_ptr<Event> a_event )
{
    bool r = false;

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

bool PageKanbanClient::HandleConnectRequest
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
            //BluetoothHF::GetMoudle()->Connect( address );
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
