#!/usr/bin/env bun

import {existsSync, mkdirSync, readFileSync, writeFileSync} from "node:fs";
import {join, resolve} from "node:path";

type Args = {
    currentReportPath: string;
    previousReportPath?: string; outDir : string; benchmarkTarget : number;
    regressionFloor : number;
};

type BenchmarkReport = {
    generated_at_utc?: string;
    score?: number;
    benchmark_target?: number;
    regression_floor?: number;
};

function parseArgs(argv: string[]): Args
{
    const cwd = process.cwd();
    const args: Args = {
        currentReportPath :
            resolve(cwd, ".artifacts/grade-school-gate/quiz-benchmark-gate-report.json"),
        outDir : resolve(cwd, ".artifacts/grade-school-gate"),
        benchmarkTarget : 0.8,
        regressionFloor : 0.65,
    };

    for (let i = 0; i < argv.length; i += 1)
    {
        const token = argv[i];
        switch (token)
        {
        case "--current-report":
            args.currentReportPath = resolve(cwd, argv[++i]);
            break;
        case "--previous-report":
            args.previousReportPath = resolve(cwd, argv[++i]);
            break;
        case "--out-dir":
            args.outDir = resolve(cwd, argv[++i]);
            break;
        case "--benchmark-target":
            args.benchmarkTarget = Number(argv[++i]);
            break;
        case "--regression-floor":
            args.regressionFloor = Number(argv[++i]);
            break;
        default:
            throw new Error(`Unknown argument: ${token}`);
        }
    }

    if (!Number.isFinite(args.benchmarkTarget) || args.benchmarkTarget < 0 ||
        args.benchmarkTarget > 1)
    {
        throw new Error("--benchmark-target must be between 0 and 1");
    }

    if (!Number.isFinite(args.regressionFloor) || args.regressionFloor < 0 ||
        args.regressionFloor > 1)
    {
        throw new Error("--regression-floor must be between 0 and 1");
    }

    return args;
}

function readReport(path: string): BenchmarkReport
{
    const raw = readFileSync(path, "utf-8");
    return JSON.parse(raw) as BenchmarkReport;
}

function toMarkdown(report: {
    generatedAtUtc: string; currentScore : number; previousScore : number | null;
    benchmarkTarget : number;
    regressionFloor : number;
    trendStatus : string;
    gatePass : boolean;
    reason : string;
}): string
{
    const lines: string[] = [];
    lines.push("# Quiz Benchmark Trend Gate Report");
    lines.push("");
    lines.push(`- generated_at_utc: ${report.generatedAtUtc}`);
    lines.push(`- current_score: ${report.currentScore.toFixed(4)}`);
    lines.push(`- previous_score: ${
        report.previousScore == null ? "n/a" : report.previousScore.toFixed(4)}`);
    lines.push(`- benchmark_target: ${report.benchmarkTarget.toFixed(4)}`);
    lines.push(`- regression_floor: ${report.regressionFloor.toFixed(4)}`);
    lines.push(`- trend_status: ${report.trendStatus}`);
    lines.push(`- gate_pass: ${report.gatePass}`);
    lines.push(`- reason: ${report.reason}`);
    lines.push("");
    return `${lines.join("\n")}\n`;
}

