using System.Text.Json;

using Xunit;

namespace Banana.E2eTests.Contracts;

public sealed class ApiParityEnforcementContractTests
{
    private static readonly string[] GateDecisions = new[] { "PASS", "FAIL" };

    [Fact]
    public void ParityContractEnumeratesRequiredMismatchTypes()
    {
        var repoRoot = FindRepoRoot();
        var contractPath = Path.Combine(
            repoRoot,
            ".specify",
            "specs",
            "047-api-parity-governance",
            "contracts",
            "api-parity-governance-contract.md");

        Assert.True(File.Exists(contractPath), $"Missing contract file: {contractPath}");
        var contractText = File.ReadAllText(contractPath);

        Assert.Contains("missing_route", contractText);
        Assert.Contains("status_mismatch", contractText);
        Assert.Contains("shape_mismatch", contractText);
        Assert.Contains("Gate Logic (Explicit Pass/Fail)", contractText);
    }

    [Fact]
    public void ParityGateArtifactContainsDecisionAndUnresolvedSummary()
    {
        var repoRoot = FindRepoRoot();
        var gatePath = Path.Combine(
            repoRoot,
            ".specify",
            "specs",
            "047-api-parity-governance",
            "artifacts",
            "parity-gate-result.json");

        Assert.True(File.Exists(gatePath), $"Missing gate artifact: {gatePath}");

        using var document = JsonDocument.Parse(File.ReadAllText(gatePath));
        var root = document.RootElement;

        Assert.True(root.TryGetProperty("decision", out var decision));
        Assert.Contains(decision.GetString(), GateDecisions);

        Assert.True(root.TryGetProperty("unresolved_summary", out var unresolved));
        Assert.True(unresolved.TryGetProperty("total_findings", out _));
        Assert.True(unresolved.TryGetProperty("missing_route", out _));
        Assert.True(unresolved.TryGetProperty("status_mismatch", out _));
        Assert.True(unresolved.TryGetProperty("shape_mismatch", out _));
    }

    private static string FindRepoRoot()
    {
        var dir = new DirectoryInfo(AppContext.BaseDirectory);
        while (dir != null)
        {
            if (File.Exists(Path.Combine(dir.FullName, "Banana.sln")))
            {
                return dir.FullName;
            }
            dir = dir.Parent;
        }

        throw new InvalidOperationException("Could not locate repository root from test base directory.");
    }
}
