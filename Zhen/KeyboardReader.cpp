#include "KeyboardReader.h"
#include "PageManager.h"
#include "KeyBoardInputEvent.h"
#include <thread>
#include <iostream>

void ReadLineEventCooker( std::string&& a_line )
{
    std::string cmd = a_line;
    std::string::size_type  firstSpacePos = cmd.find_first_not_of( ' ' );
    if( std::string::npos != firstSpacePos )
    {
        cmd = cmd.erase( 0, firstSpacePos );
    }

    auto it = cmd.rbegin();
    for( ; it != cmd.rend(); ++it )
    {
        if( ' ' != *it )
        {
            break;
        }
    }
    cmd = cmd.erase( std::distance( it, cmd.rend() ), cmd.size() );
    std::shared_ptr<Event> trimedEvent = std::make_shared<KeyBoardInputEvent>( cmd );
    PageManager::GetInstance().PostEvent( std::move( trimedEvent ) );
}

KeyboardReader::KeyboardReader()
{
}

KeyboardReader::~KeyboardReader()
{
    stop();
}

void KeyboardReader::start()
{
    if( !isRunning )
    {
        mLineHandler = std::bind( &ReadLineEventCooker, std::placeholders::_1 );
        std::thread th( &KeyboardReader::read, this );
        mThread = std::move( th );
        isRunning = true;
    }
}

void KeyboardReader::stop()
{
    if( isRunning )
    {
        isGoingToStop = true;
        if( mThread.joinable() )
        {
            mThread.join();
        }
        isRunning = false;
    }
}

void KeyboardReader::read()
{
    std::string line;
    while( !isGoingToStop )
    {
        line.clear();
        std::getline( std::cin, line );
        if( mLineHandler )
        {
            mLineHandler( std::move( line ) );
        }
    }
}

