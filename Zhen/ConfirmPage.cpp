#include "ConfirmPage.h"
#include "PageManager.h"

ConfirmPage::ConfirmPage()
{
    m_countdown = 0;
    SetPageName( "ConfirmPage" );
    m_pageToPopedConnection = PageManager::GetInstance().ConnectPageChanged
        (
            std::bind( &ConfirmPage::OnPageToPoped, this, std::placeholders::_1,
                       std::placeholders::_2, std::placeholders::_3 )
        );

    m_optionContent = std::make_shared<OptionContent>();
    m_optionContent->SetTitle( "Please select one action" );
    m_yesTitle = "Yes";
    m_noTitle = "No";
    m_yesOtionId = m_optionContent->AddOption( m_yesTitle, std::bind( &ConfirmPage::HandleYesOptionChoosed, this ) );
    m_noOptuonId = m_optionContent->AddOption( m_noTitle, std::bind( &ConfirmPage::HandleNoOptionChoosed, this ) );
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

bool ConfirmPage::HandleTimeoutTimerEvent()
{
    if( m_countdown > 0 )
    {
        m_countdown--;
    }
    else
    {
        HandleYesOptionChoosed();
        if( m_timeoutTimerConnection.connected() )
        {
            m_timeoutTimerConnection.disconnect();
        }
    }

    std::string countdownStr = std::to_string( m_countdown );
    std::string optionTile = m_yesAsDefault ? m_yesTitle : m_noTitle;
    std::size_t otionId = m_yesAsDefault ? m_yesOtionId : m_noOptuonId;
    optionTile.append( "(" ).append( countdownStr ).append( ")" );
    m_optionContent->ChangedOptionTitle( otionId, optionTile );

    PageNeedFresh();
    return true;
}

void ConfirmPage::SetDefaultOption
    (
    bool a_yesAsDefault,
    std::chrono::seconds a_timeout
    )
{
    if( a_timeout.count() < 1 )
    {
        return;
    }

    if( m_timeoutTimerConnection.connected() )
    {
        m_timeoutTimerConnection.disconnect();
    }

    m_countdown = a_timeout.count() + 1;
    m_timeout = a_timeout;
    m_yesAsDefault = a_yesAsDefault;

    m_timeoutTimerConnection = PageManager::GetInstance()
        .connectTimerTo( std::bind( &ConfirmPage::HandleTimeoutTimerEvent,
                                    this ), 1000, true);

    HandleTimeoutTimerEvent();
}
