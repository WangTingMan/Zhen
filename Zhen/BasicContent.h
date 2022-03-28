#pragma once
#include "Event.h"

#include <memory>
#include <string>

class BasicContent : public std::enable_shared_from_this<BasicContent>
{

public:

    virtual ~BasicContent() {}

    virtual bool OnEvent( std::shared_ptr<Event> a_event ) = 0;

    virtual std::string GetPrintableString()const = 0;
};
