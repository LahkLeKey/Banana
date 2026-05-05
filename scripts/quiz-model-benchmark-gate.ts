#!/usr/bin/env bun

import {mkdirSync, writeFileSync} from "node:fs";
import {join, resolve} from "node:path";

type QuizPrompt = {
    id: string; prompt : string; expectedLabel : "banana" | "not-banana";
};

type GateArgs = {
    baseUrl: string; outDir : string; benchmarkTarget : number; regressionFloor : number;
};

const DETERMINISTIC_SCANNER_VERSION = 1;

const prompts: QuizPrompt[] = [
    {
        id : "q-001",
        prompt : "A ripe yellow banana with a few brown spots.",
        expectedLabel : "banana",
    },
    {
        id : "q-002",
        prompt : "A steel keyboard with RGB lights.",
        expectedLabel : "not-banana",
    },
    {
        id : "q-003",
        prompt : "A banana smoothie in a clear cup.",
        expectedLabel : "banana",
    },
    {
        id : "q-004",
        prompt : "A ceramic coffee mug on a desk.",
        expectedLabel : "not-banana",
    },
    {
        id : "q-005",
        prompt : "A peeled banana cut into slices.",
        expectedLabel : "banana",
    },
    {
        id : "q-006",
        prompt : "A laptop charging cable with a USB-C connector.",
        expectedLabel : "not-banana",
    },
];

function parseArgs(argv: string[]): GateArgs
{
    const cwd = process.cwd();
    const args: GateArgs = {
        baseUrl : "https://api.banana.engineer",
        outDir : resolve(cwd, ".artifacts/grade-school-gate"),
        benchmarkTarget : 0.8,
        regressionFloor : 0.65,
    };

    for (let i = 0; i < argv.length; i += 1)
    {
        const token = argv[i];
        switch (token)
        {
        case "--base-url":
            args.baseUrl = argv[++i];
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

type EnsembleResponse = {
    label?: string;
};

function normalizeLabel(rawLabel: string): "banana"|"not-banana"|"unknown"
{
    const normalized = rawLabel.trim().toLowerCase().replace(/[_\s]+/g, "-");
    if (normalized === "banana")
    {
        return "banana";
    }
    if (normalized === "not-banana")
    {
        return "not-banana";
    }
    return "unknown";
}

async function requestVerdict(baseUrl: string, prompt: string): Promise<string>
{
    const response = await fetch(`${baseUrl}/ml/ensemble`, {
        method : "POST",
        headers : {"content-type" : "application/json"},
        body : JSON.stringify({inputJson : JSON.stringify({text : prompt})}),
    });

    if (!response.ok)
    {
        const message = await response.text();
        throw new Error(`request failed (${response.status}): ${message}`);
    }

    const payload = (await response.json()) as EnsembleResponse;
    return normalizeLabel(String(payload.label ?? ""));
}

function scoreToPercent(score: number): string
{
    return `${(score * 100).toFixed(2)}%`;
}

function trendLabel(score: number, benchmarkTarget: number, regressionFloor: number): string
{
    if (score < regressionFloor)
    {
        return "regressed-below-floor";
    }
    if (score >= benchmarkTarget)
    {
        return "meets-benchmark";
    }
    return "growing";
}

function toMarkdown(report: {
    generatedAtUtc: string; targetUrl : string; benchmarkTarget : number; regressionFloor : number;
    score : number;
    trend : string;
    gatePass : boolean;
    rows : Array<{id : string; expectedLabel : string; predictedLabel : string; pass : boolean;}>;
}): string
{
    const lines: string[] = [];
    lines.push("# Quiz Model Benchmark Gate Report");
    lines.push("");
    lines.push(`- generated_at_utc: ${report.generatedAtUtc}`);
    lines.push(`- target_url: ${report.targetUrl}`);
    lines.push(`- benchmark_target: ${report.benchmarkTarget}`);
    lines.push(`- regression_floor: ${report.regressionFloor}`);
    lines.push(`- score: ${scoreToPercent(report.score)}`);
    lines.push(`- trend: ${report.trend}`);
    lines.push(`- gate_pass: ${report.gatePass}`);
    lines.push("");
    lines.push("## Prompt Results");
    lines.push("");
    lines.push("| Prompt | Expected | Predicted | Pass |");
    lines.push("| --- | --- | --- | --- |");
    for (const row of report.rows)
    {
        lines.push(`| ${row.id} | ${row.expectedLabel} | ${row.predictedLabel} | ${row.pass} |`);
    }
    lines.push("");
    return `${lines.join("\n")}\n`;
}

async function main(): Promise<number>
{
    const args = parseArgs(process.argv.slice(2));
    const timestamp = new Date().toISOString();

    const rows: Array<{
        id : string; prompt : string; expectedLabel : string; predictedLabel : string;
        pass : boolean;
    }> = [];

    for (const entry of prompts)
    {
        const predictedLabel = await requestVerdict(args.baseUrl, entry.prompt);
        const pass = predictedLabel === entry.expectedLabel;
        rows.push({
            id : entry.id,
            prompt : entry.prompt,
            expectedLabel : entry.expectedLabel,
            predictedLabel,
            pass,
        });
    }

    const passed = rows.filter((row) => row.pass).length;
    const score = rows.length > 0 ? Number((passed / rows.length).toFixed(4)) : 0;
    const trend = trendLabel(score, args.benchmarkTarget, args.regressionFloor);
    const gatePass = score >= args.regressionFloor;

    const report = {
        generated_at_utc : timestamp,
        scanner_version : DETERMINISTIC_SCANNER_VERSION,
        target_url : args.baseUrl,
        benchmark_target : args.benchmarkTarget,
        regression_floor : args.regressionFloor,
        score,
        trend,
        gate_pass : gatePass,
        prompt_results : rows,
    };

    mkdirSync(args.outDir, {recursive : true});
    writeFileSync(join(args.outDir, "quiz-benchmark-gate-report.json"),
                  `${JSON.stringify(report, null, 2)}\n`, "utf-8");
    writeFileSync(join(args.outDir, "quiz-benchmark-gate-report.md"), toMarkdown({
                      generatedAtUtc : timestamp,
                      targetUrl : args.baseUrl,
                      benchmarkTarget : args.benchmarkTarget,
                      regressionFloor : args.regressionFloor,
                      score,
                      trend,
                      gatePass,
                      rows,
                  }),
                  "utf-8");

    if (!gatePass)
    {
        console.error("[quiz-model-benchmark-gate] regression floor check failed");
        console.error(`- score=${score.toFixed(4)}`);
        console.error(`- regression_floor=${args.regressionFloor.toFixed(4)}`);
        return 1;
    }

    return 0;
}

main().then((code) => process.exit(code)).catch((error) => {
    console.error(`[quiz-model-benchmark-gate] fatal: ${
        error instanceof Error ? error.message : String(error)}`);
    process.exit(1);
});
