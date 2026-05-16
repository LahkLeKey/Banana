#include "../../src/native/wrapper/banana_wrapper.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

int main(void)
{
    const char *session = "sess-anticheat-01";
    int rc = 0;
    int score = 0;
    int flagged = 0;
    uint32_t hash_before = 0;
    uint32_t hash_after = 0;

    rc = banana_anticheat_reset_session(session);
    assert(rc == BANANA_OK);

    rc = banana_anticheat_submit_usermode_heartbeat(session,
                                                    1,
                                                    10,
                                                    0,
                                                    0,
                                                    0,
                                                    0,
                                                    &score,
                                                    &flagged);
    assert(rc == BANANA_OK);
    assert(score == 0);
    assert(flagged == 0);

    rc = banana_anticheat_get_session_status(session, &score, &flagged, &hash_before);
    assert(rc == BANANA_OK);
    assert(hash_before != 0u);

    rc = banana_anticheat_submit_usermode_heartbeat(session,
                                                    1,
                                                    9,
                                                    1,
                                                    1,
                                                    1,
                                                    1,
                                                    &score,
                                                    &flagged);
    assert(rc == BANANA_OK);
    assert(score >= 8);
    assert(flagged == 1);

    rc = banana_anticheat_submit_driver_telemetry(session,
                                                  1,
                                                  1,
                                                  1,
                                                  1,
                                                  &score,
                                                  &flagged);
    assert(rc == BANANA_OK);
    assert(score >= 16);
    assert(flagged == 1);

    rc = banana_anticheat_get_session_status(session, &score, &flagged, &hash_after);
    assert(rc == BANANA_OK);
    assert(hash_after != hash_before);

    rc = banana_anticheat_submit_driver_telemetry(session,
                                                  2,
                                                  -1,
                                                  0,
                                                  0,
                                                  &score,
                                                  &flagged);
    assert(rc == BANANA_INVALID_ARGUMENT);

    printf("[native-unit] anti-cheat scoring and heartbeat integrity ok\n");
    return 0;
}
