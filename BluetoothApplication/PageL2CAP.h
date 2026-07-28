#pragma once

#include "Zhen/BasePage.h"
#include "Zhen/StringContent.h"
#include "Zhen/StrandCommandScheduler.h"

#include "BT/Adaptor.h"

#include <mutex>

class OptionContent;
class StringContent;
class PageL2CAP : public BasePage
{
public:

    PageL2CAP();

    ~PageL2CAP();

    std::string GetPrintableString()const override;

    bool OnEvent( std::shared_ptr<Event> a_event ) override;

    //Handle the user input event
private:

    bool HandleConnectRequest
        (
        std::string const& a_input
        );

    //Handle the signal changed from BT
private:

    void ListenOn( uint16_t a_psm, bool a_use_br_edr );

private:

    void UpdateStatusContent();

    void Connect( BluetoothAddress const& a_addr );

    std::shared_ptr<OptionContent> m_content;
    std::shared_ptr<StringContent> m_statusContent;
    StrandCommandScheduler m_commandScheduler;
    bool m_try_to_connect_br = true;
};