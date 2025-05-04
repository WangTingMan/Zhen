#pragma once
#include "Zhen/BasePage.h"

class OptionContent;
class StringContent;

class HomePage : public BasePage
{

public:

    HomePage();

    std::string GetPrintableString()const override;

    bool OnEvent( std::shared_ptr<Event> a_event ) override;

private:

    void UpdateStatusContent();

    bool HandleClockTimer();

    void StartShowClock();

    void StopShowClock();

    void StopShowClockWithConfim();

    std::size_t m_confirmId;
    std::string m_currentTimeString;
    std::shared_ptr<OptionContent> m_content;
    std::shared_ptr<StringContent> m_statusContent;
    boost_ns::signals2::connection m_clockUpdateTimerConnection;
    std::shared_ptr<BasePage> m_confirmPage;
};

