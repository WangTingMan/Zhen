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

    boost::signals2::connection ConnectToPagePoped
        (
        std::function<void()> a_function
        )
    {
        m_pagePoped.disconnect_all_slots();
        return m_pagePoped.connect( a_function );
    }

    bool OnEvent( std::shared_ptr<Event> a_event ) override;

    void SetEnable
        (
        bool a_enable = true
        )
    {
        m_enable = a_enable;
    }

    void DisconnectPopedSlots()
    {
        m_pagePoped.disconnect_all_slots();
    }

    /**
     * If use do not make a decision in a_timeout time, then this confirm page
     * will choose Yes when a_yesAsDefault set to true, otherwise will choose No.
     * [in] a_yesAsDefault Yes option as default option
     * [in] a_timeout Timeout time
     */
    void SetDefaultOption
        (
        bool a_yesAsDefault = true,
        std::chrono::seconds a_timeout = std::chrono::seconds{5}
        );

private:

    void OnPageToPoped
        (
        PageManager::PageChangedSignalType  a_type,         //<! The changed type
        std::shared_ptr<BasePage>           a_page,         //< Which page to be operated
        bool                                a_inPageStack   //< The page will be in the page stack or not
        );

    void HandleYesOptionChoosed();

    void HandleNoOptionChoosed();

    bool HandleTimeoutTimerEvent();

    std::shared_ptr<OptionContent> m_optionContent;
    bool m_enable = true;
    std::chrono::seconds m_timeout;
    uint32_t m_countdown;
    bool m_yesAsDefault = true;
    std::size_t m_yesOtionId;
    std::size_t m_noOptuonId;
    std::string m_yesTitle;
    std::string m_noTitle;
    boost::signals2::connection m_pageToPopedConnection;
    boost::signals2::connection m_timeoutTimerConnection;
    boost::signals2::signal<void()> m_yesChoosed;
    boost::signals2::signal<void()> m_noChoosed;
    boost::signals2::signal<void()> m_pagePoped;
};

