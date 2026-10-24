#!/usr/bin/env bash
# One-command demo: build, generate synthetic data, run all three projects.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BIN="$ROOT/build/bin"
OUT="$ROOT/outputs"

"$ROOT/scripts/build.sh"

echo
echo "==> generating demo data"
"$BIN/make_demo_data" --what all --out "$ROOT/assets"

echo
echo "==> 01 defect inspection"
"$BIN/defect_inspection" --input "$ROOT/assets/defect_parts.png" --output "$OUT/defect"

echo
echo "==> 02 label localisation"
"$BIN/label_localization" --template "$ROOT/assets/label_template.png" \
    --input "$ROOT/assets/labels" --output "$OUT/labels"

echo
echo "==> 03 video analytics"
for mode in motion track flow; do
    "$BIN/video_analytics" --input "$ROOT/assets/generated/demo_video.mp4" \
        --mode "$mode" --output "$OUT/video-$mode"
    echo
done

echo "All outputs are in: $OUT"
