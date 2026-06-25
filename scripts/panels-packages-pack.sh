#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
packages_root="${repo_root}/src/typescript/packages/panels"
modules_root="${packages_root}/modules"
out_root="${repo_root}/artifacts/npm-packages"

mkdir -p "${out_root}"

if [[ ! -d "${packages_root}" ]]; then
  echo "Panels package root not found: ${packages_root}" >&2
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
  safe_name="${package_name//@/}"
  safe_name="${safe_name//\//-}"
  destination="${out_root}/${safe_name}"

  rm -rf "${destination}"
  mkdir -p "${destination}"

  echo "Packing ${package_name} from ${package_dir}"
  npm pack "${package_dir}" --pack-destination "${destination}" >/dev/null

done

echo "Packed panel packages to ${out_root}"
