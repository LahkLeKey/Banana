export type HumanGrade = "ungraded"|"pass"|"fail";

export type QuizAttempt = {
    id: string; answeredAtUtc : string; autoScore : number; humanGrade : HumanGrade;
};

export type QuizTrend =
    |"insufficient-data"|"regressed-below-floor"|"growing"|"stable"|"meets-benchmark";

export type QuizBenchmarkSummary = {
    trend: QuizTrend; gatePass : boolean; currentAverage : number; previousAverage : number | null;
    benchmarkTarget : number;
    regressionFloor : number;
};

export type QuizBenchmarkOptions = {
    benchmarkTarget?: number;
    regressionFloor?: number;
    windowSize?: number;
};

function clamp01(value: number): number
{
    return Math.max(0, Math.min(1, value));
}

function effectiveScore(attempt: QuizAttempt): number
{
    if (attempt.humanGrade === "pass")
    {
        return 1;
    }
    if (attempt.humanGrade === "fail")
    {
        return 0;
    }
    return clamp01(attempt.autoScore);
}

function average(values: number[]): number
{
    if (values.length === 0)
    {
        return 0;
    }
    const sum = values.reduce((acc, value) => acc + value, 0);
    return Number((sum / values.length).toFixed(4));
}

export function analyzeQuizBenchmark(attempts: QuizAttempt[],
                                     options: QuizBenchmarkOptions = {}): QuizBenchmarkSummary
{
    const benchmarkTarget = options.benchmarkTarget ?? 0.8;
    const regressionFloor = options.regressionFloor ?? 0.65;
    const windowSize = options.windowSize ?? 5;

    if (attempts.length === 0)
    {
        return {
            trend : "insufficient-data",
            gatePass : false,
            currentAverage : 0,
            previousAverage : null,
            benchmarkTarget,
            regressionFloor,
        };
    }

    const orderedAttempts =
        [...attempts ].sort((a, b) => a.answeredAtUtc.localeCompare(b.answeredAtUtc));
    const scores = orderedAttempts.map(effectiveScore);

    const currentWindow = scores.slice(-windowSize);
    const previousWindow = scores.slice(-windowSize * 2, -windowSize);

    const currentAverage = average(currentWindow);
    const previousAverage = previousWindow.length > 0 ? average(previousWindow) : null;

    let trend: QuizTrend = "stable";
    if (currentAverage < regressionFloor)
    {
        trend = "regressed-below-floor";
    }
    else if (currentAverage >= benchmarkTarget &&
             (previousAverage == null || currentAverage >= previousAverage))
    {
        trend = "meets-benchmark";
    }
    else if (previousAverage != null && currentAverage > previousAverage)
    {
        trend = "growing";
    }
    else if (attempts.length < windowSize)
    {
        trend = "insufficient-data";
    }

    return {
        trend,
        gatePass : currentAverage >= regressionFloor,
        currentAverage,
        previousAverage,
        benchmarkTarget,
        regressionFloor,
    };
}
