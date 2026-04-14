const { runNativeScenario } = require("./nativeBridge");

function fail(error) {
  process.stdout.write(JSON.stringify({
    ok: false,
    error: {
      message: error.message,
      status: error.status,
    },
  }));
  process.exitCode = 1;
}

try {
  const request = JSON.parse(process.argv[2] || "{}");
  if (request.action !== "scenario") {
    throw new Error("unsupported native action");
  }

  const result = runNativeScenario(request.purchases, request.multiplier, {
    includeDatabase: Boolean(request.includeDatabase),
  });

  process.stdout.write(JSON.stringify({ ok: true, result }));
} catch (error) {
  fail(error);
}