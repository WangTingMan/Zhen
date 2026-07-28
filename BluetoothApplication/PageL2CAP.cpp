#include "PageL2CAP.h"

#include "Zhen/OptionContent.h"
#include <BT/Adaptor.h>
#include <BT/StackLayer/bluetooth_l2cap_coc_interface.h>

PageL2CAP::PageL2CAP()
{
    SetPageName( "PageRfcomm" );
    SetTitle( "RFCOMM" );

    m_content = std::make_shared<OptionContent>();
    m_content->AddOption( "Connect BR/EDR L2CAP COC",
        [ this ]()mutable
        {
            m_try_to_connect_br = true;
            m_commandScheduler.SetExecutor( std::bind(
                &PageL2CAP::HandleConnectRequest,
                this,
                std::placeholders::_1 )
            );
            m_commandScheduler.start();
        } );
    m_content->AddOption( "Connect LE L2CAP COC",
        [ this ]()mutable
        {
            m_try_to_connect_br = false;
            m_commandScheduler.SetExecutor( std::bind(
                &PageL2CAP::HandleConnectRequest,
                this,
                std::placeholders::_1 )
            );
            m_commandScheduler.start();
        } );

    m_content->AddOption( "Listen LE L2CAP COC", std::bind(&PageL2CAP::ListenOn, this, 0x81, false) );
}

PageL2CAP::~PageL2CAP()
{

}

std::string PageL2CAP::GetPrintableString()const
{
    std::string str = BasePage::GetPrintableString();
    str.append( m_content->GetPrintableString() );
    return str;
}

bool PageL2CAP::OnEvent( std::shared_ptr<Event> a_event )
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

bool PageL2CAP::HandleConnectRequest
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
            Connect( address );
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

void PageL2CAP::ListenOn( uint16_t a_psm, bool a_use_br_edr )
{
    auto _interface = Adaptor::GetInstance().GetLowLevelInterface( bluetooth_interface_type::l2cap_coc );
    auto l2cap_interface = std::dynamic_pointer_cast< bluetooth_l2cap_coc_interface >( _interface );
    l2cap_interface->listen(a_psm, a_use_br_edr);
}

void PageL2CAP::UpdateStatusContent()
{

}

void PageL2CAP::Connect( BluetoothAddress const& a_addr )
{
    constexpr uint16_t br_psm_value = 0x8091;
    constexpr uint16_t le_psm_value = 0x0091;
    auto _interface = Adaptor::GetInstance().GetLowLevelInterface(bluetooth_interface_type::l2cap_coc);
    auto l2cap_interface = std::dynamic_pointer_cast<bluetooth_l2cap_coc_interface>(_interface );
    l2cap_interface->Connect(a_addr, m_try_to_connect_br ? br_psm_value : le_psm_value, m_try_to_connect_br );
}

