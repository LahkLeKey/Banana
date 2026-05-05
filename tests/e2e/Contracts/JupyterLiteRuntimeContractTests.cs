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
        var source = ReadDataSciencePageSource();

        Assert.Contains("PYODIDE_INDEX", source, StringComparison.Ordinal);
        Assert.Contains("PYODIDE_SCRIPT", source, StringComparison.Ordinal);
        Assert.Contains("Loading Python runtime (Pyodide)", source, StringComparison.Ordinal);
        Assert.Contains("window.loadPyodide", source, StringComparison.Ordinal);
        Assert.Contains("Python runtime ready.", source, StringComparison.Ordinal);
    }

    [Fact]
    public void DataSciencePageContainsBaselineCodeCellExecutionContracts()
    {
        var source = ReadDataSciencePageSource();

        Assert.Contains("PYODIDE_SCRIPT", source, StringComparison.Ordinal);
        Assert.Contains("runPythonAsync", source, StringComparison.Ordinal);
        Assert.Contains("starterCells", source, StringComparison.Ordinal);
        Assert.Contains("stdout", source, StringComparison.Ordinal);
        Assert.Contains("traceback", source, StringComparison.Ordinal);
    }

    private static string ReadDataSciencePageSource()
    {
        return File.ReadAllText(Path.Combine(FindRepoRoot(), "src", "typescript", "react", "src", "pages", "DataSciencePage.tsx"));
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