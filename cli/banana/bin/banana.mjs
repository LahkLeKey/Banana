#!/usr/bin/env bun

const argv = process.argv.slice(2);

function printRootHelp() {
  console.log(`banana CLI (scaffold)

Usage:
  banana <command> [options]

Commands:
  k3h4      K3H4 command group

Examples:
  banana --help
  banana k3h4 --help
`);
}

function printK3h4Help() {
  console.log(`banana k3h4 (scaffold)

Usage:
  banana k3h4 <command> [options]

Commands:
  sample    Generate deterministic sample dataset (stub)
  run       Execute K3H4 via API contract path (stub)
  explain   Explain K3H4 outputs (stub)
  export    Export K3H4 artifacts (stub)

Global options:
  --api-url <url>    Override API base URL (default: http://localhost:3000)

Examples:
  banana k3h4 sample --help
  banana k3h4 run --help
`);
}

function printSubcommandHelp(name) {
  console.log(`banana k3h4 ${name} (stub)

Status:
  This command is scaffolded but not implemented yet.

Usage:
  banana k3h4 ${name} [options]

Notes:
  - V1 will use stdin JSON as canonical input/output chaining contract.
  - V1 defaults to strict validation and non-zero exit on contract violations.
`);
}

function notImplemented(name) {
  console.error(`banana k3h4 ${name}: not implemented yet (scaffold placeholder)`);
  process.exitCode = 2;
}

if (argv.length === 0 || argv[0] === "--help" || argv[0] === "-h") {
  printRootHelp();
  process.exit(0);
}

const [command, ...rest] = argv;

if (command !== "k3h4") {
  console.error(`Unknown command: ${command}`);
  printRootHelp();
  process.exit(1);
}

if (rest.length === 0 || rest.includes("--help") || rest.includes("-h")) {
  printK3h4Help();
  process.exit(0);
}

const [subcommand, ...subArgs] = rest;
const supported = new Set(["sample", "run", "explain", "export"]);

if (!supported.has(subcommand)) {
  console.error(`Unknown k3h4 subcommand: ${subcommand}`);
  printK3h4Help();
  process.exit(1);
}

if (subArgs.includes("--help") || subArgs.includes("-h")) {
  printSubcommandHelp(subcommand);
  process.exit(0);
}

notImplemented(subcommand);