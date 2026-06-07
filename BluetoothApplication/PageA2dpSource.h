#pragma once
#include "Zhen/BasePage.h"
#include "Zhen/StringContent.h"
#include "Zhen/StrandCommandScheduler.h"

#include "BT/Adaptor.h"

#include <mutex>

class OptionContent;

class PageA2dpSource : public BasePage
{

public:

    PageA2dpSource();

    ~PageA2dpSource();

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

    bool HandleSetActiveDevice();

    bool HandleSetSilenceDevice();

    bool HandleRequestStartStream();

    bool HandleRequestSuspendStream();

    bool HandleRequestStopStream();

private:

    void UpdateStatusContent();

private:

    void HandleConnectionStatusChanged();

private:

    std::shared_ptr<OptionContent> m_content;
    std::shared_ptr< StringContent > m_statusContent;
    boost_ns::signals2::connection m_connectionStatusChangedConnection;
    boost_ns::signals2::connection m_connectionCallListChangedConnection;
    boost_ns::signals2::connection m_connectionOperatorChangedConnection;
    std::vector<boost_ns::signals2::connection> m_signalConnections;
    StrandCommandScheduler m_commandScheduler;
    BluetoothAddress m_connectedDevice;
};

