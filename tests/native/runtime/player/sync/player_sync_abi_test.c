#include "runtime/player/sync/player_sync_abi.h"

#include "../../support/test_support.h"

#include <string.h>

static char g_last_guid[40];
static int64_t g_last_seen_ms = 0;
static int64_t g_last_update_now_ms = 0;
static int64_t g_last_update_threshold_ms = 0;
static int g_deactivate_calls = 0;
static int g_count_active_result = 0;

void runtime_player_registry_mark_seen(const char *guid, int64_t current_time_ms)
{
    g_last_guid[0] = '\0';
    if (guid)
        strncpy(g_last_guid, guid, sizeof(g_last_guid) - 1);
    g_last_seen_ms = current_time_ms;
}

void runtime_player_registry_update_staleness(int64_t current_time_ms, int64_t stale_threshold_ms)
{
    g_last_update_now_ms = current_time_ms;
    g_last_update_threshold_ms = stale_threshold_ms;
}

void runtime_player_registry_deactivate_stale(void)
{
    g_deactivate_calls += 1;
}

int runtime_player_registry_count_active(void)
{
    return g_count_active_result;
}

static void reset_state(void)
{
    memset(g_last_guid, 0, sizeof(g_last_guid));
    g_last_seen_ms = 0;
    g_last_update_now_ms = 0;
    g_last_update_threshold_ms = 0;
    g_deactivate_calls = 0;
    g_count_active_result = 0;
}

static void test_mark_seen_forwards_guid_and_time(void **state)
{
    (void)state;
    reset_state();

    runtime_player_sync_abi_mark_seen("player-123", 12345);

    BANANA_TEST_ASSERT_STR_EQ(g_last_guid,
                              "player-123",
                              "mark_seen should forward guid to registry");
    BANANA_TEST_ASSERT_INT_EQ((int)g_last_seen_ms,
                              12345,
                              "mark_seen should forward timestamp to registry");
}

static void test_update_staleness_forwards_window_values(void **state)
{
    RuntimePlayerSyncWindow window;

    (void)state;
    reset_state();

    window.current_time_ms = 777;
    window.stale_threshold_ms = 333;
    runtime_player_sync_abi_update_staleness(window);

    BANANA_TEST_ASSERT_INT_EQ((int)g_last_update_now_ms,
                              777,
                              "update_staleness should forward current time");
    BANANA_TEST_ASSERT_INT_EQ((int)g_last_update_threshold_ms,
                              333,
                              "update_staleness should forward stale threshold");
}

static void test_deactivate_and_count_forward_registry_calls(void **state)
{
    (void)state;
    reset_state();

    g_count_active_result = 42;
    runtime_player_sync_abi_deactivate_stale();

    BANANA_TEST_ASSERT_INT_EQ(g_deactivate_calls,
                              1,
                              "deactivate_stale should call registry deactivate");
    BANANA_TEST_ASSERT_INT_EQ(runtime_player_sync_abi_count_active(),
                              42,
                              "count_active should return registry active count");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_mark_seen_forwards_guid_and_time),
    BANANA_TEST_CASE(test_update_staleness_forwards_window_values),
    BANANA_TEST_CASE(test_deactivate_and_count_forward_registry_calls)
)
