#include "anticheat_scoring.h"
#include "../entities/anticheat_session.h"
#include <stdint.h>

void anticheat_scoring_update(const char *session_id, int risk_delta, int confidence_delta) {
    BananaAnticheatSession *session = anticheat_session_find(session_id, 1);
    if (!session) return;
    session->score += risk_delta;
    if (session->score < 0) session->score = 0;
    // Confidence increases if evidence is corroborated, decays otherwise
    extern int anticheat_evidence_correlate(const char *session_id);
    if (anticheat_evidence_correlate(session_id))
        session->confidence_score += 2;
    else
        session->confidence_score -= 1;
    if (session->confidence_score < 0) session->confidence_score = 0;
}

BananaRiskConfidence anticheat_scoring_get(const char *session_id) {
    BananaRiskConfidence rc = {0, 0};
    BananaAnticheatSession *session = anticheat_session_find(session_id, 0);
    if (session) {
        rc.risk_score = session->score;
        rc.confidence_score = session->confidence_score;
    }
    return rc;
}