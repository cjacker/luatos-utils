# luatos-utils

This project provide various utilities to compile user's lua scripts and generate `script.img` or `script.bin` for LuatOS with OpenLuat AIR101 / AIR103 / ESP32S3 / ESP32C3 Soc and devboards.

You can play with LuatOS on Linux now.

```
$ luatos-flash-soc <luatos soc file>
$ luatos-gen-script-img <source dir>
$ luatos-flash-script-img <script bin or img>
```

**NOTE :** There is no plan to support AIR105 and AIR780E up to now. AIR780E doesn't have a Linux flash tool. AIR105 and MH1903S is identical and based on ARM core, since it didn't export SWD interface, without modify the hardware of AIR105 devboard, there is no way to flash it with CMSIS-DAP, Jlink, ST-Link, etc. Either you switch to Windows to use LuatTools, or you can use [air105-uploader](https://github.com/racerxdl/air105-uploader) with [mh1903s firmware library](https://github.com/cjacker/mh1903_firmware_library_gcc_makefile) instead.

## About LuatOS

[LuatOS](https://github.com/openLuat/LuatOS) is a opensource, powerful embedded Lua engine for IoT devices with many components. It already support lots of different SOCs (even different arch), such as W806/AIR101/103, AIR105, AIR780E. 

For a complete tutorial about using LuatOS with AIR101 / AIR103, refer to : https://github.com/cjacker/opensource-toolchain-w80x-air101-air103

The upstream flash tool is LuatTools, it combined different flash tools together and able to flash various SOC that LuatOS already supported. But this tool is close source and only for windows, even couldn't run with wine on Linux. For Linux users, it's difficult to flash soc firmware and lua script of LuatOS to target device.

## Build and Install

To build:
```
make
```

To install utilities to system wide (it's not always necessary, all utils are able to work at current dir without installation):
```
sudo make install DESTDIR=<dest dir>
```

**NOTE 1 :** There are some changes to upstream `luac` includes:
- built with "-DLUA_32BITS"
- although `-DLUA_32BITS` already set, compared with 32bit `luac`, the result file compiled by 64bit `luac` still generate a different file header, the root cause is the header of luac file contains a byte of `sizeof(size_t)`, for 64bit, it's 8, and for 32bit, it's 4. to get the same result of 32bit `luac`, I changed it to `sizeof(int)` for 64bit luac.

All above changes was done in `Makefile` when building luac.

**NOTE 2 :** In order to flash to ESP32S3 and ESP32C3, you should have [esptool.py](https://github.com/espressif/esptool) installed.

## Usage

### To flash LuatOS soc firmware

Usually, LuatOS release pre-built base firmwares (the `.soc` file) for several SOCs.
Base firmware works as a fundamental layer which support runing users' lua scripts upon it.

You can download various pre-built base firmwares from [LuatOS project](https://gitee.com/openLuat/LuatOS/releases): 

- For AIR101 : https://gitee.com/openLuat/LuatOS/releases/download/v0007.air101.v0015/core_V0015.zip
- For AIR103 : https://gitee.com/openLuat/LuatOS/releases/download/v0007.air103.v0015/core_V0015.zip
- For ESP32S3 : https://gitee.com/openLuat/LuatOS/releases/download/v0007.esp32s3.v1003/core_V1003.zip
- For ESP32C3 : https://gitee.com/openLuat/LuatOS/releases/download/v0007.esp32c3.v1003/core_V1003.zip

After download and extracted, you will find one or more `*.soc` files, the soc file can be flashed to target device by:

```
luatos-flash-soc <soc file>
```

### To build and flash LuatOS script image

The lua scripts written by users will be compiled and flashed as 'script.bin' (ESP32S3/C3) or 'script.img' (AIR101/103), you can follow below step:
- create a `src` dir, put lua sources and other resources into it.
- run `luatos-gen-script-img [air101|air103|esp32] src_dir` to compile the lua sources and generate `script.img` for air101 / air103 or `script.bin` for esp32s3 /esp32c3.
- run `luatos-flash-script-img [air101|air103|esp32c3|esp32s3] <script.img>` to program `script.img` to air101 / air103 or `script.bin` to esp32s3 / esp32c3.

Using 'lcd-demo' with OpenLuat 0.96 lcd panel and AIR101 as example:
```
luatos-gen-script-img air101 demos/lcd-demo
luatos-flash-script-img air101 script-lcd-demo-air101.img
```
**NOTE :** `lvgl-demo` in this repo requires base firmware support lvgl.


## UART connection

After script image programmed, you can use below commands to connect to devboard via UART, 921600 is the default baudrate defined by base firmware:

```
tio -b 921600 /dev/ttyUSB0 -m INLCRNL
```
Or

```
picocom -b 921600 /dev/ttyUSB0 --imap spchex,lfcrlf
```

## RTS behavior of Linux

Openning serial port will cause target device RESET on Linux (and Windows will not). If your devboards has RTS or DTR pin connect to RESET pin of the chip, every time the serial port opened, target device reset.

Refer to [https://stackoverflow.com/questions/5090451/how-to-open-serial-port-in-linux-without-changing-any-pin](https://stackoverflow.com/questions/5090451/how-to-open-serial-port-in-linux-without-changing-any-pin) for more info about this serial port behavior.

For LuatOS, it heavily depend on UART log for debugging and monitoring. "reset device every time when serial port openned ?" - this hehavior is unacceptable.

For Air 101 and 103 devboards, there is CH34X UART chip on board, we can change this behavior by modify the source codes of ch341 driver.

You can download the `ch341.c` from `drivers/usb/serial/ch341.c` of upstream kernel, and find `ch341_dtr_rts` function, comment out all contents of this function and put it in `ch341-mod` dir.

Then type `make` to build the new driver, it will rename the driver to `ch341-uart.ko`. you can :
- either blacklist original `ch341.ko` and install this new driver.
- or `sudo rmmod ch341.ko && insmod ./ch341-kmod/ch341-uart.ko` everytime you need it.
 
## Dir structure
```
luatos-utils
├── ch341-mod : changed sources code fo ch341 kernel driver.
├── luatos-flash-soc : flash LuatOS soc file to air101/103 and esp32s3/c3
├── luatos-flash-script-img : flash script.img to air101/103 and script.bin to esp32s3/c3
├── luatos-gen-script-img : generate script.img for air101/103 and script.bin for escp32s3/c3
├── LICENSE
├── demos : lcd/lvgl/blink demos for air101/103 and esp32s3/c3 devboard
├── Makefile
├── README.md
└── utils
    ├── lua-5.3.6.tar.gz : upstream lua 5.3.6 tarball, used to build 32bit luac
    ├── mkscriptbin.c : source of mkscriptbin, a tool to generate script.bin in luadb format
    └── wm_tool_mod_by_luatos.c : source of air101 / 103 firmware convert and flash tool
```

## More about soc file

LuatOS soc firmware is actually a 7z archive, you can extract it by :
```
7za x <soc file>
```

Soc firmware for different MCU may contains different files, it's heavily depend on the target device.

For example, the ESP32 soc files contains:
- `bootloader.bin`
- `partition-table.bin`
- `luatos.bin`

All these files can be programmbed by `esptool.py`.

But the AIR101/103 soc files contains:
- `AIR[101|103].fls`

And the `fls` file can be programmed by `wm_tool`.
