#pragma once
#include "TitleContent.h"

#include <functional>
#include <map>
#include <vector>

class OptionContent : public TitleContent
{

public:

    bool OnEvent( std::shared_ptr<Event> a_event )override;

    std::string AddOption
        (
        std::string a_option,
        std::function<void()> a_optionSelectCallBack = std::function<void()>()
        );

    std::string GetPrintableString()const override;

    void ClearOption()
    {
        m_opthions.clear();
    }

    void SetExecutable
        (
        bool a_executable = true
        )
    {
        m_executable = a_executable;
    }

    std::size_t GetOptionCount()const
    {
        return m_opthions.size();
    }

private:

    struct Opthion
    {
        std::string index;
        std::string opthion;
        std::function<void()> callback;
    };

    std::vector<Opthion> m_opthions;
    bool m_executable = true;
};

