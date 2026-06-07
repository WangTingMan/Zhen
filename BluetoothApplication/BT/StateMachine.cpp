#include "StateMachine.h"
#include "IState .h"
#include "System.h"

#include <base/time/time.h>

void StateMachine::LogRec::update
    (
    std::shared_ptr < StateMachine > sm,
    std::shared_ptr < Message> msg,
    std::string info,
    std::shared_ptr<IState> state,
    std::shared_ptr<IState> orgState,
    std::shared_ptr<IState> dstState
    )
{
    mSm = sm;
    mTime = System::currentTimeMillis();
    mWhat = ( msg ) ? msg->what : 0;
    mInfo = info;
    mState = state;
    mOrgState = orgState;
    mDstState = dstState;
}

std::string StateMachine::LogRec::toString()
{
    std::string sb;
    sb.append( "time=" );

    base::Time::NowFromSystemTime();
    std::string timeNowStr;//%tm-%td %tH:%tM:%tS.%tL

    sb.append( timeNowStr );
    sb.append( " processed=" );
    sb.append( mState ? mState->getName() : "<null>" );
    sb.append( " org=" );
    sb.append( mOrgState ? mOrgState->getName() : "<null>" );
    sb.append( " dest=" );
    sb.append( mDstState ? mDstState->getName() : "<null>" );
    sb.append( " what=" );
    std::string what = mSm ? mSm->getWhatToString( mWhat ) : "";
    if( what.empty() )
    {
        sb.append( std::to_string( mWhat ) );
    }
    else
    {
        sb.append( what );
    }
    if( mInfo.empty() )
    {
        sb.append( " " );
        sb.append( mInfo );
    }
    return sb;
}
