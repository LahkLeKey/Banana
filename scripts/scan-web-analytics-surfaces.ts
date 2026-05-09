#!/usr/bin/env bun
/**
 * scan-web-analytics-surfaces.ts — Spec 256
 *
 * Scans the React route tree and interaction surfaces for analytics
 * instrumentation coverage and emits deterministic JSON + Markdown reports.
 *
 * Usage:
 *   bun scripts/scan-web-analytics-surfaces.ts [options]
 *
 * Options:
 *   --manifest <path>            Coverage manifest (default:
 * scripts/analytics-surface-manifest.json)
 *   --out-dir <path>             Output directory   (default: artifacts/analytics)
 *   --router-file <path>         React router source (default:
 * src/typescript/react/src/lib/router.tsx)
 *   --analytics-file <path>      Analytics helper   (default:
 * src/typescript/react/src/lib/analytics.ts)
 *   --workspace-shell-file <path> WorkspaceShell   (default:
 * src/typescript/react/src/components/WorkspaceShell.tsx)
 *   --strict                     Exit 1 when coverage thresholds are not met
 *   --min-route-coverage <0-1>   Minimum required route coverage ratio (default: 1)
 *   --min-event-coverage <0-1>   Minimum required event coverage ratio  (default: 1)
 *
 * Outputs:
 *   <out-dir>/coverage.json — machine-readable coverage report
 *   <out-dir>/coverage.md  — human-readable summary
 *
 * Remediation:
 *   Missing routes:  All routes in the React router receive page-view coverage
 *                    automatically via PageShell → trackPageView. Add the route
 *                    to the manifest's requiredRoutes if intentional.
 *   Missing events:  Add a trackEvent("event_name", ...) call to the relevant
 *                    component and ensure the event name is in @banana/react
 *                    analytics.ts or WorkspaceShell.tsx for discovery.
 */

import {existsSync, mkdirSync, readFileSync, writeFileSync} from "node:fs";
import {join, resolve} from "node:path";

type Args = {
    manifestPath: string; outDir : string; routerPath : string; analyticsPath : string;
    workspaceShellPath : string;
    strict : boolean;
    minRouteCoverage : number;
    minEventCoverage : number;
};

type CriticalWorkflow = {
    name: string; requiredRoutes : string[]; requiredEvents : string[];
};

type Manifest = {
    criticalWorkflows: CriticalWorkflow[];
};

type RouteCoverage = {
    route: string; instrumented : boolean; required : boolean;
};

type EventCoverage = {
    event: string; present : boolean; required : boolean;
};

const DETERMINISTIC_GENERATED_AT = "1970-01-01T00:00:00Z";

function resolveFirstExisting(cwd: string, candidates: string[]): string
{
    for (const candidate of candidates)
    {
        const absolute = resolve(cwd, candidate);
        if (existsSync(absolute))
        {
            return absolute;
        }
    }
    return resolve(cwd, candidates[0]);
}

function parseArgs(argv: string[]): Args
{
    const cwd = process.cwd();
    const args: Args = {
        manifestPath : resolve(cwd, "scripts/analytics-surface-manifest.json"),
        outDir : resolve(cwd, "artifacts/analytics"),
        routerPath : resolve(cwd, "src/typescript/react/src/lib/router.tsx"),
        analyticsPath : resolve(cwd, "src/typescript/react/src/lib/analytics.ts"),
        workspaceShellPath :
            resolveFirstExisting(cwd,
                                 [
                                     "src/typescript/react/src/components/WorkspaceShell.tsx",
                                     "src/typescript/react/src/App.tsx",
                                 ]),
        strict : false,
        minRouteCoverage : 1,
        minEventCoverage : 1,
    };

    for (let i = 0; i < argv.length; i += 1)
    {
        const token = argv[i];
        switch (token)
        {
        case "--manifest":
            args.manifestPath = resolve(cwd, argv[++i]);
            break;
        case "--out-dir":
            args.outDir = resolve(cwd, argv[++i]);
            break;
        case "--router-file":
            args.routerPath = resolve(cwd, argv[++i]);
            break;
        case "--analytics-file":
            args.analyticsPath = resolve(cwd, argv[++i]);
            break;
        case "--workspace-shell-file":
            args.workspaceShellPath = resolve(cwd, argv[++i]);
            break;
        case "--strict":
            args.strict = true;
            break;
        case "--min-route-coverage":
            args.minRouteCoverage = Number(argv[++i]);
            break;
        case "--min-event-coverage":
            args.minEventCoverage = Number(argv[++i]);
            break;
        default:
            throw new Error(`Unknown argument: ${token}`);
        }
    }

    if (!Number.isFinite(args.minRouteCoverage) || args.minRouteCoverage < 0 ||
        args.minRouteCoverage > 1)
    {
        throw new Error("--min-route-coverage must be a number between 0 and 1");
    }
    if (!Number.isFinite(args.minEventCoverage) || args.minEventCoverage < 0 ||
        args.minEventCoverage > 1)
    {
        throw new Error("--min-event-coverage must be a number between 0 and 1");
    }

    return args;
}

