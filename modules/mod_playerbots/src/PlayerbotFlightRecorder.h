/*
 * This file is part of the Legends of Azeroth Pandaria Project.
 *
 * A deliberately small, synchronous breadcrumb recorder for diagnosing
 * playerbot crashes.  Keep this interface independent of AI-owned pointers:
 * callers may use it immediately before code which can despawn a target.
 */

#ifndef _PLAYERBOT_FLIGHT_RECORDER_H
#define _PLAYERBOT_FLIGHT_RECORDER_H

#include <string>

class Player;

namespace PlayerbotFlightRecorder
{
    void Record(Player* bot, char const* phase,
        std::string const& detail = std::string());
}

#endif
