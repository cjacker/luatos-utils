LUA=lua-5.3.6

all:
	gcc -o mkscriptbin utils/mkscriptbin.c
	gcc -o wm_tool_luatos utils/wm_tool_mod_by_luatos.c
	rm -rf $(LUA)
	tar xf utils/$(LUA).tar.gz
	sed -i "s/^CC= gcc -std=gnu99/CC= gcc -m32 -std=gnu99/g" $(LUA)/src/Makefile
	sed -i "s/^CFLAGS= -O2 -Wall -Wextra -DLUA_COMPAT_5_2/CFLAGS= -O2 -Wall -Wextra -DLUA_32BITS/g" $(LUA)/src/Makefile
	make -C $(LUA) linux && cp $(LUA)/src/luac . && rm -rf $(LUA) 
clean:
	rm -f mkscriptbin wm_tool_luatos luac
	rm -f script.bin script.img script-*.img
	rm -rf disk
