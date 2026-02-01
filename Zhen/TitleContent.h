#pragma once
#include "Event.h"
#include "BasicContent.h"
#include "global.h"

#include <memory>
#include <string>

class LIBZHEN_API TitleContent : public BasicContent
{

public:

    TitleContent();

    virtual ~TitleContent(){}

    std::string GetPrintableString()const override;

    virtual void SetTitle( std::string a_tile )
    {
        m_title = std::move( a_tile );
    }

    virtual std::string GetTitle()const
    {
        return m_title;
    }

private:

    std::string m_title;

};

