# luatos-utils

This project provide a set of utils and scripts to compile and generate the script.img for LuatOS with OpenLuat AIR101 / AIR103 Soc/Devboards.

## Build
Type `make` to build the project. `mkscriptbin` and `wm_tool_luatos` had no external dependencies, `luac` will be built as 32bit elf and requires some 32bit libraries and development packages, such as readline, please install it according to your distribution.

After built successfully, `luac` / `mkscriptbin` and `wm_tool_luatos` will be generated at current dir, and will be used by `gen-script-img` and `flash-script-img`.

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

## RTS behavior

Open serial port will cause target device RESET with Linux (Windows will not). If your devboards has RTS or DTR pin connect to RESET pin of the chip, every time the serial port opened, it will reset target device.

Refer to [https://stackoverflow.com/questions/5090451/how-to-open-serial-port-in-linux-without-changing-any-pin](https://stackoverflow.com/questions/5090451/how-to-open-serial-port-in-linux-without-changing-any-pin) for more info.

For LuatOS, it heavily depend on UART log for debugging and monitoring, everytime the serial port opened, it will reset the target device. this hehavior is unacceptable.

For Air 101 and 103 board, there is CH34X UART chip on board, we can change this behavior by modify the driver source code.

You can download the `ch341.c` from `drivers/usb/serial/ch341.c` of upstream kernel, and find `ch341_dtr_rts` function, comment out all contents of this function and put it in `ch341-mod` dir.

Then type `make` to build the new driver, it will rename the driver to `ch341-uart.ko`. you can :
- either blacklist original `ch341.ko` and install this new driver.
- or `sudo rmmod ch341.ko && insmod ./ch341-kmod/ch341-uart.ko` everytime you need it.
 
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
