#ifndef BANANA_ENGINE_RUNTIME_CONTROLLER_KIND_DOMAIN_H
#define BANANA_ENGINE_RUNTIME_CONTROLLER_KIND_DOMAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum RuntimeControllerKind
    {
        RUNTIME_CONTROLLER_KIND_UNKNOWN = 0,
        RUNTIME_CONTROLLER_KIND_HUMAN,
        RUNTIME_CONTROLLER_KIND_AI,
        RUNTIME_CONTROLLER_KIND_MERCHANT,
        RUNTIME_CONTROLLER_KIND_COMBAT,
        RUNTIME_CONTROLLER_KIND_WILDLIFE,
        RUNTIME_CONTROLLER_KIND_RESOURCE,
        RUNTIME_CONTROLLER_KIND_QUEST,
        RUNTIME_CONTROLLER_KIND_CAMP,
        RUNTIME_CONTROLLER_KIND_PBJ_PICKUP,
        RUNTIME_CONTROLLER_KIND_COUNT
    } RuntimeControllerKind;

    int runtime_controller_kind_parse(const char *value,
                                      RuntimeControllerKind *out_kind);

    RuntimeControllerKind runtime_controller_kind_parse_or_unknown(const char *value);

    const char *runtime_controller_kind_name(RuntimeControllerKind kind);

#ifdef __cplusplus
}
#endif

#endif
