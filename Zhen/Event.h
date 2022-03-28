#pragma once
class Event
{

public:

    enum class EventType
    {
        InvalidEvent = 0,
        KeyBoardInputEvent,
        ExecutbleEvent
    };

    virtual ~Event(){}

    EventType GetEventType()const
    {
        return m_type;
    }

protected:

    void SetEventType( EventType a_type )
    {
        m_type = a_type;
    }

private:

    EventType m_type = EventType::InvalidEvent;
};

