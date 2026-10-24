#!/usr/bin/env bash
# Rebuild the images used in the README from a fresh demo run.
# Requires ffmpeg for the animated GIF.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BIN="$ROOT/build/bin"
OUT="$ROOT/outputs"
IMG="$ROOT/docs/images"

"$ROOT/scripts/run_all.sh"

mkdir -p "$IMG"
cp "$OUT/defect/defect_parts_annotated.jpg" "$IMG/defect-inspection.jpg"
cp "$OUT/labels/label_ok_01_annotated.jpg" "$IMG/label-localization.jpg"
cp "$OUT/labels/label_ok_01_aligned.png" "$IMG/label-aligned.png"
cp "$OUT/labels/label_missing_annotated.jpg" "$IMG/label-missing.jpg"

ffmpeg -y -loglevel error -i "$OUT/video-track/annotated.mp4" \
    -vf "select=eq(n\,125)" -vframes 1 "$IMG/video-tracking.jpg"
ffmpeg -y -loglevel error -i "$OUT/video-motion/annotated.mp4" \
    -vf "select=eq(n\,130)" -vframes 1 "$IMG/video-motion.jpg"
ffmpeg -y -loglevel error -i "$OUT/video-flow/annotated.mp4" \
    -vf "select=eq(n\,120)" -vframes 1 "$IMG/video-flow.jpg"

ffmpeg -y -loglevel error -i "$OUT/video-track/annotated.mp4" -t 6 \
    -vf "fps=10,scale=480:-1:flags=lanczos,palettegen" /tmp/opencv_portfolio_palette.png
ffmpeg -y -loglevel error -i "$OUT/video-track/annotated.mp4" \
    -i /tmp/opencv_portfolio_palette.png -t 6 \
    -lavfi "fps=10,scale=480:-1:flags=lanczos[x];[x][1:v]paletteuse" \
    "$IMG/video-tracking.gif"

echo "Updated images in $IMG"
