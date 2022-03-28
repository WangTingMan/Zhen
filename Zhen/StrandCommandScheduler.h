#pragma once
#include <functional>
#include <string>
#include <any>
#include <vector>
#include <memory>

class Event;
class StrandCommandScheduler
{

public:

    StrandCommandScheduler();

    void SetExecutor( std::function<bool( std::string const& )> a_callBack )
    {
        m_callBack = a_callBack;
    }

    bool OnEvent( std::shared_ptr<Event> a_event );

    void start();

    //! Add parameter to the command
    void AddParameter
        (
        const std::any& a_parameter //!< [in] Command parameter
        );

    void RemoveLastParameter()
    {
        m_parameters.pop_back();
    }

    //! Get parameter from the command
    std::any GetParameter
        (
        const std::size_t a_index //!< [in] Index to command parameter
        ) const;

    //! Get number of command parameters
    //! @return number of command parameters
    std::size_t GetNumParameters() const
    {
        return m_parameters.size();
    }

    //! Get parameter number that the command is waiting for
    //! @return waiting parameter number
    std::size_t GetStepNumber() const
    {
        return m_stepNumber;
    }

    //! Set parameter number that the command is waiting for
    void SetStepNumber
        (
        const std::size_t a_parameterNum //!< [in] Waiting parameter number
        )
    {
        m_stepNumber = a_parameterNum;
    }

    void clear();

    void BackToBeginning();

private:

    std::function<bool( std::string const& )> m_callBack;
    std::vector<std::any> m_parameters;
    std::size_t m_stepNumber = 0u;
    std::size_t m_executTimeCurrently = 0;
    std::size_t m_maxExecutionTimes = 5u;
};
