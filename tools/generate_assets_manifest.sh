#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/.build"
BIN="$BUILD_DIR/generate_assets_manifest"

if [[ $# -eq 0 ]]; then
  echo "Usage: $0 --asset-root <dir> [--output <file>] [--zip-extract <path>] [--no-zip-package]" >&2
  exit 1
fi

mkdir -p "$BUILD_DIR"
c++ -std=c++17 -O2 "$SCRIPT_DIR/generate_assets_manifest.cpp" -o "$BIN"

"$BIN" "$@"
