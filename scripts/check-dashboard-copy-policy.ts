#!/usr/bin/env bun
/**
 * check-dashboard-copy-policy.ts — Spec 861
 *
 * Scans user-visible copy in telemetry dashboard source files for restricted
 * vendor-brand terms. Fails with a non-zero exit code when violations are found.
 *
 * Usage:
 *   bun scripts/check-dashboard-copy-policy.ts [options]
 *
 * Options:
 *   --files <glob,...>  Comma-separated file paths to scan
 *                       (default: TelemetryDashboardPage.tsx + WorkspaceShell.tsx)
 *   --terms <path>      Path to JSON array of restricted term strings
 *                       (default: scripts/dashboard-copy-policy-terms.json)
 *   --strict            Exit 1 on violations (default: true)
 *   --no-strict         Report violations but exit 0 (audit mode)
 *
 * Exit codes:
 *   0  — No violations found (or --no-strict mode)
 *   1  — One or more violations found in strict mode
 *   2  — Fatal error (file not found, bad JSON, etc.)
 */

import { existsSync, readFileSync } from "node:fs";
import { resolve } from "node:path";

const CWD = process.cwd();

const DEFAULT_FILES = [
    "src/typescript/react/src/pages/TelemetryDashboardPage.tsx",
    "src/typescript/react/src/components/WorkspaceShell.tsx",
];

const DEFAULT_TERMS_PATH = "scripts/dashboard-copy-policy-terms.json";

type Args = {
    files: string[];
    termsPath: string;
    strict: boolean;
};

function parseArgs(argv: string[]): Args {
    const args: Args = {
        files: DEFAULT_FILES.map((f) => resolve(CWD, f)),
        termsPath: resolve(CWD, DEFAULT_TERMS_PATH),
        strict: true,
    };

    for (let i = 0; i < argv.length; i += 1) {
        const token = argv[i];
        switch (token) {
            case "--files":
                args.files = (argv[++i] ?? "")
                    .split(",")
                    .map((f) => f.trim())
                    .filter(Boolean)
                    .map((f) => resolve(CWD, f));
                break;
            case "--terms":
                args.termsPath = resolve(CWD, argv[++i]);
                break;
            case "--strict":
                args.strict = true;
                break;
            case "--no-strict":
                args.strict = false;
                break;
            default:
                console.error(`[check-dashboard-copy-policy] unknown argument: ${token}`);
                process.exit(2);
        }
    }
    return args;
}

type Violation = {
    file: string;
    line: number;
    term: string;
    snippet: string;
};

function scan(filePath: string, terms: string[]): Violation[] {
    if (!existsSync(filePath)) {
        console.error(`[check-dashboard-copy-policy] file not found: ${filePath}`);
        process.exit(2);
    }
    const source = readFileSync(filePath, "utf-8");
    const lines = source.split("\n");
    const violations: Violation[] = [];

    for (let lineIdx = 0; lineIdx < lines.length; lineIdx += 1) {
        const line = lines[lineIdx] ?? "";
        // Skip comment-only lines and import statements
        const trimmed = line.trimStart();
        if (trimmed.startsWith("//") || trimmed.startsWith("*") || trimmed.startsWith("import ")) {
            continue;
        }
        for (const term of terms) {
            // Case-insensitive substring match
            if (line.toLowerCase().includes(term.toLowerCase())) {
                violations.push({
                    file: filePath,
                    line: lineIdx + 1,
                    term,
                    snippet: line.trim().slice(0, 120),
                });
            }
        }
    }
    return violations;
}

function main(): number {
    const args = parseArgs(process.argv.slice(2));

    if (!existsSync(args.termsPath)) {
        console.error(
            `[check-dashboard-copy-policy] terms file not found: ${args.termsPath}`,
        );
        process.exit(2);
    }

    let terms: string[];
    try {
        terms = JSON.parse(readFileSync(args.termsPath, "utf-8")) as string[];
        if (!Array.isArray(terms) || terms.some((t) => typeof t !== "string")) {
            throw new Error("terms file must be a JSON array of strings");
        }
    } catch (err) {
        console.error(
            `[check-dashboard-copy-policy] failed to parse terms: ${
                err instanceof Error ? err.message : String(err)
            }`,
        );
        process.exit(2);
    }

    const allViolations: Violation[] = [];
    for (const filePath of args.files) {
        allViolations.push(...scan(filePath, terms));
    }

    if (allViolations.length === 0) {
        console.log(
            `[check-dashboard-copy-policy] OK — no restricted terms found in ${args.files.length} file(s)`,
        );
        return 0;
    }

    console.error(
        `[check-dashboard-copy-policy] found ${allViolations.length} violation(s):`,
    );
    for (const v of allViolations) {
        console.error(`  ${v.file}:${v.line} — restricted term "${v.term}"`);
        console.error(`    ${v.snippet}`);
    }

    if (args.strict) {
        return 1;
    }

    console.warn("[check-dashboard-copy-policy] violations found (non-strict mode — not failing)");
    return 0;
}

try {
    process.exit(main());
} catch (error) {
    console.error(
        `[check-dashboard-copy-policy] fatal: ${
            error instanceof Error ? error.message : String(error)
        }`,
    );
    process.exit(2);
}
