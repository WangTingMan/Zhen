#include "TitleContent.h"
#include "PageManager.h"
#include <iostream>

TitleContent::TitleContent()
{
}

std::string TitleContent::GetPrintableString()const
{
    int width = PageManager::GetInstance().GetDisplayAreaWidth();
    int titleStartPos = ( width - m_title.size() ) / 2;
    if( titleStartPos < 0 )
    {
        titleStartPos = 0;
    }
    std::string title_stash( titleStartPos, '-' );
    std::string title_show;
    title_show.append( title_stash ).append( m_title ).append( std::move( title_stash ) ).append( "\n" );
    return title_show;
}


