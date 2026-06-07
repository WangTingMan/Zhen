#pragma once
#include <string>
#include <memory>

class Message;

class IState
{

    /**
     * Returned by processMessage to indicate the message was processed.
     */
    static constexpr bool HANDLED = true;

    /**
     * Returned by processMessage to indicate the message was NOT processed.
     */
    static constexpr bool NOT_HANDLED = false;

public:

    /**
     * Called when a state is entered.
     */
    virtual void enter() = 0;

    /**
     * Called when a state is exited.
     */
    virtual void exit() = 0;

    /**
     * Called when a message is to be processed by the
     * state machine.
     *
     * This routine is never reentered thus no synchronization
     * is needed as only one processMessage method will ever be
     * executing within a state machine at any given time. This
     * does mean that processing by this routine must be completed
     * as expeditiously as possible as no subsequent messages will
     * be processed until this routine returns.
     *
     * @param msg to process
     * @return HANDLED if processing has completed and NOT_HANDLED
     *         if the message wasn't processed.
     */
    virtual bool processMessage( std::shared_ptr<Message> msg ) = 0;

    /**
     * Name of State for debugging purposes.
     *
     * @return name of state.
     */
    virtual std::string getName() = 0;
};
