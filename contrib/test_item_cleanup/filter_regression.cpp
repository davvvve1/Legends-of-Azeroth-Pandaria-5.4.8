#include "../../src/server/game/Entities/Item/TestItemFilter.h"
#include <cassert>
int main()
{
    assert(IsInternalTestOrVipItem(17886, "QA Test Ring +285 Spell Damage"));
    assert(IsInternalTestOrVipItem(24358, "QATest +1000 Spell Dmg Ring"));
    assert(IsInternalTestOrVipItem(18968, "Ring of Critical Testing"));
    assert(IsInternalTestOrVipItem(18800, "TEST 1H Amberseal Keeper"));
    assert(IsInternalTestOrVipItem(1, "Armor (JEFFTEST)"));
    assert(IsInternalTestOrVipItem(17, "Martin Fury"));
    assert(IsInternalTestOrVipItem(991000, "VIP3 PvE Tank Faceguard"));
    assert(IsInternalTestOrVipItem(24115, "TestProspectIronOre"));
    assert(!IsInternalTestOrVipItem(24448, "Battle Tested Blade"));
    assert(!IsInternalTestOrVipItem(18361, "The Greatest Race of Hunters"));
    assert(!IsInternalTestOrVipItem(6472, "Stinging Viper"));
    assert(!IsInternalTestOrVipItem(9437, "Untested Basilisk Sample"));
    assert(!IsInternalTestOrVipItem(38608, "Tester's Notes"));
    assert(!IsInternalTestOrVipItem(37636, "Helm of Cheated Fate"));
    assert(!IsInternalTestOrVipItem(8523, "Field Testing Kit"));
    assert(!IsInternalTestOrVipItem(8527, "Sealed Field Testing Kit"));
    assert(!IsInternalTestOrVipItem(19971, "High Test Eternium Fishing Line"));
    assert(!IsInternalTestOrVipItem(69217, "Test Pilot Britches"));
}
