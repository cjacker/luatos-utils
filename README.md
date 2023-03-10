# luatos-utils

This project provide a set of utils and scripts to compile and generate the script.img for LuatOS with OpenLuat AIR101 / AIR103 Soc/Devboards.

## Usage
- create 'luatos-app-src' dir, put lua sources and other resources into it.
- run `gen-script-img [air101|air103]` to generate script.img for air101 or air103.
- run `flash-script-img [air101|air103] <script.img>` to program script.img to air101 or air103.

This `script.img` can be append to LuatOS base firmware and generate a whole img, such as:
```
cat script.img >>AIR101.fls
```

And program to target device as:
```
./wm_tool_luatos -ds 2M -c ttyUSB0 -ws 115200 -rs rts -dl AIR101.fls
```
