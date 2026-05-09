using Xunit;

namespace Banana.E2eTests.Contracts;

public sealed class JupyterLiteRuntimeContractTests
{
    [Fact]
    public void StarterNotebookPathExistsAndIsDiscoverable()
    {
        var notebookPath = Path.Combine(FindRepoRoot(), "src", "typescript", "react", "public", "notebooks", "banana-ds-starter.ipynb");

        Assert.True(File.Exists(notebookPath), "Starter notebook is missing from public/notebooks.");

        var content = File.ReadAllText(notebookPath);
        Assert.Contains("\"cells\"", content, StringComparison.Ordinal);
    }

    [Fact]
    public void DataSciencePageContainsJupyterLiteReadinessAndRetryFlowContracts()
    {
        var source = ReadDataScienceLibSource();

        Assert.Contains("PYODIDE_INDEX", source, StringComparison.Ordinal);
        Assert.Contains("PYODIDE_SCRIPT", source, StringComparison.Ordinal);
    }

    [Fact]
    public void DataSciencePageContainsBaselineCodeCellExecutionContracts()
    {
        var source = ReadDataScienceLibSource();

        Assert.Contains("PYODIDE_SCRIPT", source, StringComparison.Ordinal);
        Assert.Contains("runPythonAsync", source, StringComparison.Ordinal);
        Assert.Contains("starterCells", source, StringComparison.Ordinal);
        Assert.Contains("stdout", source, StringComparison.Ordinal);
        Assert.Contains("traceback", source, StringComparison.Ordinal);
    }

    private static string ReadDataScienceLibSource()
    {
        var root = FindRepoRoot();
        var lib = Path.Combine(root, "src", "typescript", "react", "src", "lib");
        return string.Join("\n",
            File.ReadAllText(Path.Combine(lib, "dsConstants.ts")),
            File.ReadAllText(Path.Combine(lib, "dsTypes.ts")),
            File.ReadAllText(Path.Combine(lib, "dsNotebook.ts")));
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
