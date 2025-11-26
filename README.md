# Overview
Nagel–Schreckenberg model
https://en.wikipedia.org/wiki/Nagel%E2%80%93Schreckenberg_model
TODO
# How to use
```bash
make
./main -v
# MP4 usually doesn't play when run with default settings (non-standard resolution)
ffmpeg -i viz/frame_%05d.ppm -r 20 output.mp4 
# Use gif as alternative
ffmpeg -i viz/frame_%05d.ppm -r 20 output.gif
# Better alternative (to get true colors)
ffmpeg -i viz/frame_%05d.ppm -r 20 -filter_complex \
"[0:v]split[a][b];[a]palettegen=reserve_transparent=0[p];[b][p]paletteuse=dither=sierra2_4a" \
output.gif
```
