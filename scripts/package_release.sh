#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
APP_DIR="$ROOT/dist/Field of Chaos.app"
CONTENTS_DIR="$APP_DIR/Contents"
MACOS_DIR="$CONTENTS_DIR/MacOS"
RESOURCES_DIR="$CONTENTS_DIR/Resources"
EXECUTABLE="$ROOT/.build/release/FieldOfChaosAppShell"

cd "$ROOT"

if [ ! -f "$ROOT/packaging/AppIcon.icns" ] || [ "$ROOT/scripts/generate_app_icon.swift" -nt "$ROOT/packaging/AppIcon.icns" ]; then
    swift "$ROOT/scripts/generate_app_icon.swift"
fi

swift build -c release

mkdir -p "$MACOS_DIR" "$RESOURCES_DIR"
cp -f "$EXECUTABLE" "$MACOS_DIR/FieldOfChaos"
cp -f "$ROOT/packaging/Info.plist" "$CONTENTS_DIR/Info.plist"
cp -f "$ROOT/packaging/AppIcon.icns" "$RESOURCES_DIR/AppIcon.icns"

for BUNDLE in "$ROOT"/.build/release/*FieldOfChaosAppShell.bundle; do
    if [ -d "$BUNDLE" ]; then
        cp -R "$BUNDLE" "$RESOURCES_DIR/"
    fi
done

if [ -f "$ROOT/AppShell/Shaders/BoardShaders.metal" ]; then
    cp -f "$ROOT/AppShell/Shaders/BoardShaders.metal" "$RESOURCES_DIR/BoardShaders.metal"
fi

for RESOURCE in "$ROOT/docs/alice.json" "$ROOT/docs/bob.json" "$ROOT/docs/field-of-chaos-rules-natural-english.md"; do
    if [ -f "$RESOURCE" ]; then
        cp -f "$RESOURCE" "$RESOURCES_DIR/"
    fi
done

echo "Packaged $APP_DIR"
