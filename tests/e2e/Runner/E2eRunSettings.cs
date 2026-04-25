namespace Banana.E2eTests.Runner;

public sealed record E2eRunSettings(
    string ComposeProjectName,
    string BootstrapScript,
    string TeardownScript,
    Uri ApiBaseUri,
    bool RequireScriptFiles)
{
    public static E2eRunSettings FromEnvironment()
    {
        var composeProjectName = ReadString("BANANA_E2E_COMPOSE_PROJECT", "banana-container");
        var bootstrapScript = ReadString("BANANA_E2E_BOOTSTRAP_SCRIPT", "scripts/compose-e2e-bootstrap.sh");
        var teardownScript = ReadString("BANANA_E2E_TEARDOWN_SCRIPT", "scripts/compose-e2e-teardown.sh");
        var apiBaseUrl = ReadString("BANANA_E2E_API_BASE_URL", "http://localhost:8080");
        var requireScriptFiles = ReadBool("BANANA_E2E_REQUIRE_SCRIPT_FILES", defaultValue: false);

        return new E2eRunSettings(
            ComposeProjectName: composeProjectName,
            BootstrapScript: bootstrapScript,
            TeardownScript: teardownScript,
            ApiBaseUri: new Uri(apiBaseUrl, UriKind.Absolute),
            RequireScriptFiles: requireScriptFiles);
    }

    private static string ReadString(string variableName, string defaultValue)
    {
        var value = Environment.GetEnvironmentVariable(variableName);
        return string.IsNullOrWhiteSpace(value) ? defaultValue : value.Trim();
    }

    private static bool ReadBool(string variableName, bool defaultValue)
    {
        var value = Environment.GetEnvironmentVariable(variableName);
        if (string.IsNullOrWhiteSpace(value))
        {
            return defaultValue;
        }

        return value.Trim().ToLowerInvariant() switch
        {
            "1" or "true" or "yes" or "on" => true,
            "0" or "false" or "no" or "off" => false,
            _ => defaultValue
        };
    }
}
