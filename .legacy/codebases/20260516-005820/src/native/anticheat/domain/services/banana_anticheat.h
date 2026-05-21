#ifndef BANANA_ANTICHEAT_CONTRACT_H
#define BANANA_ANTICHEAT_CONTRACT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum BananaTrustState
    {
        BANANA_TRUST_NEW = 0,
        BANANA_TRUST_ESTABLISHED = 1,
        BANANA_TRUST_ELEVATED_OBSERVATION = 2,
        BANANA_TRUST_SUSPICIOUS = 3,
        BANANA_TRUST_RESTRICTED = 4,
        BANANA_TRUST_QUARANTINED = 5,
        BANANA_TRUST_FLAGGED = 6
    } BananaTrustState;

    typedef struct BananaRiskConfidence
    {
        int risk_score;
        int confidence_score;
    } BananaRiskConfidence;

    typedef enum BananaOpMode
    {
        BANANA_OPMODE_SOFT_ENFORCEMENT = 0,
        BANANA_OPMODE_ACTIVE_ENFORCEMENT = 1
    } BananaOpMode;

    typedef struct BananaAnticheatPolicyConfig
    {
        int suspicious_threshold;
        int elevated_threshold;
        int restricted_threshold;
        int quarantine_threshold;
        int flagged_threshold;
        BananaOpMode op_mode;
    } BananaAnticheatPolicyConfig;

    typedef struct BananaAnticheatMetrics
    {
        int active_sessions;
        int trust_tier_counts[7];
        int replay_rejections;
        int challenge_successes;
        int challenge_failures;
        int telemetry_parse_failures;
        int anomaly_events;
        int enforcement_actions;
        int behavioral_validation_hits;
    } BananaAnticheatMetrics;

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ANTICHEAT_CONTRACT_H */