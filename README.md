# luatos-utils

This project provide a set of utils and scripts to compile and generate the script.img for LuatOS with OpenLuat AIR101 / AIR103 Soc/Devboards.

## Usage
- create a `src` dir, put lua sources and other resources into it.
- run `gen-script-img [air101|air103] src_dir` to generate `script.img` for air101 or air103.
- run `flash-script-img [air101|air103] <script.img>` to program `script.img` to air101 or air103.

This `script.img` can also be append to LuatOS base firmware and generate a whole img, such as:
```
cat script.img >>AIR101.fls
```

And program to target device as:
```
./wm_tool_luatos -ds 2M -c ttyUSB0 -ws 115200 -rs rts -dl AIR101.fls
```

## UART connection
After script.img programmed, you can use below commands to connect to devboard via UART:

```
tio -b 921600 /dev/ttyUSB0 -m INLCRNL
```
Or

```
picocom -b 921600 /dev/ttyUSB0 --imap spchex,lfcrlf
```

## Dir structure
```
luatos-utils
├── ch341-mod
│   ├── ch341.c
│   └── Makefile
├── flash-script-img : a script to flash script.img to air101 / 103
├── gen-script-img : a script to generate script.img for air101 / 103
├── LICENSE
├── luatos-app-src : LVGL demo project works with AIR 101 / 103 and OpenLuat LCD panel.
│   └── main.lua
├── Makefile : build all related utils
├── README.md : this file
└── utils
    ├── lua-5.3.6.tar.gz : upstream lua 5.3.6 tarball, used to build 32bit luac
    ├── mkscriptbin.c : source of mkscriptbin, a tool to generate script.bin in luadb format
    └── wm_tool_mod_by_luatos.c : source of air101 / 103 firmware convert and flash tool
```
