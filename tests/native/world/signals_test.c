#include "world/signals.h"
#include "world/world.h"

#include "runtime/support/test_support.h"

#include <string.h>

static void test_signals_get_queue(void **state)
{
    (void)state;
    SignalQueue *q = signals_get_queue();
    BANANA_TEST_ASSERT_TRUE(q != NULL, "get_queue must return non-null");
    BANANA_TEST_ASSERT_INT_EQ(q->count, 0, "fresh queue must be empty");
}

static void test_signals_send_and_flush(void **state)
{
    (void)state;
    World world = {0};
    Entity entity = { .id = 5, .active = 1, .controller_id = 7 };
    world.entities[0] = &entity;
    world.entity_count = 1;

    SignalQueue *q = signals_get_queue();
    q->count = 0;
    q->head = 0;

    /* send a signal */
    signals_send(5, "test-event", "payload");
    BANANA_TEST_ASSERT_INT_EQ(q->count, 1, "queue must contain one item after send");

    /* send with null data */
    signals_send(5, "no-payload-event", NULL);
    BANANA_TEST_ASSERT_INT_EQ(q->count, 2, "queue must contain two items");

    /* flush delivers to world entities */
    signals_flush(&world);
    BANANA_TEST_ASSERT_INT_EQ(q->count, 0, "queue must be empty after flush");

    /* flush with empty queue must not crash */
    signals_flush(&world);
    signals_flush(NULL);
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_signals_get_queue),
    BANANA_TEST_CASE(test_signals_send_and_flush)
)
