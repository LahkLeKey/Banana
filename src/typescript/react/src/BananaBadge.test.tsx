import { describe, expect, test } from "bun:test";
import { BananaBadge, type EnsembleVerdict } from "@banana/ui";
import { renderToStaticMarkup } from "react-dom/server";

function render(node: ReturnType<typeof BananaBadge>): string {
  return renderToStaticMarkup(node);
}

describe("BananaBadge ensemble variant", () => {
  test("renders count variant unchanged when variant is omitted", () => {
    const html = render(<BananaBadge count={3} />);
    expect(html).toContain("🍌 3");
    expect(html).not.toContain("ensemble");
  });

  test("clearly-banana anchor renders label + magnitude, no escalation pill", () => {
    const verdict: EnsembleVerdict = {
      label: "banana",
      score: 0.95,
      did_escalate: false,
      calibration_magnitude: 0.71,
      status: "ok",
    };
    const html = render(<BananaBadge variant="ensemble" verdict={verdict} />);
    expect(html).toContain("🍌 banana");
    expect(html).toContain("m=0.71");
    expect(html).not.toContain("escalated");
  });

  test("clearly-not-banana anchor renders label + magnitude", () => {
    const verdict: EnsembleVerdict = {
      label: "not_banana",
      score: 0.95,
      did_escalate: false,
      calibration_magnitude: 0.12,
      status: "ok",
    };
    const html = render(<BananaBadge variant="ensemble" verdict={verdict} />);
    expect(html).toContain("🍌 not banana");
    expect(html).toContain("m=0.12");
    expect(html).not.toContain("escalated");
  });

  test("ambiguous (escalated) anchor renders label + magnitude + escalation pill", () => {
    const verdict: EnsembleVerdict = {
      label: "not_banana",
      score: 0.56,
      did_escalate: true,
      calibration_magnitude: 0.5,
      status: "ok",
    };
    const html = render(<BananaBadge variant="ensemble" verdict={verdict} />);
    expect(html).toContain("🍌 not banana");
    expect(html).toContain("escalated");
  });

  test("US3: attention chip absent by default = byte-identical to absent flag", () => {
    const verdict: EnsembleVerdict = {
      label: "banana",
      score: 0.83,
      did_escalate: true,
      calibration_magnitude: 0.6,
      status: "ok",
    };
    const off = render(<BananaBadge variant="ensemble" verdict={verdict} />);
    const offExplicit = render(
      <BananaBadge variant="ensemble" verdict={verdict} showAttention={false} />
    );
    expect(off).toBe(offExplicit);
    expect(off).not.toContain("attention");
  });

  test("US3: attention chip present when opted in", () => {
    const verdict: EnsembleVerdict = {
      label: "banana",
      score: 0.95,
      did_escalate: false,
      calibration_magnitude: 0.7,
      status: "ok",
    };
    const html = render(<BananaBadge variant="ensemble" verdict={verdict} showAttention />);
    expect(html).toContain("banana-badge-ensemble-attention");
    // RB tag because did_escalate=false
    expect(html).toContain(">rb<");
  });

  test("escalated + attention = fb chip", () => {
    const verdict: EnsembleVerdict = {
      label: "banana",
      score: 0.83,
      did_escalate: true,
      calibration_magnitude: 0.7,
      status: "ok",
    };
    const html = render(<BananaBadge variant="ensemble" verdict={verdict} showAttention />);
    expect(html).toContain(">fb<");
  });

  test("degraded status surfaces via data-status", () => {
    const verdict: EnsembleVerdict = {
      label: "unknown",
      score: 0,
      did_escalate: true,
      calibration_magnitude: 0,
      status: "degraded",
    };
    const html = render(<BananaBadge variant="ensemble" verdict={verdict} />);
    expect(html).toContain('data-status="degraded"');
    expect(html).toContain("🍌 unknown");
  });
});
