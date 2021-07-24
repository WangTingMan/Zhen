#pragma once
#include "BasePromptedPage.h"
#include "PageManager.h"
#include "OptionContent.h"

class ConfirmPage : public BasePromptedPage
{

public:

    ConfirmPage();

    std::string GetPrintableString()const override;

    ~ConfirmPage();

    boost::signals2::connection ConnectToYesChoosed
        (
        std::function<void()> a_function
        )
    {
        m_yesChoosed.disconnect_all_slots();
        return m_yesChoosed.connect( a_function );
    }

    boost::signals2::connection ConnectToNoChoosed
        (
        std::function<void()> a_function
        )
    {
        m_noChoosed.disconnect_all_slots();
        return m_noChoosed.connect( a_function );
    }

    bool OnEvent( std::shared_ptr<Event> a_event ) override;

    void SetEnable
        (
        bool a_enable = true
        )
    {
        m_enable = a_enable;
    }

private:

    void OnPageToPoped
        (
        PageManager::PageChangedSignalType  a_type,         //<! The changed type
        std::shared_ptr<BasePage>           a_page,         //< Which page to be operated
        bool                                a_inPageStack   //< The page will be in the page stack or not
        );

    void HandleYesOptionChoosed();

    void HandleNoOptionChoosed();

    std::shared_ptr< OptionContent > m_optionContent;
    bool m_enable = true;

    boost::signals2::connection m_pageToPopedConnection;
    boost::signals2::signal<void()> m_yesChoosed;
    boost::signals2::signal<void()> m_noChoosed;
};

