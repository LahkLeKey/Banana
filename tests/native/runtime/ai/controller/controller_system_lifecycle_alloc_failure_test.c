#include "ai/controller_system.h"

#include "runtime/support/test_support.h"

#include <stddef.h>

void *banana_test_calloc(size_t count, size_t size)
{
    (void)count;
    (void)size;
    return NULL;
}

ControllerInstance *controller_create(const char *type, float x, float y, float z)
{
    (void)type;
    (void)x;
    (void)y;
    (void)z;
    return NULL;
}

void controller_destroy(ControllerInstance *instance)
{
    (void)instance;
}

static void test_controller_system_create_returns_null_on_allocator_failure(void **state)
{
    (void)state;

    BANANA_TEST_ASSERT_TRUE(controller_system_create() == NULL,
                            "controller_system_create must return NULL when allocation fails");
}

BANANA_TEST_MAIN(
    BANANA_TEST_CASE(test_controller_system_create_returns_null_on_allocator_failure)
)
