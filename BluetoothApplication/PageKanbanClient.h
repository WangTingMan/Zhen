#pragma once
#include "Zhen/BasePage.h"
#include "Zhen/OptionContent.h"
#include "Zhen/StrandCommandScheduler.h"

class PageKanbanClient : public BasePage
{

public:

    PageKanbanClient();

    ~PageKanbanClient();

    std::string GetPrintableString()const override;

    bool OnEvent( std::shared_ptr<Event> a_event ) override;

private:

    bool HandleConnectRequest
        (
        std::string const& a_input
        );

    std::shared_ptr<OptionContent> m_content;
    StrandCommandScheduler m_commandScheduler;
};

