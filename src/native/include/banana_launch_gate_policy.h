#ifndef BANANA_LAUNCH_GATE_POLICY_H
#define BANANA_LAUNCH_GATE_POLICY_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum banana_launch_gate_mode {
	BANANA_LAUNCH_GATE_MODE_DEVELOPMENT = 0,
	BANANA_LAUNCH_GATE_MODE_STAGING_UAT = 1,
	BANANA_LAUNCH_GATE_MODE_PRODUCTION_STEAM_PACKAGE = 2,
} banana_launch_gate_mode;

typedef enum banana_launch_gate_reason_code {
	BANANA_LAUNCH_GATE_REASON_OK = 0,
	BANANA_LAUNCH_GATE_REASON_UNLINKED_ACCOUNT = 1,
	BANANA_LAUNCH_GATE_REASON_SUSPENDED_OR_RESTRICTED = 2,
	BANANA_LAUNCH_GATE_REASON_STALE_SESSION = 3,
	BANANA_LAUNCH_GATE_REASON_STEAM_UNAVAILABLE = 4,
	BANANA_LAUNCH_GATE_REASON_API_UNAVAILABLE = 5,
	BANANA_LAUNCH_GATE_REASON_OFFLINE_UNVERIFIABLE = 6,
	BANANA_LAUNCH_GATE_REASON_UNKNOWN_MODE = 7,
} banana_launch_gate_reason_code;

typedef struct banana_launch_gate_policy {
	banana_launch_gate_mode mode;
	int allow_non_steam_startup;
	int allow_cached_verification;
	int allow_override_context;
} banana_launch_gate_policy;

#ifdef __cplusplus
}
#endif

#endif
