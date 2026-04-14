const http = require("http");
const https = require("https");

function requestJson(url) {
  return new Promise((resolve, reject) => {
    const target = new URL(url);
    const client = target.protocol === "https:" ? https : http;
    const request = client.get(target, (response) => {
      let body = "";

      response.setEncoding("utf8");
      response.on("data", (chunk) => {
        body += chunk;
      });
      response.on("end", () => {
        const text = body.trim();
        const payload = text ? JSON.parse(text) : null;
        if (response.statusCode >= 400) {
          const error = new Error(`request failed with status ${response.statusCode}`);
          error.statusCode = response.statusCode;
          error.payload = payload;
          reject(error);
          return;
        }

        resolve({
          statusCode: response.statusCode,
          payload,
        });
      });
    });

    request.on("error", reject);
  });
}

async function fetchHealth(baseUrl) {
  const { payload, statusCode } = await requestJson(new URL("/health", baseUrl).toString());
  return {
    statusCode,
    payload,
  };
}

async function fetchBanana(baseUrl, purchases, multiplier) {
  const endpoint = new URL("/banana", baseUrl);
  endpoint.searchParams.set("purchases", String(purchases));
  endpoint.searchParams.set("multiplier", String(multiplier));

  const { payload, statusCode } = await requestJson(endpoint.toString());
  return {
    statusCode,
    payload,
  };
}

module.exports = {
  fetchBanana,
  fetchHealth,
};