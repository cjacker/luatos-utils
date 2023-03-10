DESTDIR=

LUA=lua-5.3.6

all:
	gcc -o luatos-mkscriptbin utils/mkscriptbin.c
	gcc -o luatos-wm_tool utils/wm_tool_mod_by_luatos.c
	rm -rf $(LUA)
	tar xf utils/$(LUA).tar.gz
	sed -i "s/^CC= gcc -std=gnu99/CC= gcc -m32 -std=gnu99/g" $(LUA)/src/Makefile
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
