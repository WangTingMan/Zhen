#pragma once

#include "Zhen/BasePromptedPage.h"

class PageHandfreeStatus : public BasePromptedPage
{

public:

    PageHandfreeStatus();

    ~PageHandfreeStatus();

private:

    void RefreshHandfreeStatus();

private:

    boost_ns::signals2::connection m_connectStatusConnection;
    boost_ns::signals2::connection m_connectCallListChangeConnection;
};
