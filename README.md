# Overview
Nagel–Schreckenberg model
https://en.wikipedia.org/wiki/Nagel%E2%80%93Schreckenberg_model
TODO
# How to use
```bash
make
./main -v
# MP4 usually doesn't play when run with default settings (non-standard resolution)
ffmpeg -i viz/frame_%05d.ppm -r 10 output.mp4 -y
# Use gif as alternative
ffmpeg -i viz/frame_%05d.ppm -r 10 output.gif -y
# Better alternative for smoother colors
ffmpeg -i viz/frame_%05d.ppm -r 10 -filter_complex \
"[0:v]split[a][b];[a]palettegen=reserve_transparent=0[p];[b][p]paletteuse=dither=sierra2_4a" \
output.gif -y
```

### Third-party resources

**Turbo Colormap**
- Author: Anton Mikhailov / Google LLC
- Source: https://ai.googleblog.com/2019/08/turbo-improved-rainbow-colormap.html
- License: Apache 2.0
