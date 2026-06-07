#pragma once
#include "Zhen/BasePage.h"
#include "Zhen/OptionContent.h"
#include "Zhen/StrandCommandScheduler.h"

class PageGatt : public BasePage
{

public:

    PageGatt();

    ~PageGatt();

    std::string GetPrintableString()const override;

    bool OnEvent( std::shared_ptr<Event> a_event ) override;

private:

    void GotoANCSPage();

    void GotoKanbanServerPage();

    void GotoKanbanClientPage();

    std::shared_ptr<OptionContent> m_content;
    std::shared_ptr<BasePage> m_ancsClientPage;
    std::shared_ptr<BasePage> m_kanbanServerPage;
    std::shared_ptr<BasePage> m_kanbanClientPage;
};


