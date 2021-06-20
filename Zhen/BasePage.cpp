#include "BasePage.h"
#include "PageManager.h"
#include "KeyBoardInputEvent.h"
#include "ExecutbleEvent.h"
#include <iostream>

std::string BasePage::GetPrintableString()const
{
    std::string ret;
    ret = GeneratePrintableTitle();

    int width = PageManager::GetInstance().GetDisplayAreaWidth();
    bool emptyMenu = m_menu.empty();
    bool showTopMenu = MenuPosition::Top == m_position || MenuPosition::TopBottom == m_position;
    if( !emptyMenu && showTopMenu )
    {
        ret.append( m_menu.GetPrintableString() );
    }
    return ret;
}

void BasePage::show()
{
    std::string r = GetPrintableString();
    bool emptyMenu = m_menu.empty();
    int width = PageManager::GetInstance().GetDisplayAreaWidth();
    bool showBottomMenu = MenuPosition::Bottom == m_position || MenuPosition::TopBottom == m_position;
    if( !emptyMenu && showBottomMenu )
    {
        r.append( m_menu.GetPrintableString() );
    }

    std::string str( width, '-' );
    r.append( str ).append( "\n" );

    std::cout << r;
}

void BasePage::AddDefaultActionToMenu()
{
    m_menu.SetTitle( "Menu" );
    m_menu.AddAction( Action::MakeBackAction() );
    m_menu.AddAction( Action::MakeQuitAction() );
}

std::string BasePage::GeneratePrintableTitle()const
{
    int width = PageManager::GetInstance().GetDisplayAreaWidth();
    int titleStartPos = ( width - m_tile.size() ) / 2;
    if( titleStartPos < 0 )
    {
        titleStartPos = 0;
    }
    std::string title_stash( titleStartPos, '=' );
    std::string title_show;
    title_show.append( title_stash ).append( m_tile ).append( std::move( title_stash ) );
    if( title_show.size() < width )
    {
        title_show.push_back( '=' );
    }
    title_show.push_back( '\n' );
    return title_show;
}

bool BasePage::OnEvent( std::shared_ptr<Event> a_event )
{
    if( !a_event )
    {
        return true;
    }

    int ret = false;
    switch( a_event->GetEventType() )
    {
    case Event::EventType::KeyBoardInputEvent:
        break;
    default:
        break;
    }

    if( Event::EventType::KeyBoardInputEvent == a_event->GetEventType() )
    {
        ret= m_menu.OnEvent( a_event );
    }

    if( !ret )
    {

    }
    return ret;
}

void BasePage::AddMenu( Menu&& a_menu, MenuPosition a_postion )
{
    Menu menu = std::move( a_menu );
    m_position = a_postion;
}

void BasePage::AddMenuAction( std::shared_ptr<Action> a_action )
{
    m_menu.AddAction( a_action );
}

void BasePage::PageNeedFresh()
{
    if( m_hasRegisteredFreshPageEvent )
    {
        return;
    }

    std::shared_ptr< BasePage > page = std::dynamic_pointer_cast< BasePage >( shared_from_this() );
    std::shared_ptr< ExecutbleEvent > event = std::make_shared<ExecutbleEvent>(
        [=]()
        {
            if( PageManager::GetInstance().GetTopPage() == page )
            {
                PageManager::GetInstance().FreshTopPageShow();
            }
            page->m_hasRegisteredFreshPageEvent = false;
        }
        );

    PageManager::GetInstance().PostEvent( std::move( event ) );
    m_hasRegisteredFreshPageEvent = true;
}

