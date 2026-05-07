# Tasks — 072 Native ABI Versioning Contract

- [x] T001 — Add `BANANA_ABI_MAJOR` / `BANANA_ABI_MINOR` constants to `banana_wrapper.h`
- [x] T002 — Implement `banana_native_version(int* out_major, int* out_minor)` in `banana_wrapper.c`
- [x] T003 — Export `banana_native_version` via `BANANA_API` macro in wrapper header
- [x] T004 — Document ABI versioning policy and deprecation grace period in wrapper header comments
- [x] T005 — Add ABI version assertion in C# interop at startup
- [x] T006 — Add CI lane / script to diff exported symbols against prior tag
