# Tasks: Jupyter Plotly React Embedding Spike (187)

**Input**: Spec at `.specify/specs/187-jupyter-plotly-react-embedding-spike/spec.md`

## Phase 1: Discovery

- [x] T001 Inventory Plotly output forms used by Jupyter and notebook-style workflows.
- [x] T002 Define the candidate Banana Plotly figure contract, including layout, data, and metadata needs.

## Phase 2: Recommendation

- [x] T003 Document the React embedding lifecycle, including load, update, resize, and cleanup behavior.
- [x] T004 Evaluate bundle, performance, and theming implications for Plotly as Banana's primary interactive chart stack.

## Phase 3: Validation

- [x] T005 Capture adoption rules for when notebook outputs should prefer Plotly over simpler rendering paths.
- [x] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/187-jupyter-plotly-react-embedding-spike` and confirm `OK`.