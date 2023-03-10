DESTDIR=

LUA=lua-5.3.6

all:
	gcc -o luatos-mkscriptbin utils/mkscriptbin.c
	gcc -o luatos-wm_tool utils/wm_tool_mod_by_luatos.c
	rm -rf $(LUA)
	tar xf utils/$(LUA).tar.gz
	# 32bit ELF: 1b 4c 75 61 53 00 19 93   0d 0a 1a 0a 04 04 04 04
	# 64bit ELF: 1b 4c 75 61 53 00 19 93   0d 0a 1a 0a 04 08 04 04
	# The only difference built luac as 32bit or 64bit is 0x04 and 0x08,
	# it's the sizeof(size_t) cause this different.
	sed -i "s/DumpByte(sizeof(size_t), D)/DumpByte(sizeof(int), D)/g" $(LUA)/src/ldump.c
	sed -i "s/^CFLAGS= -O2 -Wall -Wextra -DLUA_COMPAT_5_2/CFLAGS= -O2 -Wall -Wextra -DLUA_32BITS/g" $(LUA)/src/Makefile
	make -C $(LUA) linux && cp $(LUA)/src/luac ./luatos-luac && rm -rf $(LUA)

install: all
	mkdir -p $(DESTDIR)/usr/bin
	install -m0755 luatos-luac $(DESTDIR)/usr/bin
	install -m0755 luatos-wm_tool $(DESTDIR)/usr/bin
	install -m0755 luatos-mkscriptbin $(DESTDIR)/usr/bin
	install -m0755 luatos-gen-script-img $(DESTDIR)/usr/bin
	install -m0755 luatos-flash-script-img $(DESTDIR)/usr/bin
	install -m0755 luatos-flash-base-firmware $(DESTDIR)/usr/bin
clean:
	rm -f luatos-mkscriptbin luatos-wm_tool  luatos-luac
	rm -f script.bin script.img script-*.img
	rm -rf disk
