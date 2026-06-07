#include "PagePan.h"
#include "BT/BluetoothPAN.h"

#include "Zhen/OptionContent.h"

PagePan::PagePan()
{
    m_content = std::make_shared<OptionContent>();

    SetTitle("PAN");

    m_content->AddOption("Connect",
        [this]()mutable
        {
            m_commandScheduler.SetExecutor(std::bind(
                &PagePan::HandleConnectRequest,
                this,
                std::placeholders::_1)
            );
            m_commandScheduler.start();
        });
    m_content->AddOption("Disconnect",
        [this]()mutable
        {
            m_commandScheduler.SetExecutor(std::bind(
                &PagePan::HandleDisconnectRequest,
                this,
                std::placeholders::_1)
            );
            m_commandScheduler.start();
        });

    m_statusContent = std::make_shared<StringContent>();
    m_statusContent->SetTitle("Stauts");
}

PagePan::~PagePan()
{

}

std::string PagePan::GetPrintableString()const
{
    PagePan* page = const_cast<PagePan*> (this);

    std::string str = BasePage::GetPrintableString();
    str.append(m_content->GetPrintableString());
    str.append(m_statusContent->GetPrintableString());
    return str;
}

bool PagePan::OnEvent(std::shared_ptr<Event> a_event)
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

bool PagePan::HandleConnectRequest
    (
    std::string const& a_input
    )
{
    bool done = false;

    if (0 == m_commandScheduler.GetStepNumber())
    {
        std::list<RemoteDevice>const& pairedDevices = Adaptor::GetInstance().GetPairedDevices();
        if (pairedDevices.empty())
        {
            done = true;
            std::cout << "There is no paired device to connect\n";
        }
        else
        {
            int i = 1;
            for (auto it = pairedDevices.begin(); it != pairedDevices.end(); ++it, ++i)
            {
                std::cout << i << ". " << it->name << std::endl;
            }
            std::cout << "Enter the number of remote device:\n";
            m_commandScheduler.SetStepNumber(1);
        }
    }
    else if (1 == m_commandScheduler.GetStepNumber())
    {
        m_commandScheduler.AddParameter(a_input);
        m_commandScheduler.SetStepNumber(2);
    }

    if (!done && 1 == m_commandScheduler.GetNumParameters())
    {
        std::string numberString = std::any_cast<std::string>(m_commandScheduler.GetParameter(0));
        int number = 0;
        try
        {
            number = std::stoi(numberString);
            if (number < 1)
            {
                std::cout << "Wrong index, enter again:\n";
                m_commandScheduler.RemoveLastParameter();
                m_commandScheduler.SetStepNumber(1);
                done = false;
                return done;
            }

            BluetoothAddress address;
            std::list<RemoteDevice>const& pairedDevices = Adaptor::GetInstance().GetPairedDevices();
            if (pairedDevices.size() < number)
            {
                std::cout << "Wrong index. Enter again:\n";
                m_commandScheduler.RemoveLastParameter();
                m_commandScheduler.SetStepNumber(1);
                done = false;
                return done;
            }
            std::list<RemoteDevice>::const_iterator it = pairedDevices.begin();
            std::advance(it, number - 1);
            address = it->address;
            Bluetooth::BluetoothPAN::GetMoudle()->Connect(address);
            m_commandScheduler.BackToBeginning();
            done = true;
        }
        catch (std::exception& e)
        {
            std::cout << "Error: " << e.what() << ", enter again:";
            m_commandScheduler.RemoveLastParameter();
            m_commandScheduler.SetStepNumber(1);
            done = false;
        }
    }
    return done;
}

bool PagePan::HandleDisconnectRequest
    (
    std::string const& a_input
    )
{
    bool done = false;
    BluetoothAddress a_address;
    ConnectionStatus a_status = ConnectionStatus::ServiceDisconnected;
    auto devs = Bluetooth::BluetoothPAN::GetMoudle()->GetConnectedDevices();

    if (devs.size() > 0)
    {
        Bluetooth::BluetoothPAN::GetMoudle()->Disconnect(devs[0]);
    }
    done = true;
    return done;
}
