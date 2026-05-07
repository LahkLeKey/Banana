#ifndef BANANA_ENGINE_SIGNALS_H
#define BANANA_ENGINE_SIGNALS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Forward declaration */
struct World;

#define BANANA_SIGNAL_NAME_LEN  32
#define BANANA_SIGNAL_DATA_LEN  64
#define BANANA_MAX_SIGNALS      256

typedef struct Signal {
    uint32_t target_id;                   /* entity/controller to deliver to */
    char     name[BANANA_SIGNAL_NAME_LEN];
    char     data[BANANA_SIGNAL_DATA_LEN]; /* arbitrary payload (JSON or raw) */
} Signal;

typedef struct SignalQueue {
    Signal   items[BANANA_MAX_SIGNALS];
    int      head;
    int      tail;
    int      count;
} SignalQueue;

SignalQueue *signals_get_queue(void);

/* Enqueue a signal for delivery at next world_tick. */
void signals_send(uint32_t target_id, const char *name, const char *data);

/* Deliver all queued signals to their target controllers, then clear queue. */
void signals_flush(struct World *world);

#ifdef __cplusplus
}
#endif

#endif /* BANANA_ENGINE_SIGNALS_H */