function resolveWorkspaceShellPath(requestedPath: string): string
{
    if (existsSync(requestedPath))
    {
        return requestedPath;
    }

    return resolveFirstExisting(process.cwd(), [
        "src/typescript/react/src/components/WorkspaceShell.tsx",
        "src/typescript/react/src/App.tsx",
    ]);
}

function readUtf8(path: string): string
{
    return readFileSync(path, "utf-8");
}

function normalizeRoute(pathValue: string): string
{
    if (pathValue === "*")
    {
        return "/*";
    }
    return pathValue.startsWith("/") ? pathValue : `/${pathValue}`;
}

function extractRoutes(routerSource: string): string[]
{
    const routeSet = new Set<string>();

    if (/\{\s*index\s*:\s*true\s*,/m.test(routerSource))
    {
        routeSet.add("/");
    }

    const pathRegex = /path\s*:\s*"([^"]+)"/g;
    let match: RegExpExecArray|null;
    while ((match = pathRegex.exec(routerSource)) != null)
    {
        routeSet.add(normalizeRoute(match[1]));
    }

    const legacyRoutesMatch = routerSource.match(
        /const\s+legacyRoutes\s*=\s*\[([\s\S]*?)\];/m,
    );
    if (legacyRoutesMatch)
    {
        const entryRegex = /["']([^"']+)["']/g;
        let entry: RegExpExecArray|null;
        while ((entry = entryRegex.exec(legacyRoutesMatch[1])) != null)
        {
            routeSet.add(normalizeRoute(entry[1]));
        }
    }

    return [...routeSet ].sort((a, b) => a.localeCompare(b));
}

