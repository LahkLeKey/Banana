function formatJson(value) {
  return JSON.stringify(value, null, 2);
}

function setPill(element, label, tone) {
  element.textContent = label;
  element.className = `pill ${tone}`;
}

function readFormState() {
  return {
    purchases: Number(document.getElementById("purchases").value),
    multiplier: Number(document.getElementById("multiplier").value),
    apiUrl: document.getElementById("api-url").value,
    includeDatabase: document.getElementById("include-database").checked,
  };
}

function bindAction(buttonId, runner, statusElement, outputElement, loadingLabel) {
  const button = document.getElementById(buttonId);
  button.addEventListener("click", async () => {
    setPill(statusElement, loadingLabel, "working");
    outputElement.textContent = "Running...";

    try {
      const result = await runner(readFormState());
      setPill(statusElement, "Ready", "success");
      outputElement.textContent = formatJson(result);
    } catch (error) {
      setPill(statusElement, "Failed", "danger");
      outputElement.textContent = formatJson({
        message: error.message,
        status: error.status,
        statusCode: error.statusCode,
        payload: error.payload,
      });
    }
  });
}

async function bootstrap() {
  const config = await window.bananaQA.getConfig();
  document.getElementById("api-url").value = config.apiUrl;
  document.getElementById("api-url-label").textContent = config.apiUrl;

  const nativeStatus = document.getElementById("native-status");
  const apiStatus = document.getElementById("api-status");
  const comparisonStatus = document.getElementById("comparison-status");

  bindAction(
    "native-button",
    (payload) => window.bananaQA.runNative(payload),
    nativeStatus,
    document.getElementById("native-output"),
    "Running",
  );

  bindAction(
    "api-button",
    (payload) => window.bananaQA.runApi(payload),
    apiStatus,
    document.getElementById("api-output"),
    "Calling",
  );

  bindAction(
    "compare-button",
    async (payload) => {
      const result = await window.bananaQA.runCompare(payload);
      setPill(comparisonStatus, result.matches ? "Match" : "Mismatch", result.matches ? "success" : "danger");
      return result;
    },
    comparisonStatus,
    document.getElementById("comparison-output"),
    "Comparing",
  );

  document.getElementById("health-button").addEventListener("click", async () => {
    setPill(apiStatus, "Checking", "working");
    try {
      const result = await window.bananaQA.checkHealth(readFormState());
      setPill(apiStatus, "Healthy", "success");
      document.getElementById("api-output").textContent = formatJson(result);
    } catch (error) {
      setPill(apiStatus, "Unavailable", "danger");
      document.getElementById("api-output").textContent = formatJson({
        message: error.message,
        statusCode: error.statusCode,
        payload: error.payload,
      });
    }
  });
}

bootstrap().catch((error) => {
  document.getElementById("comparison-output").textContent = formatJson({
    message: error.message,
  });
});