function resolveTrend(currentScore: number, previousScore: number|null, benchmarkTarget: number,
                      regressionFloor: number):
    {trendStatus: string; gatePass : boolean; reason : string}
{
    const tolerance = 0.0001;

    if (currentScore < regressionFloor)
    {
        return {
            trendStatus : "regressed-below-floor",
            gatePass : false,
            reason : `Current score ${currentScore.toFixed(4)} is below floor ${
                regressionFloor.toFixed(4)}.`,
        };
    }

    if (previousScore == null)
    {
        if (currentScore >= benchmarkTarget)
        {
            return {
                trendStatus : "meets-benchmark-no-baseline",
                gatePass : true,
                reason :
                    "No previous baseline artifact found; current score meets benchmark target.",
            };
        }

        return {
            trendStatus : "no-baseline",
            gatePass : true,
            reason :
                "No previous baseline artifact found; floor is satisfied and trend comparison skipped.",
        };
    }

    if (currentScore >= benchmarkTarget)
    {
        if (currentScore + tolerance < previousScore)
        {
            return {
                trendStatus : "meets-benchmark-soft-regression",
                gatePass : true,
                reason :
                    "Current score remains above benchmark target despite a small drop from previous run.",
            };
        }

        return {
            trendStatus : "meets-benchmark",
            gatePass : true,
            reason : "Current score meets benchmark target and floor.",
        };
    }

    if (currentScore + tolerance < previousScore)
    {
        return {
            trendStatus : "regressed-vs-previous",
            gatePass : false,
            reason : `Current score ${currentScore.toFixed(4)} regressed below previous ${
                previousScore.toFixed(
                    4)} while still below benchmark target ${benchmarkTarget.toFixed(4)}.`,
        };
    }

    if (currentScore > previousScore + tolerance)
    {
        return {
            trendStatus : "growing",
            gatePass : true,
            reason : "Current score improved relative to previous run.",
        };
    }

    return {
        trendStatus : "stable",
        gatePass : true,
        reason : "Current score is stable versus previous run and above floor.",
    };
}

function main(): number
{
    const args = parseArgs(process.argv.slice(2));

    if (!existsSync(args.currentReportPath))
    {
        throw new Error(`Current report not found: ${args.currentReportPath}`);
    }

    const current = readReport(args.currentReportPath);
    const currentScore = Number(current.score ?? Number.NaN);
    if (!Number.isFinite(currentScore))
    {
        throw new Error(`Current report has invalid score: ${args.currentReportPath}`);
    }

    let previousScore: number|null = null;
    if (args.previousReportPath && existsSync(args.previousReportPath))
    {
        const previous = readReport(args.previousReportPath);
        const parsedPrevious = Number(previous.score ?? Number.NaN);
        if (Number.isFinite(parsedPrevious))
        {
            previousScore = parsedPrevious;
        }
    }

    const benchmarkTarget = args.benchmarkTarget;
    const regressionFloor = args.regressionFloor;

    const resolution = resolveTrend(currentScore, previousScore, benchmarkTarget, regressionFloor);
    const generatedAtUtc = new Date().toISOString();

    const report = {
        generated_at_utc : generatedAtUtc,
        current_score : Number(currentScore.toFixed(4)),
        previous_score : previousScore == null ? null : Number(previousScore.toFixed(4)),
        benchmark_target : benchmarkTarget,
        regression_floor : regressionFloor,
        trend_status : resolution.trendStatus,
        gate_pass : resolution.gatePass,
        reason : resolution.reason,
        current_report_path : args.currentReportPath,
        previous_report_path : args.previousReportPath ?? null,
    };

    mkdirSync(args.outDir, {recursive : true});
    writeFileSync(join(args.outDir, "quiz-benchmark-trend-gate-report.json"),
                  `${JSON.stringify(report, null, 2)}\n`, "utf-8");
    writeFileSync(join(args.outDir, "quiz-benchmark-trend-gate-report.md"), toMarkdown({
                      generatedAtUtc,
                      currentScore,
                      previousScore,
                      benchmarkTarget,
                      regressionFloor,
                      trendStatus : resolution.trendStatus,
                      gatePass : resolution.gatePass,
                      reason : resolution.reason,
                  }),
                  "utf-8");

    if (!resolution.gatePass)
    {
        console.error("[quiz-benchmark-trend-gate] trend policy failure");
        console.error(`- trend_status=${resolution.trendStatus}`);
        console.error(`- reason=${resolution.reason}`);
        return 1;
    }

    return 0;
}

try
{
    process.exit(main());
}
catch (error)
{
    console.error(`[quiz-benchmark-trend-gate] fatal: ${
        error instanceof Error ? error.message : String(error)}`);
    process.exit(1);
}
