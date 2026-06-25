#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
packages_root="${repo_root}/src/typescript/packages/panels"
modules_root="${packages_root}/modules"

dry_run="${PANELS_PUBLISH_DRY_RUN:-true}"
access="${PANELS_PUBLISH_ACCESS:-public}"
tmp_userconfig=""

if [[ -f "${repo_root}/.env" ]]; then
  if [[ -z "${NODE_AUTH_TOKEN:-}" ]]; then
    token_from_env="$(sed -n 's/^NODE_AUTH_TOKEN=//p' "${repo_root}/.env" | tail -n 1 | tr -d '"\r')"
    if [[ -z "${token_from_env}" ]]; then
      token_from_env="$(sed -n 's/^NPM_TOKEN=//p' "${repo_root}/.env" | tail -n 1 | tr -d '"\r')"
    fi
    if [[ -n "${token_from_env}" ]]; then
      export NODE_AUTH_TOKEN="${token_from_env}"
    fi
  fi

  if [[ -z "${NPM_CONFIG_OTP:-}" ]]; then
    otp_from_env="$(sed -n 's/^NPM_OTP=//p' "${repo_root}/.env" | tail -n 1 | tr -d '"\r')"
    if [[ -n "${otp_from_env}" ]]; then
      export NPM_CONFIG_OTP="${otp_from_env}"
    fi
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

cleanup() {
  if [[ -n "${tmp_userconfig}" && -f "${tmp_userconfig}" ]]; then
    rm -f "${tmp_userconfig}"
  fi
}
trap cleanup EXIT

if [[ "${dry_run}" == "false" ]]; then
  tmp_userconfig="$(mktemp)"
  cat >"${tmp_userconfig}" <<EOF
registry=https://registry.npmjs.org/
always-auth=true
//registry.npmjs.org/:_authToken=${NODE_AUTH_TOKEN}
EOF
  export NPM_CONFIG_USERCONFIG="${tmp_userconfig}"
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
    if [[ -n "${NPM_CONFIG_OTP:-}" ]]; then
      npm publish --access "${access}" --provenance --otp "${NPM_CONFIG_OTP}"
    else
      npm publish --access "${access}" --provenance
    fi
  fi
  popd >/dev/null

done
