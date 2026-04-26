using Banana.E2eTests.Contracts;
using Banana.E2eTests.Runner;

using Xunit;

namespace Banana.E2eTests;

public sealed class E2eRunnerContractTests
{
    [Fact]
    public void CriticalJourneysUsesStableDeterministicScenarioOrder()
    {
        var contract = E2eRunnerContracts.CriticalJourneys();

        var scenarioIds = contract.Scenarios.Select(static scenario => scenario.Id).ToArray();

        Assert.Equal("banana-critical-journeys", contract.Name);
        Assert.Equal(["CJ001", "CJ002", "CJ003", "CJ004", "CJ005", "CJ006", "CJ007"], scenarioIds);
    }

    [Fact]
    public void E2eRunSettingsUsesDefaultRuntimeContractsWhenEnvironmentIsUnset()
    {
        Environment.SetEnvironmentVariable("BANANA_E2E_COMPOSE_PROJECT", null);
        Environment.SetEnvironmentVariable("BANANA_E2E_BOOTSTRAP_SCRIPT", null);
        Environment.SetEnvironmentVariable("BANANA_E2E_TEARDOWN_SCRIPT", null);
        Environment.SetEnvironmentVariable("BANANA_E2E_API_BASE_URL", null);
        Environment.SetEnvironmentVariable("BANANA_E2E_REQUIRE_SCRIPT_FILES", null);

        var settings = E2eRunSettings.FromEnvironment();

        Assert.Equal("banana-container", settings.ComposeProjectName);
        Assert.Equal("scripts/compose-e2e-bootstrap.sh", settings.BootstrapScript);
        Assert.Equal("scripts/compose-e2e-teardown.sh", settings.TeardownScript);
        Assert.Equal(new Uri("http://localhost:8080"), settings.ApiBaseUri);
        Assert.False(settings.RequireScriptFiles);
    }

    [Fact]
    public void ValidatorRejectsDuplicateScenarioIdentifiers()
    {
        var contract = new E2eRunnerContract(
            Name: "duplicate-case",
            Scenarios:
            [
                new E2eScenario("CJ001", "first", "/banana", ["runtime"]),
                new E2eScenario("CJ001", "second", "/banana", ["runtime"])
            ]);

        var settings = E2eRunSettings.FromEnvironment() with { RequireScriptFiles = false };

        var error = Assert.Throws<InvalidOperationException>(
            () => E2eRunnerContractValidator.Validate(contract, settings, repositoryRoot: "."));

        Assert.Contains("Duplicate E2E scenario id detected", error.Message, StringComparison.Ordinal);
    }

    [Fact]
    public void ValidatorAcceptsScaffoldContractWhenScriptPresenceChecksAreDisabled()
    {
        var contract = E2eRunnerContracts.CriticalJourneys();
        var settings = E2eRunSettings.FromEnvironment() with { RequireScriptFiles = false };

        E2eRunnerContractValidator.Validate(contract, settings, repositoryRoot: ".");
    }

    [Fact]
    public void ValidatorAcceptsScaffoldContractWhenScriptPresenceChecksAreEnabled()
    {
        var contract = E2eRunnerContracts.CriticalJourneys();
        var settings = E2eRunSettings.FromEnvironment() with { RequireScriptFiles = true };
        var repositoryRoot = ResolveRepositoryRoot();

        E2eRunnerContractValidator.Validate(contract, settings, repositoryRoot);
    }

    private static string ResolveRepositoryRoot()
    {
        var cursor = new DirectoryInfo(AppContext.BaseDirectory);

        while (cursor is not null)
        {
            if (File.Exists(Path.Combine(cursor.FullName, "Directory.Build.props")))
            {
                return cursor.FullName;
            }

            cursor = cursor.Parent;
        }

        throw new InvalidOperationException("Unable to resolve repository root from test execution directory.");
    }
}
