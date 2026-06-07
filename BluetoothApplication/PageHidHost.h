#pragma once
#include "Zhen/BasePage.h"
#include "Zhen/StringContent.h"
#include "Zhen/StrandCommandScheduler.h"

#include "BT/Adaptor.h"

#include <mutex>

class OptionContent;
class StringContent;
class PageHidHost : public BasePage
{
public:

    PageHidHost();

    ~PageHidHost();

    std::string GetPrintableString()const override;

    bool OnEvent( std::shared_ptr<Event> a_event ) override;

//Handle the user input event
private:

    bool HandleConnectRequest
        (
        std::string const& a_input
        );
    
    bool HandleDisconnectRequest
        (
        std::string const& a_input
        );

    bool HandleGetReport
        (
        std::string const& a_input
        );

//Handle the signal changed from BT
private:

    void HandleConnectionStatusChanged
        (
        BluetoothAddress a_address,
        ConnectionStatus a_status
        );

private:

    void UpdateStatusContent();

    std::vector<boost_ns::signals2::connection> m_signalConnections;
    std::shared_ptr<OptionContent> m_content;
    std::shared_ptr<StringContent> m_statusContent;
    StrandCommandScheduler m_commandScheduler;
    boost_ns::signals2::connection m_connectionStatusChangedConnection;
    boost_ns::signals2::connection m_audioCondigChangedConnection;

};

