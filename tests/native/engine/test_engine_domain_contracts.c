/* Engine Domain Contract Test Orchestrator (DDD/SOLID aligned) */

#include <stdio.h>

int run_engine_physics_suite(void);
int run_engine_ai_fsm_suite(void);
int run_engine_ai_navigation_suite(void);
int run_engine_ai_controller_suite(void);
int run_engine_render_suite(void);
int run_engine_world_suite(void);

int main(void)
{
    int failures = 0;

    printf("\n=== Banana Engine Domain Contract Suites ===\n\n");

    failures += run_engine_physics_suite();
    failures += run_engine_ai_fsm_suite();
    failures += run_engine_ai_navigation_suite();
    failures += run_engine_ai_controller_suite();
    failures += run_engine_render_suite();
    failures += run_engine_world_suite();

    printf("\n============================================\n");
    printf("Total suite failures: %d\n", failures);
    return failures == 0 ? 0 : 1;
}
