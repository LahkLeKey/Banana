#include "anticheat_config.h"
static BananaAnticheatPolicyConfig g_config = {8, 1, 0, 8, 1, BANANA_OPMODE_SOFT_ENFORCEMENT};
void anticheat_config_set(const BananaAnticheatPolicyConfig *config) {
    if (config) g_config = *config;
}
const BananaAnticheatPolicyConfig *anticheat_config_get(void) {
    return &g_config;
}
int anticheat_should_degrade(void) {
    return 0;
}
int anticheat_is_enforcement_active(void) {
    return g_config.op_mode == BANANA_OPMODE_ACTIVE_ENFORCEMENT;
}
