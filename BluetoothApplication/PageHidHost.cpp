#include "PageHidHost.h"

#include "Zhen/OptionContent.h"
#include "Zhen/StringContent.h"
#include "Zhen/BasePromptedPage.h"

#include <BT/HID/BluetoothHidHost.h>

PageHidHost::PageHidHost()
{
    m_content = std::make_shared<OptionContent>();
    m_statusContent = std::make_shared<StringContent>();
    m_statusContent->SetTitle("Stauts");

    SetTitle("HID Host");
    m_content->AddOption("Connect",
        [this]()mutable
        {
            m_commandScheduler.SetExecutor(std::bind(
                &PageHidHost::HandleConnectRequest,
                this,
                std::placeholders::_1)
            );
            m_commandScheduler.start();
        });
    m_content->AddOption("Disconnect",
        [this]()mutable
        {
            m_commandScheduler.SetExecutor(std::bind(
                &PageHidHost::HandleDisconnectRequest,
                this,
                std::placeholders::_1)
            );
            m_commandScheduler.start();
        });
    m_content->AddOption("Get Report",
        [this]()mutable
        {
            m_commandScheduler.SetExecutor(std::bind(
                &PageHidHost::HandleGetReport,
                this,
                std::placeholders::_1)
            );
            m_commandScheduler.start();
        });

    auto hid_host = Bluetooth::BluetoothHidHost::GetMoudle();
    boost_ns::signals2::connection connection;
    connection = hid_host->ConnectToConnectionStatusChanged(
        std::bind(&PageHidHost::HandleConnectionStatusChanged, this, std::placeholders::_1, std::placeholders::_2));
    m_signalConnections.push_back(connection);

}

PageHidHost::~PageHidHost()
{
    for (auto ele : m_signalConnections)
    {
        ele.disconnect();
    }
}

std::string PageHidHost::GetPrintableString()const
{
    PageHidHost* page = const_cast<PageHidHost*> (this);
    page->UpdateStatusContent();

    std::string str = BasePage::GetPrintableString();
    str.append(m_content->GetPrintableString());
    str.append(m_statusContent->GetPrintableString());
    return str;
}

bool PageHidHost::OnEvent(std::shared_ptr<Event> a_event)
{
    bool r = m_commandScheduler.OnEvent(a_event);

    if (!r)
    {
        r = m_content->OnEvent(a_event);
    }

    if (!r)
    {
        BasePage::OnEvent(a_event);
    }

    return r;
}

bool PageHidHost::HandleConnectRequest
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
            Bluetooth::BluetoothHidHost::GetMoudle()->Connect( address );
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

bool PageHidHost::HandleDisconnectRequest
    (
    std::string const& a_input
    )
{
    BluetoothAddress address;
    ConnectionStatus status = ConnectionStatus::ServiceDisconnected;
    Bluetooth::BluetoothHidHost::GetMoudle()->GetConnectionStatus(address, status);
    Bluetooth::BluetoothHidHost::GetMoudle()->Disconnect(address);
    return true;
}

bool PageHidHost::HandleGetReport
    (
    std::string const& a_input
    )
{
    BluetoothAddress address;
    ConnectionStatus status = ConnectionStatus::ServiceDisconnected;
    Bluetooth::BluetoothHidHost::GetMoudle()->GetConnectionStatus(address, status);
    Bluetooth::BluetoothHidHost::GetMoudle()->GetReport(address);
    return true;
}

void PageHidHost::HandleConnectionStatusChanged
    (
    BluetoothAddress a_address,
    ConnectionStatus a_status
    )
{
    PageNeedFresh(true);
}

void PageHidHost::UpdateStatusContent()
{
    std::string str;
    BluetoothAddress address;
    ConnectionStatus status = ConnectionStatus::ServiceDisconnected;

    Bluetooth::BluetoothHidHost::GetMoudle()->GetConnectionStatus(address, status);
    str.append("Connection status: ").append(ConnectionStatusToString(status));
    if (status == ConnectionStatus::ServiceConnected)
    {
        str.append("; Address: ").append(address.ToString());
    }

    str.append("\n");
    m_statusContent->SetString(std::move(str));
}

