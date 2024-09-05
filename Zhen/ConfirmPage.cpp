#include "ConfirmPage.h"
#include "PageManager.h"

ConfirmPage::ConfirmPage()
{
    SetPageName( "ConfirmPage" );
    m_pageToPopedConnection = PageManager::GetInstance().ConnectPageChanged
        (
            std::bind( &ConfirmPage::OnPageToPoped, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 )
        );

    m_optionContent = std::make_shared<OptionContent>();
    m_optionContent->SetTitle( "Please select one action" );
    m_optionContent->AddOption( "Yes", std::bind( &ConfirmPage::HandleYesOptionChoosed, this ) );
    m_optionContent->AddOption( "No", std::bind( &ConfirmPage::HandleNoOptionChoosed, this ) );
}

void ConfirmPage::OnPageToPoped
    (
    PageManager::PageChangedSignalType  a_type,
    std::shared_ptr<BasePage>           a_page,
    bool                                a_inPageStack
    )
{
    std::shared_ptr<ConfirmPage> page = std::dynamic_pointer_cast<ConfirmPage>(a_page);
    if (!page)
    {
        return;
    }


    if( this == page.get() &&
        a_type == PageManager::PageChangedSignalType::FINISHED_POP_OUT
      )
    {
        m_pagePoped();
    }
}

ConfirmPage::~ConfirmPage()
{
    m_pageToPopedConnection.disconnect();
}

std::string ConfirmPage::GetPrintableString()const
{
    std::string ret = BasePromptedPage::GetPrintableString();
    ret.append( m_optionContent->GetPrintableString() );
    return ret;
}

bool ConfirmPage::OnEvent( std::shared_ptr<Event> a_event )
{
    bool ret = false;
    ret = m_optionContent->OnEvent( a_event );
    if( ret )
    {
        return ret;
    }

    ret = BasePage::OnEvent( a_event );
    return ret;
}

void ConfirmPage::HandleYesOptionChoosed()
{
    if( m_enable )
    {
        m_yesChoosed();
    }
}

void ConfirmPage::HandleNoOptionChoosed()
{
    if( m_enable )
    {
        m_noChoosed();
    }
}