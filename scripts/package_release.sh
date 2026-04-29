#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
APP_DIR="$ROOT/dist/Field of Chaos.app"
CONTENTS_DIR="$APP_DIR/Contents"
MACOS_DIR="$CONTENTS_DIR/MacOS"
RESOURCES_DIR="$CONTENTS_DIR/Resources"
EXECUTABLE="$ROOT/.build/release/FieldOfChaosAppShell"

cd "$ROOT"
swift build -c release

mkdir -p "$MACOS_DIR" "$RESOURCES_DIR"
cp -f "$EXECUTABLE" "$MACOS_DIR/FieldOfChaos"
cp -f "$ROOT/packaging/Info.plist" "$CONTENTS_DIR/Info.plist"

for BUNDLE in "$ROOT"/.build/release/*FieldOfChaosAppShell.bundle; do
    if [ -d "$BUNDLE" ]; then
        cp -R "$BUNDLE" "$RESOURCES_DIR/"
    fi
done

if [ -f "$ROOT/AppShell/Shaders/BoardShaders.metal" ]; then
    cp -f "$ROOT/AppShell/Shaders/BoardShaders.metal" "$RESOURCES_DIR/BoardShaders.metal"
fi

echo "Packaged $APP_DIR"
