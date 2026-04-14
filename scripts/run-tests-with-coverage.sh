#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

if [[ -z "${BANANA_PG_CONNECTION:-}" ]]; then
	echo "BANANA_PG_CONNECTION is required for Docker test runs with native DB enabled."
	exit 1
fi

cmake --build build/native --config Release

if ! ctest --test-dir build/native -C Release --output-on-failure; then
	ctest --test-dir build/native --output-on-failure
fi

gcovr --object-directory build/native --root . --filter 'src/native' --exclude 'tests/native' --xml-pretty --output build/native/coverage-native.xml
gcovr --object-directory build/native --root . --filter 'src/native' --exclude 'tests/native' --txt > build/native/Summary.txt

dotnet test tests/unit/CInteropSharp.UnitTests.csproj -c Release --collect:"XPlat Code Coverage"
dotnet test tests/integration/CInteropSharp.IntegrationTests.csproj -c Release --collect:"XPlat Code Coverage"

if [[ -d /artifacts ]]; then
	mkdir -p /artifacts
	cp -f build/native/coverage-native.xml /artifacts/coverage-native.xml || true
	cp -f build/native/Summary.txt /artifacts/coverage-native-summary.txt || true

	while IFS= read -r -d '' file; do
		base_name="$(basename "$(dirname "$file")")"
		cp -f "$file" "/artifacts/coverage-${base_name}.cobertura.xml"
	done < <(find tests -path '*/TestResults/*/coverage.cobertura.xml' -print0)
fi

echo "Coverage artifacts:"
echo "  Native C: build/native/coverage-native.xml"
echo "  Native C summary: build/native/Summary.txt"
echo "  .NET: TestResults/**/coverage.cobertura.xml"
