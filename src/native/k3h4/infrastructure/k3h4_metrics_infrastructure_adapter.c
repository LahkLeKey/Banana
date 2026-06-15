#include "k3h4_metrics_infrastructure_adapter.h"

#include "../k3h4_native_orchestrator.h"

BananaNativeK3h4DomainPort banana_native_k3h4_infrastructure_create_domain_port(void)
{
    BananaNativeK3h4DomainPort port;
    /* Bind each domain port slot to the direct ABI facade so the layered path
     * preserves exactly the same numerical behavior as the non-layered path.
     */
    port.build_learning = banana_native_k3h4_build_learning;
    port.build_reward = banana_native_k3h4_build_reward;
    port.build_link = banana_native_k3h4_build_link;
    port.build_vector = banana_native_k3h4_build_vector;
    port.build_k3h4 = banana_native_k3h4_build_k3h4;
    return port;
}
