# Tests V3 Scaffold

- Native runtime validation now lives in `tests/native/feedback/native_feedback_loop_factory.c` and supports playloop scripts in `tests/native/feedback/scripts/*.dx12play` via `scripts/run-native-feedback-loop.sh --script ...`.
- Scenario aliases (`flank`, `pressure`, `truce`, `rally`) map to the core warfront/negotiate/comeback gameplay loops for extended test coverage.
