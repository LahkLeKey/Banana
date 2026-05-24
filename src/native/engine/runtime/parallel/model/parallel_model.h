#ifndef BANANA_ENGINE_RUNTIME_PARALLEL_MODEL_H
#define BANANA_ENGINE_RUNTIME_PARALLEL_MODEL_H

#ifdef __cplusplus
extern "C"
{
#endif

    double runtime_parallel_amdahl_speedup(double serial_fraction, int worker_count);
    double runtime_parallel_amdahl_efficiency(double serial_fraction, int worker_count);
    double runtime_parallel_amdahl_serial_fraction_from_speedup(double speedup, int worker_count);

#ifdef __cplusplus
}
#endif

#endif
