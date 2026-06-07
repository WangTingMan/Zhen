#pragma once
#include "Zhen/BasePage.h"
#include "Zhen/StringContent.h"
#include "Zhen/StrandCommandScheduler.h"

#include "BT/Adaptor.h"

#include <mutex>

class OptionContent;

class PageHandfreeAg : public BasePage
{

public:

    PageHandfreeAg();

    ~PageHandfreeAg();

    std::string GetPrintableString()const override;

    bool OnEvent( std::shared_ptr<Event> a_event ) override;

    // Handle input event from the user
private:

    bool HandleConnectRequest
        (
        std::string const& a_input
        );

    bool HandleDisconnectRequest
        (
        std::string const& a_input
        );

    bool HandleDialNumber
        (
        std::string const& a_input
        );

    bool HandleAnswerInComingCall
        (
        std::string const& a_input
        );

    bool HandleSplitCall
        (
        std::string const& a_input
        );

private:

    void UpdateStatusContent();

private:

    void HandleConnectionStatusChanged();

    void HandleCallListChanged();

    void HandleShowStatusPage();

private:

    std::shared_ptr<OptionContent> m_content;
    std::shared_ptr<StringContent> m_statusContent;
    boost_ns::signals2::connection m_connectionStatusChangedConnection;
    boost_ns::signals2::connection m_connectionCallListChangedConnection;
    boost_ns::signals2::connection m_connectionOperatorChangedConnection;
    StrandCommandScheduler m_commandScheduler;
    BluetoothAddress m_connectedDevice;
};

