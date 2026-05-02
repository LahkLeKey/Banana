import { describe, expect, test } from "bun:test";

import { PII_FIELDS, redactFields, redactForLevel } from "./pii";

describe("pii redaction", () => {
  test("redactForLevel handles high and medium levels", () => {
    expect(redactForLevel("192.168.0.1", "high")).toBe("[REDACTED]");
    expect(redactForLevel("abcdef", "medium")).toBe("abcd****");
    expect(redactForLevel("hello", "low")).toBe("hello");
  });

  test("redactFields redacts matching fields based on annotations", () => {
    const input = {
      actor: "operator-1234",
      ip_address: "192.168.0.1",
      text: "ripe banana",
      content: "chat payload",
      untouched: "keep me",
    };

    const output = redactFields(input, PII_FIELDS);

    expect(output.actor).toBe("oper****");
    expect(output.ip_address).toBe("[REDACTED]");
    expect(output.text).toBe("ripe banana");
    expect(output.content).toBe("chat****");
    expect(output.untouched).toBe("keep me");
  });
});
