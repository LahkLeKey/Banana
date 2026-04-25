namespace Banana.E2eTests.Contracts;

public sealed record E2eScenario(
    string Id,
    string Title,
    string EndpointFlow,
    string[] Tags);

public sealed record E2eRunnerContract(
    string Name,
    IReadOnlyList<E2eScenario> Scenarios);

public static class E2eRunnerContracts
{
    public static E2eRunnerContract CriticalJourneys()
    {
        return new E2eRunnerContract(
            Name: "banana-critical-journeys",
            Scenarios:
            [
                new E2eScenario(
                    Id: "CJ001",
                    Title: "Banana runtime console core success",
                    EndpointFlow: "/health -> /banana -> /batches/create -> /batches/{id}/status -> /ripeness/predict",
                    Tags: ["runtime", "core-success"]),
                new E2eScenario(
                    Id: "CJ002",
                    Title: "Not-banana polymorphic classification success",
                    EndpointFlow: "/not-banana/junk",
                    Tags: ["runtime", "classification"]),
                new E2eScenario(
                    Id: "CJ003",
                    Title: "Validation failure branch",
                    EndpointFlow: "/banana (invalid query)",
                    Tags: ["runtime", "validation"]),
                new E2eScenario(
                    Id: "CJ004",
                    Title: "Native failure mapping branch",
                    EndpointFlow: "/banana (native failure)",
                    Tags: ["runtime", "error-mapping"]),
                new E2eScenario(
                    Id: "CJ005",
                    Title: "Database failure mapping branch",
                    EndpointFlow: "/banana (database failure)",
                    Tags: ["runtime", "error-mapping"]),
                new E2eScenario(
                    Id: "CJ006",
                    Title: "Failure then recovery journey",
                    EndpointFlow: "/banana (failure) -> /banana (recovery)",
                    Tags: ["runtime", "recovery"]),
                new E2eScenario(
                    Id: "CJ007",
                    Title: "Deterministic reset between scenarios",
                    EndpointFlow: "/batches/create -> /batches/{id}/status across fresh hosts",
                    Tags: ["runtime", "deterministic-reset"])
            ]);
    }
}
