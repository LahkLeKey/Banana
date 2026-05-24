#include "controller_kind_domain.h"

#include <stddef.h>
#include <string.h>

typedef struct RuntimeControllerKindEntry
{
    RuntimeControllerKind kind;
    const char *name;
} RuntimeControllerKindEntry;

static const RuntimeControllerKindEntry k_controller_kind_entries[] = {
    {RUNTIME_CONTROLLER_KIND_HUMAN, "human"},
    {RUNTIME_CONTROLLER_KIND_AI, "ai"},
    {RUNTIME_CONTROLLER_KIND_MERCHANT, "merchant"},
    {RUNTIME_CONTROLLER_KIND_COMBAT, "combat"},
    {RUNTIME_CONTROLLER_KIND_WILDLIFE, "wildlife"},
    {RUNTIME_CONTROLLER_KIND_RESOURCE, "resource"},
    {RUNTIME_CONTROLLER_KIND_QUEST, "quest"},
    {RUNTIME_CONTROLLER_KIND_CAMP, "camp"},
    {RUNTIME_CONTROLLER_KIND_PBJ_PICKUP, "pbj_pickup"},
};

int runtime_controller_kind_parse(const char *value,
                                  RuntimeControllerKind *out_kind)
{
    int i = 0;

    if (!value || !value[0] || !out_kind)
        return -1;

    for (i = 0; i < (int)(sizeof(k_controller_kind_entries) / sizeof(k_controller_kind_entries[0])); i++)
    {
        if (strcmp(value, k_controller_kind_entries[i].name) == 0)
        {
            *out_kind = k_controller_kind_entries[i].kind;
            return 0;
        }
    }

    return -1;
}

RuntimeControllerKind runtime_controller_kind_parse_or_unknown(const char *value)
{
    RuntimeControllerKind kind = RUNTIME_CONTROLLER_KIND_UNKNOWN;
    if (runtime_controller_kind_parse(value, &kind) != 0)
        return RUNTIME_CONTROLLER_KIND_UNKNOWN;

    return kind;
}

const char *runtime_controller_kind_name(RuntimeControllerKind kind)
{
    int i = 0;

    for (i = 0; i < (int)(sizeof(k_controller_kind_entries) / sizeof(k_controller_kind_entries[0])); i++)
    {
        if (k_controller_kind_entries[i].kind == kind)
            return k_controller_kind_entries[i].name;
    }

    return "";
}
