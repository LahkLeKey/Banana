#ifndef BANANA_NATIVE_K3H4_INFRA_ADAPTER_H
#define BANANA_NATIVE_K3H4_INFRA_ADAPTER_H

#include "../domain/k3h4_metrics_domain_port.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /* Creates the concrete domain port backed by the direct native k3h4 facade. */
    BananaNativeK3h4DomainPort banana_native_k3h4_infrastructure_create_domain_port(void);

#ifdef __cplusplus
}
#endif

#endif
