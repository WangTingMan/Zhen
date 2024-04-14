#ifndef BASE_PAGE_H__
#define BASE_PAGE_H__

#include <string>
#include <memory>
#include <vector>

#include "Event.h"
#include "menu.h"
#include "PageManager.h"
#include "TitleContent.h"

class BasePage : public TitleContent
{

public:

    enum class MenuPosition
    {
        Top,
        Bottom,
        TopBottom
    };

    std::string GetPrintableString()const override;

    template<typename T,
        typename = typename std::enable_if<
        std::is_constructible<std::string, T>::value,
        std::true_type>::type
        >
        void SetTitle( T&& a_title )
    {
        static_assert( std::is_constructible<std::string, T>::value, "std::string cannot construct with a_input!" );
        m_tile = std::string( std::forward<T>( a_title ) );
    }

    void show();

    virtual bool OnEvent( std::shared_ptr<Event> a_event );

    /**
     * This page will be shown onto UI. Just notify page to do something or prepare something
     */
    virtual void HandleWillShow();

    /**
     * This page will be hided from UI. Just notify page to do something or prepare something
     */
    virtual void HandleWillHide();

    void AddMenu( Menu&& a_menu, MenuPosition a_postion = MenuPosition::Bottom );

    void AddMenuAction( std::shared_ptr<Action> a_action );

    int GetPageStackPosition()const
    {
        return m_stackPosition;
    }

    BasePage()
    {
        AddDefaultActionToMenu();
    }

    ~BasePage()
    {
        m_combineFreshTimerConnection.disconnect();
    }

    /**
     * Notify this page need fresh onto screen.
     * if a_combine equals true, then the freshing action will be executed a_combineTime
     * milliseconds later.
     */
    void PageNeedFresh
        (
        bool a_combine = false,
        uint32_t a_combineTime = 1000
        );

    template<typename T,
        typename = typename std::enable_if<
        std::is_constructible<std::string, T>::value,
        std::true_type>::type
        >
        void SetPageName( T&& a_pageName )
    {
        static_assert( std::is_constructible<std::string, T>::value, "std::string cannot construct with a_input!" );
        m_pageName = std::string( std::forward<T>( a_pageName ) );
    }

    std::string const& GetPageName()const
    {
        return m_pageName;
    }

protected:

    void SetMenuPos( MenuPosition a_postion = MenuPosition::Bottom )
    {
        m_position = a_postion;
    }

private:

    friend class PageManager;

    void SetStackPosition( int a_pos )
    {
        m_stackPosition = a_pos;
    }

    std::string GeneratePrintableTitle()const;

    void AddDefaultActionToMenu();

    std::string m_tile;

    Menu m_menu;

    MenuPosition m_position = MenuPosition::TopBottom;

    int m_stackPosition = -1;

    bool m_hasRegisteredFreshPageEvent = false;

    std::string m_pageName;

    boost::signals2::connection m_combineFreshTimerConnection;
};

#endif

