#include "PageGatt.h"
#include "PageKanban.h"
#include "PageAppleANCS.h"
#include "PageKanbanClient.h"

PageGatt::PageGatt()
{
    SetTitle( "GATT" );

    m_content = std::make_shared<OptionContent>();
    m_content->AddOption( "ANCS Client", std::bind( &PageGatt::GotoANCSPage, this ) );
    m_content->AddOption( "Kanban Server", std::bind( &PageGatt::GotoKanbanServerPage, this ) );
    m_content->AddOption( "Kanban Client", std::bind( &PageGatt::GotoKanbanClientPage, this ) );
}

PageGatt::~PageGatt()
{

}

std::string PageGatt::GetPrintableString()const
{
    std::string str = BasePage::GetPrintableString();
    str.append( m_content->GetPrintableString() );
    return str;
}

bool PageGatt::OnEvent( std::shared_ptr<Event> a_event )
{
    bool r = false;

    if (!r)
    {
        r = m_content->OnEvent( a_event );
    }

    if (!r)
    {
        BasePage::OnEvent( a_event );
    }

    return r;
}

void PageGatt::GotoANCSPage()
{
    if (!m_ancsClientPage)
    {
        m_ancsClientPage = std::make_shared<PageAppleANCS>();
    }

    std::shared_ptr<BasePage> page = m_ancsClientPage;
    PageManager::GetInstance().PushPage( std::move( page ) );
}

void PageGatt::GotoKanbanServerPage()
{
    if (!m_kanbanServerPage)
    {
        m_kanbanServerPage = std::make_shared<PageKanban>();
    }

    std::shared_ptr<BasePage> page = m_kanbanServerPage;
    PageManager::GetInstance().PushPage( std::move( page ) );
}

void PageGatt::GotoKanbanClientPage()
{
    if (!m_kanbanClientPage)
    {
        m_kanbanClientPage = std::make_shared<PageKanbanClient>();
    }

    std::shared_ptr<BasePage> page = m_kanbanClientPage;
    PageManager::GetInstance().PushPage( std::move( page ) );
}
