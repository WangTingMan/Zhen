#pragma once
#include <string>

class AbstractModule
{

public:

    virtual ~AbstractModule();

    virtual int Init();

    virtual void Stop();

    virtual void Release();

    virtual std::string GetModuleName()const final
    {
        return m_name;
    }

protected:

    virtual void SetModuleName( std::string a_name )final
    {
        m_name = a_name;
    }

private:

    std::string m_name{ "NULL" };
};

