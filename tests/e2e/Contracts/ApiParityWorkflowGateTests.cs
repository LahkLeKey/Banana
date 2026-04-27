using Xunit;

namespace Banana.E2eTests.Contracts;

public sealed class ApiParityWorkflowGateTests
{
    [Fact]
    public void ComposeCiWorkflowIncludesApiParityGovernanceLane()
    {
        var workflow = File.ReadAllText(Path.Combine(FindRepoRoot(), ".github", "workflows", "compose-ci.yml"));

        Assert.Contains("api-parity-governance:", workflow);
        Assert.Contains("BANANA_CI_LANE: api-parity", workflow);
        Assert.Contains("bash scripts/validate-api-parity-governance.sh --strict", workflow);
        Assert.Contains("name: api-parity-governance-artifacts", workflow);
    }

    [Fact]
    public void WorkflowOrchestrationScriptsExposeApiParityPreflightInvocation()
    {
        var repoRoot = FindRepoRoot();
        var sdlc = File.ReadAllText(Path.Combine(repoRoot, "scripts", "workflow-orchestrate-sdlc.sh"));
        var triaged = File.ReadAllText(Path.Combine(repoRoot, "scripts", "workflow-orchestrate-triaged-item-pr.sh"));

        Assert.Contains("BANANA_SDLC_RUN_API_PARITY_CHECK", sdlc);
        Assert.Contains("bash scripts/validate-api-parity-governance.sh", sdlc);

        Assert.Contains("BANANA_TRIAGE_RUN_API_PARITY_CHECK", triaged);
        Assert.Contains("bash scripts/validate-api-parity-governance.sh", triaged);
    }

    [Fact]
    public void ComposeCiLaneWriterCanEmbedParityGateMetadata()
    {
        var script = File.ReadAllText(Path.Combine(FindRepoRoot(), "scripts", "compose-ci-write-lane-result.sh"));

        Assert.Contains("--parity-gate-file", script);
        Assert.Contains("\"parity_gate\"", script);
        Assert.Contains("if [[ \"$lane\" == \"api-parity\"", script);
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
