using Banana.E2eTests.Contracts;

namespace Banana.E2eTests.Runner;

public static class E2eRunnerContractValidator
{
    public static void Validate(E2eRunnerContract contract, E2eRunSettings settings, string repositoryRoot)
    {
        ArgumentNullException.ThrowIfNull(contract);
        ArgumentNullException.ThrowIfNull(settings);
        ArgumentException.ThrowIfNullOrWhiteSpace(repositoryRoot);

        if (contract.Scenarios.Count == 0)
        {
            throw new InvalidOperationException("E2E runner contract must declare at least one scenario.");
        }

        var duplicateId = contract.Scenarios
            .GroupBy(static scenario => scenario.Id, StringComparer.Ordinal)
            .FirstOrDefault(static group => group.Count() > 1)?.Key;

        if (!string.IsNullOrWhiteSpace(duplicateId))
        {
            throw new InvalidOperationException($"Duplicate E2E scenario id detected: {duplicateId}.");
        }

        foreach (var scenario in contract.Scenarios)
        {
            if (string.IsNullOrWhiteSpace(scenario.Id) || !scenario.Id.StartsWith("CJ", StringComparison.Ordinal))
            {
                throw new InvalidOperationException("Each scenario id must be a non-empty CJ-prefixed identifier.");
            }

            if (string.IsNullOrWhiteSpace(scenario.EndpointFlow))
            {
                throw new InvalidOperationException($"Scenario {scenario.Id} must declare endpoint flow details.");
            }
        }

        if (!settings.RequireScriptFiles)
        {
            return;
        }

        var bootstrapPath = ToAbsolutePath(repositoryRoot, settings.BootstrapScript);
        var teardownPath = ToAbsolutePath(repositoryRoot, settings.TeardownScript);

        if (!File.Exists(bootstrapPath))
        {
            throw new FileNotFoundException("Configured E2E bootstrap script was not found.", bootstrapPath);
        }

        if (!File.Exists(teardownPath))
        {
            throw new FileNotFoundException("Configured E2E teardown script was not found.", teardownPath);
        }
    }

    private static string ToAbsolutePath(string repositoryRoot, string path)
    {
        var normalised = path.Replace('/', Path.DirectorySeparatorChar).Replace('\\', Path.DirectorySeparatorChar);
        return Path.GetFullPath(Path.Combine(repositoryRoot, normalised));
    }
}
