#include "HomePage.h"

#include "Zhen/OptionContent.h"
#include "Zhen/StringContent.h"
#include "Zhen/PageManager.h"
#include "Zhen/ConfirmPage.h"

#include <chrono>
#include <ctime>
#include <sstream>

HomePage::HomePage()
{
    SetPageName( "HomePage" );
    SetTitle( "Home Page" );

    m_content = std::make_shared<OptionContent>();
    m_content->AddOption( "Start Show Clock", std::bind(
        &HomePage::StartShowClock, this) );
    m_content->AddOption( "Stop Show Clock", std::bind(
        &HomePage::StopShowClock, this ) );
    m_confirmId = m_content->AddOption( "Stop Show Clock With Confirm", std::bind(
        &HomePage::StopShowClockWithConfim, this ) );
    m_content->SetVisiable( m_confirmId, false );

    m_statusContent = std::make_shared<StringContent>();
    m_statusContent->SetTitle( "Stauts" );
}

std::string HomePage::GetPrintableString()const
{
    HomePage* page = const_cast<HomePage*> ( this );
    page->UpdateStatusContent();

    std::string ret = BasePage::GetPrintableString();
    ret.append( m_content->GetPrintableString() );
    ret.append( m_statusContent->GetPrintableString() );
    return ret;
}

bool HomePage::OnEvent( std::shared_ptr<Event> a_event )
{
    bool r = m_content->OnEvent( a_event );
    if( !r )
    {
        r = BasePage::OnEvent( a_event );
    }
    return r;
}

void HomePage::UpdateStatusContent()
{
    std::string status_str;
    if( m_clockUpdateTimerConnection.connected() )
    {
        status_str.append( "Time showing." );
    }
    else
    {
        status_str.append( "Time not showing." );
    }

    status_str.push_back( ' ' );
    status_str.append( m_currentTimeString );
    status_str.push_back( '\n' );

    m_statusContent->SetString( std::move( status_str ) );
}

bool HomePage::HandleClockTimer()
{
    auto now_ = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t( now_ );
#ifdef _MSC_VER
    std::tm now_time;
    localtime_s( &now_time, &t );
#else
    std::tm tm = *std::localtime( &t );
#endif

    std::stringstream ss;
    ss.imbue( std::locale( "ja_JP.utf8" ) );
    ss << std::put_time( &now_time, "%c %Z" );

    m_currentTimeString = ss.str();
    PageNeedFresh();
    // If we do not want to trigger timer event any more,
    // then we can return false here.
    return true;
}

void HomePage::StartShowClock()
{
    if( !m_clockUpdateTimerConnection.connected() )
    {
        m_content->SetVisiable( m_confirmId );
        m_clockUpdateTimerConnection = PageManager::GetInstance()
            .connectTimerTo( std::bind( &HomePage::HandleClockTimer, this ), 500, false );
    }
}

void HomePage::StopShowClock()
{
    m_clockUpdateTimerConnection.disconnect();
    m_content->SetVisiable( m_confirmId, false );
    PageNeedFresh();
}

void HomePage::StopShowClockWithConfim()
{
    if( !m_clockUpdateTimerConnection.connected() )
    {
        m_content->SetVisiable( m_confirmId, false );
        return;
    }

    std::shared_ptr<ConfirmPage> confirmPage;
    if( m_confirmPage )
    {
        confirmPage = std::dynamic_pointer_cast<ConfirmPage>( m_confirmPage );
    }
    else
    {
        confirmPage = std::make_shared<ConfirmPage>();
        m_confirmPage = confirmPage;
    }

    std::weak_ptr<ConfirmPage> watcherPage( confirmPage );
    confirmPage->SetTitle( "Stop Show Time" );
    confirmPage->SetEnable();
    confirmPage->ConnectToYesChoosed
        (
        [ this, watcherPage ]()mutable
        {
            if( !watcherPage.expired() )
            {
                auto page = watcherPage.lock();
                page->SetEnable( false );
                PageManager::GetInstance().PopPage( page );
            }
            StopShowClock();
        }
        );

    confirmPage->ConnectToNoChoosed
        (
        [ this, watcherPage ]()mutable
        {
            if( !watcherPage.expired() )
            {
                auto page = watcherPage.lock();
                page->SetEnable( false );
                PageManager::GetInstance().PopPage( page );
            }
        }
        );

    std::string content;
    content.append( "Please confirm that you need stop showing time?\n" );
    confirmPage->SetContentString( content );
    confirmPage->SetDefaultOption( true, std::chrono::seconds( 10 ) );
    PageManager::GetInstance().PushPage( std::move( confirmPage ) );
}
