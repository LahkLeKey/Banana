import {describe, expect, test} from "bun:test";

import {analyzeQuizBenchmark, type QuizAttempt} from "./quizBenchmark";

function attempt(id: number, score: number,
                 humanGrade: "ungraded"|"pass"|"fail" = "ungraded"): QuizAttempt
{
    return {
        id : `a-${id}`,
        answeredAtUtc : `2026-05-${String(id).padStart(2, "0")}T00:00:00Z`,
        autoScore : score,
        humanGrade,
    };
}

describe("quiz benchmark analysis", () => {
    test("flags regression below floor", () => {
        const summary = analyzeQuizBenchmark([
            attempt(1, 0.9),
            attempt(2, 0.8),
            attempt(3, 0.4),
            attempt(4, 0.45),
            attempt(5, 0.5),
        ]);

        expect(summary.trend).toBe("regressed-below-floor");
        expect(summary.gatePass).toBe(false);
    });

    test("detects growth when current window improves over previous", () => {
        const summary = analyzeQuizBenchmark([
            attempt(1, 0.6),
            attempt(2, 0.6),
            attempt(3, 0.7),
            attempt(4, 0.7),
            attempt(5, 0.72),
            attempt(6, 0.75),
            attempt(7, 0.78),
            attempt(8, 0.79),
            attempt(9, 0.8),
            attempt(10, 0.82),
        ]);

        expect(summary.trend).toBe("growing");
        expect(summary.gatePass).toBe(true);
    });

    test("honors human grading overrides", () => {
        const summary = analyzeQuizBenchmark([
            attempt(1, 0.2, "pass"),
            attempt(2, 0.9, "fail"),
            attempt(3, 0.8),
            attempt(4, 0.9),
            attempt(5, 0.9),
        ]);

        expect(summary.currentAverage).toBeGreaterThan(0.7);
    });
});
