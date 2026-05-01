#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
APP_DIR="$ROOT/dist/Field of Chaos.app"
REPORT_DIR="$ROOT/reports"
REPORT_PATH="$REPORT_DIR/ui-exploration-latest.json"
LIMIT="${FOC_UI_LIMIT:-0}"

cd "$ROOT"

if [ ! -x "$APP_DIR/Contents/MacOS/FieldOfChaos" ]; then
    sh scripts/package_release.sh
fi

mkdir -p "$REPORT_DIR"

if [ "$LIMIT" = "0" ]; then
    swift UITests/FieldOfChaosExploratoryUITests.swift --app "$APP_DIR" --report "$REPORT_PATH"
else
    swift UITests/FieldOfChaosExploratoryUITests.swift --app "$APP_DIR" --report "$REPORT_PATH" --limit "$LIMIT"
fi
