# Confidence and Uncertainty Policy

## Confidence Scale

- 0: insufficient evidence
- 1: weak evidence
- 2: partial evidence
- 3: reasonable evidence
- 4: strong evidence
- 5: high-confidence evidence

## Evidence Inputs

Confidence should be based on:

- existing repo code and tests
- prior feature outcomes (004/005)
- stability of related contracts
- availability of clear validation lanes

## Uncertainty Handling

For each candidate, record:

- known unknowns
- assumptions that could shift ranking
- mitigation approach for highest-impact uncertainty

## Promotion Rule

A top-3 candidate should not be packetized without:

- explicit uncertainty notes
- at least one mitigation statement
- a confidence score of 2 or greater
