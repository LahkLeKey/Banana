#ifndef BANANA_ENGINE_SERIALIZE_H
#define BANANA_ENGINE_SERIALIZE_H

#ifdef __cplusplus
extern "C"
{
#endif

    const char *engine_tick_serialize(float dt);
    const char *engine_serialize_world(void);
    void engine_serialize_reset(void);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_SERIALIZE_H */
