#pragma once

#include "IState .h"

class State : public IState
{

public:

    void enter(){}

    void exit(){}

    bool processMessage( std::shared_ptr<Message> msg )
    {
        return false;
    }

    std::string getName()
    {
        return "";
    }
};

