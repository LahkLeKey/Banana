// Policy/config domain
#ifndef BANANA_ANTICHEAT_CONFIG_H
#define BANANA_ANTICHEAT_CONFIG_H
#include "banana_anticheat.h"
void anticheat_config_set(const BananaAnticheatPolicyConfig *config);
const BananaAnticheatPolicyConfig *anticheat_config_get(void);
#endif