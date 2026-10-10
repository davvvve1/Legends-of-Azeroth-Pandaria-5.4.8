#include "FollowMasterStrategy.h"

#include "Playerbots.h"

NextAction** FollowMasterStrategy::getDefaultActions()
{
    return NextAction::array(0,
        new NextAction("lead instance", 2.0f),
        new NextAction("follow", 1.0f), nullptr);
}