function extractEvents(source: string): string[]
{
    const eventSet = new Set<string>();
    const eventRegex = /trackEvent\(\s*"([a-z0-9_\-]+)"/g;
    let match: RegExpExecArray|null;
    while ((match = eventRegex.exec(source)) != null)
    {
        eventSet.add(match[1]);
    }
    return [...eventSet ].sort((a, b) => a.localeCompare(b));
}

function unionSorted(values: string[][]): string[]
{
    const all = new Set<string>();
    for (const list of values)
    {
        for (const item of list)
        {
            all.add(item);
        }
    }
    return [...all ].sort((a, b) => a.localeCompare(b));
}

function ratio(covered: number, total: number): number
{
    if (total <= 0)
    {
        return 1;
    }
    return Number((covered / total).toFixed(4));
}

function toMarkdown(report: {
    routeCoverage: RouteCoverage[]; eventCoverage : EventCoverage[]; requiredRoutes : string[];
    requiredEvents : string[];
    missingRoutes : string[];
    missingEvents : string[];
    routeCoveragePct : number;
    eventCoveragePct : number;
    strict : boolean;
    minRouteCoverage : number;
    minEventCoverage : number;
}): string
{
    const lines: string[] = [];
    lines.push("# Analytics Surface Coverage");
    lines.push("");
    lines.push(`- route_coverage: ${(report.routeCoveragePct * 100).toFixed(2)}% (${
        report.requiredRoutes.length === 0
            ? 0
            : report.requiredRoutes.length -
                  report.missingRoutes.length}/${report.requiredRoutes.length || 0})`);
    lines.push(`- event_coverage: ${(report.eventCoveragePct * 100).toFixed(2)}% (${
        report.requiredEvents.length === 0
            ? 0
            : report.requiredEvents.length -
                  report.missingEvents.length}/${report.requiredEvents.length || 0})`);
    lines.push(`- strict_mode: ${report.strict}`);
    lines.push(
        `- thresholds: route>=${report.minRouteCoverage}, event>=${report.minEventCoverage}`);
    lines.push("");

    lines.push("## Missing Required Routes");
    lines.push("");
    if (report.missingRoutes.length === 0)
    {
        lines.push("- None");
    }
    else
    {
        for (const route of report.missingRoutes)
        {
            lines.push(`- ${route}`);
        }
    }
    lines.push("");

    lines.push("## Missing Required Events");
    lines.push("");
    if (report.missingEvents.length === 0)
    {
        lines.push("- None");
    }
    else
    {
        for (const event of report.missingEvents)
        {
            lines.push(`- ${event}`);
        }
    }
    lines.push("");

    lines.push("## Route Coverage Details");
    lines.push("");
    lines.push("| Route | Required | Instrumented |");
    lines.push("| --- | --- | --- |");
    for (const entry of report.routeCoverage)
    {
        lines.push(`| ${entry.route} | ${entry.required} | ${entry.instrumented} |`);
    }
    lines.push("");

    lines.push("## Event Coverage Details");
    lines.push("");
    lines.push("| Event | Required | Present |");
    lines.push("| --- | --- | --- |");
    for (const entry of report.eventCoverage)
    {
        lines.push(`| ${entry.event} | ${entry.required} | ${entry.present} |`);
    }
    lines.push("");

    return `${lines.join("\n")}\n`;
}

function main(): number
{
    const args = parseArgs(process.argv.slice(2));

    const manifest = JSON.parse(readUtf8(args.manifestPath)) as Manifest;
    const routerSource = readUtf8(args.routerPath);
    const analyticsSource = readUtf8(args.analyticsPath);
    const shellSource = readUtf8(resolveWorkspaceShellPath(args.workspaceShellPath));

    const discoveredRoutes = extractRoutes(routerSource);
    const discoveredEvents =
        unionSorted([ extractEvents(analyticsSource), extractEvents(shellSource) ]);

    const requiredRoutes =
        unionSorted(manifest.criticalWorkflows.map((item) => item.requiredRoutes));
    const requiredEvents =
        unionSorted(manifest.criticalWorkflows.map((item) => item.requiredEvents));

    const hasPageViewInstrumentation = /trackPageView\(/.test(routerSource);

    const routeCoverage: RouteCoverage[] =
        discoveredRoutes.map((route) => ({
                                 route,
                                 required : requiredRoutes.includes(route),
                                 instrumented : hasPageViewInstrumentation,
                             }));

    const allEvents = unionSorted([ discoveredEvents, requiredEvents ]);
    const eventCoverage: EventCoverage[] =
        allEvents.map((event) => ({
                          event,
                          required : requiredEvents.includes(event),
                          present : discoveredEvents.includes(event),
                      }));

    const missingRoutes = requiredRoutes.filter((requiredRoute) => {
        const match = routeCoverage.find((entry) => entry.route === requiredRoute);
        return !match || !match.instrumented;
    });
    const missingEvents =
        requiredEvents.filter((requiredEvent) => !discoveredEvents.includes(requiredEvent));

    const coveredRequiredRouteCount = requiredRoutes.length - missingRoutes.length;
    const coveredRequiredEventCount = requiredEvents.length - missingEvents.length;

    const routeCoveragePct = ratio(coveredRequiredRouteCount, requiredRoutes.length);
    const eventCoveragePct = ratio(coveredRequiredEventCount, requiredEvents.length);

    const report = {
        generatedAtUtc : DETERMINISTIC_GENERATED_AT,
        scannerVersion : 1,
        strict : args.strict,
        thresholds : {
            minRouteCoverage : args.minRouteCoverage,
            minEventCoverage : args.minEventCoverage,
        },
        discovered : {
            routes : discoveredRoutes,
            events : discoveredEvents,
        },
        required : {
            routes : requiredRoutes,
            events : requiredEvents,
        },
        coverage : {
            routeCoveragePct,
            eventCoveragePct,
            routeCoverage,
            eventCoverage,
            missingRoutes,
            missingEvents,
        },
    };

    mkdirSync(args.outDir, {recursive : true});
    writeFileSync(join(args.outDir, "coverage.json"), `${JSON.stringify(report, null, 2)}\n`,
                  "utf-8");
    writeFileSync(join(args.outDir, "coverage.md"), toMarkdown({
                      routeCoverage,
                      eventCoverage,
                      requiredRoutes,
                      requiredEvents,
                      missingRoutes,
                      missingEvents,
                      routeCoveragePct,
                      eventCoveragePct,
                      strict : args.strict,
                      minRouteCoverage : args.minRouteCoverage,
                      minEventCoverage : args.minEventCoverage,
                  }),
                  "utf-8");

    const diagnostics: string[] = [];
    if (routeCoveragePct < args.minRouteCoverage)
    {
        diagnostics.push(`Route coverage ${routeCoveragePct.toFixed(4)} below threshold ${
            args.minRouteCoverage.toFixed(4)}`);
    }
    if (eventCoveragePct < args.minEventCoverage)
    {
        diagnostics.push(`Event coverage ${eventCoveragePct.toFixed(4)} below threshold ${
            args.minEventCoverage.toFixed(4)}`);
    }
    if (missingRoutes.length > 0)
    {
        diagnostics.push(`Missing required routes: ${missingRoutes.join(", ")}`);
    }
    if (missingEvents.length > 0)
    {
        diagnostics.push(`Missing required events: ${missingEvents.join(", ")}`);
    }

    if (args.strict && diagnostics.length > 0)
    {
        console.error("[scan-web-analytics-surfaces] strict coverage failure");
        for (const line of diagnostics)
        {
            console.error(`- ${line}`);
        }
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
    console.error(`[scan-web-analytics-surfaces] fatal: ${
        error instanceof Error ? error.message : String(error)}`);
    process.exit(1);
}
