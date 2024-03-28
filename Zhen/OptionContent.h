#pragma once
#include "TitleContent.h"

#include <functional>
#include <map>
#include <vector>

class OptionContent : public TitleContent
{

public:

    static constexpr std::size_t s_invalid_id = 0u;

    bool OnEvent( std::shared_ptr<Event> a_event )override;

    std::size_t AddOption
        (
        std::string a_option,
        std::function<void()> a_optionSelectCallBack = std::function<void()>()
        );

    std::string GetPrintableString()const override;

    void ClearOption()
    {
        m_options.clear();
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
        return m_options.size();
    }

    /**
     * Set specific option visiable or not.
     * [in] a_id which option to set
     * [in] a_visiable visiable or not
     * [return] true: found and set; false: cannot find or already be a_visiable
     */
    bool SetVisiable
        (
        std::size_t a_id,
        bool a_visiable = true
        );

private:

    struct Option
    {
        std::size_t id;     // identification for this option
        std::string index;  // index for this option, type the index then the callback will be invoked
        std::string option_title;// title of this option
        std::function<void()> callback; // when actived, this callbeck will be executed
        bool visiable = true;
    };

    std::size_t m_current_id = 1u;
    std::vector<Option> m_options;
    bool m_executable = true;
};

