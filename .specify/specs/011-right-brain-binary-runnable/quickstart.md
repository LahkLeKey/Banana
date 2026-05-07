# Quickstart — 011 Right Brain Binary

```
cmake --preset default
cmake --build --preset default
ctest --preset default --output-on-failure
```

After this slice: 6/6 native ctest targets must pass, including
`banana_test_ml_binary` (US1 positive anchor, US2 negative samples,
US3 monotone threshold sweep, US4 JSON field-presence + null guards).

To exercise the new threshold knob from C:

```c
#include "banana_wrapper.h"
char* json = NULL;
int rc = banana_classify_banana_binary_with_threshold(
    "{\"text\":\"yellow ripe banana\"}", 0.65, &json);
/* ... use json ... */
banana_free(json);
```
