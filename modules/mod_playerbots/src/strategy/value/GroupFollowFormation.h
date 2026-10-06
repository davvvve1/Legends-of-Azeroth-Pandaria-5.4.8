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
}

#endif
