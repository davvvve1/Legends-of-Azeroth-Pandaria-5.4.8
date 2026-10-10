#ifndef PLAYERBOT_GROUP_FOLLOW_FORMATION_H
#define PLAYERBOT_GROUP_FOLLOW_FORMATION_H

#include <cstddef>

namespace GroupFollowFormation
{
struct Offset
{
    float forward;
    float sideways;
};

inline Offset GetOffset(bool tank, std::size_t slot)
{
    // Keep tanks far enough ahead to meet the next pack before the rest of
    // the group, while healers and damage dealers remain behind the leader.
    // Additional tanks retain stable three-yard rows behind the first tank.
    float const row = static_cast<float>(slot / 3) * 3.0f;
    float const sideways = static_cast<float>(static_cast<int>((slot + 1) % 3) - 1) * 3.0f;
    return {tank ? 8.0f + row : -4.0f - row, sideways};
}

inline Offset GetHealerOffset(std::size_t slot)
{
    // Healers occupy their own compact row immediately behind the movement
    // anchor. They must not be pushed into a later ranged row merely because
    // several damage dealers happen to have lower character GUIDs.
    float const row = static_cast<float>(slot / 3) * 3.0f;
    float const sideways = static_cast<float>(static_cast<int>((slot + 1) % 3) - 1) * 2.5f;
    return {-2.5f - row, sideways};
}
}

#endif
