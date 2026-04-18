# Release Readiness Checklist

## Code And Contract

- The changed area matches the intended domain scope
- Cross-layer contracts were preserved or explicitly documented
- Native interop changes were paired with managed validation when needed

## Validation

- Relevant build task or script was run
- Relevant focused tests were run
- Broader integration, compose, or coverage checks were run when risk justified them

## Documentation And Ops

- User-facing or developer-facing workflow changes are documented near the changed entry point
- Prompt, skill, or instruction references remain accurate after the change
- Runtime env vars and health check expectations still match the compose and workflow configuration
