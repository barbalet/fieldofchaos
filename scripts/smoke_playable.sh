#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
APP_DIR="$ROOT/dist/Field of Chaos.app"
RESOURCES_DIR="$APP_DIR/Contents/Resources"

cd "$ROOT/src"
make test

cd "$ROOT"
swift build
xcodebuild -project FieldOfChaos.xcodeproj -scheme FieldOfChaos -configuration Debug build
sh scripts/package_release.sh

test -x "$APP_DIR/Contents/MacOS/FieldOfChaos"
test -f "$RESOURCES_DIR/AppIcon.icns"
test -f "$RESOURCES_DIR/alice.json"
test -f "$RESOURCES_DIR/bob.json"
test -f "$RESOURCES_DIR/field-of-chaos-rules-natural-english.md"

echo "Playable smoke passed: $APP_DIR"
