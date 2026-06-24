#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
packages_root="${repo_root}/src/typescript/packages/panels"
modules_root="${packages_root}/modules"

dry_run="${PANELS_PUBLISH_DRY_RUN:-true}"
access="${PANELS_PUBLISH_ACCESS:-public}"

if [[ -z "${NODE_AUTH_TOKEN:-}" && -f "${repo_root}/.env" ]]; then
  token_from_env="$(sed -n 's/^NPM_TOKEN=//p' "${repo_root}/.env" | tail -n 1 | tr -d '"\r')"
  if [[ -n "${token_from_env}" ]]; then
    export NODE_AUTH_TOKEN="${token_from_env}"
  fi
fi

if [[ "${dry_run}" != "true" && "${dry_run}" != "false" ]]; then
  echo "PANELS_PUBLISH_DRY_RUN must be true or false" >&2
  exit 1
fi

if [[ "${dry_run}" == "false" && -z "${NODE_AUTH_TOKEN:-}" ]]; then
  echo "NODE_AUTH_TOKEN is required when PANELS_PUBLISH_DRY_RUN=false" >&2
  exit 1
fi

mapfile -t module_dirs < <(find "${modules_root}" -mindepth 1 -maxdepth 1 -type d | sort)
package_dirs=("${packages_root}")
for module_dir in "${module_dirs[@]}"; do
  package_dirs+=("${module_dir}")
done

for package_dir in "${package_dirs[@]}"; do
  if [[ ! -f "${package_dir}/package.json" ]]; then
    continue
  fi

  package_name="$(node -e "const fs=require('fs');const p=process.argv[1];const j=JSON.parse(fs.readFileSync(p,'utf8'));process.stdout.write(j.name||'unknown');" "${package_dir}/package.json")"

  echo "Publishing ${package_name} from ${package_dir} (dry-run=${dry_run})"

  pushd "${package_dir}" >/dev/null
  if [[ "${dry_run}" == "true" ]]; then
    npm publish --access "${access}" --dry-run
  else
    npm publish --access "${access}" --provenance
  fi
  popd >/dev/null

done
