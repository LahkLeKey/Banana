const { fetchBanana, fetchHealth } = require("./apiClient");
const { runNativeScenario } = require("./nativeBridge");

async function main() {
  const purchases = Number(process.env.BANANA_SMOKE_PURCHASES || 10);
  const multiplier = Number(process.env.BANANA_SMOKE_MULTIPLIER || 2);
  const apiUrl = process.env.BANANA_API_URL || "http://127.0.0.1:8080";
  const includeDatabase = process.env.BANANA_SMOKE_DB === "1";

  const nativeResult = runNativeScenario(purchases, multiplier, { includeDatabase });
  const health = await fetchHealth(apiUrl);
  const apiResult = await fetchBanana(apiUrl, purchases, multiplier);

  console.log(JSON.stringify({
    health,
    native: nativeResult,
    api: apiResult,
    matches: nativeResult.calculation === apiResult.payload.banana,
  }, null, 2));
}

main().catch((error) => {
  console.error("electron QA smoke failed", {
    message: error.message,
    status: error.status,
    statusCode: error.statusCode,
    payload: error.payload,
  });
  process.exitCode = 1;
});