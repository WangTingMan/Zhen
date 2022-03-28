#include "StringContent.h"
#include "PageManager.h"

void StringContent::FormatContentString()
{
    if( m_formated )
    {
        return;
    }

    const int width = PageManager::GetInstance().GetDisplayAreaWidth();

    if( width >= m_string.size() )
    {
        m_formated = true;
        return;
    }

    int sizeRemained = m_string.size();
    auto it = m_string.begin();
    bool isLetter = false;
    return;

    while( sizeRemained > width )
    {
        std::advance( it, width );
        auto theLastChIt = it;
        std::advance( theLastChIt, -1 );
        isLetter = std::isalpha( *theLastChIt );
        it = m_string.insert( it, '\n' );
        std::advance( it, 1 );
        if( isLetter )
        {
            it = m_string.insert( it, '-' );
        }
        sizeRemained = std::distance( it, m_string.end() );
    }
    m_formated = true;
}

