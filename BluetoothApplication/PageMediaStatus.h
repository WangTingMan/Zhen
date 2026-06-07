#pragma once
#include "Zhen/BasePromptedPage.h"

class PageMediaStatus : public BasePromptedPage
{

public:

    PageMediaStatus();

    ~PageMediaStatus();

private:

    void HandleMediaStatusChanged();

    void RefreshMediaStatus();

private:

    boost_ns::signals2::connection m_postChangedConnection;
};

