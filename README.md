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


**Notes**
Cell length = 5m (car length ~ 4.5m) -> 4.5 + 0.5 = 5m
delta(t) = 1s   -> 1 step == 1s
vMax = 3
Colors: (from google maps mapped to cell speed) [avergae speeds per lane]
    - red = 0-1
    - yellow = 1-2
    - green = 2-3
1 cell/update = 18km/h
Junction: https://www.google.com/maps/@49.1871103,16.6193066,51m/data=!3m1!1e3!5m1!1e1?entry=ttu&g_ep=EgoyMDI1MTEyMy4xIKXMDSoASAFQAw%3D%3D