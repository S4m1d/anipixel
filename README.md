# General
Media converter for my project with ssd1306 animations

# Buid
release mode:
```bash
make
```
debug mode:
```bash
make debug
```

# Usage
Convert animation spreadsheet (sprites png + metadata-json) into ssd1306 animation (my own simple custom format)
```bash
./build/release/anipixel -m spritesheet_to_ssd1306 -t assets/ass-shoulder-dance -d assets/ass-shoulder-dance/animation.bin
```

Convert png into a frame fitting ssd1306 display
```bash
./release/anipixel -m png_to_ssd1306 -t assets/some_image.png -d assets/frame.bin
```
