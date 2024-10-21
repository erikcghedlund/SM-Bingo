CC = gcc
CJSON = third_party/lib/cjson/cJSON.o
STD = --std=c18
DEBUG = -g -Wall

DIRECTORIES = debug third_party
LIBRARIES = debug/lib/json_conversion.o third_party/lib/rxi/log.o
EXECUTABLES = debug/bin/server debug/bin/gtkclient
HEADERS = third_party/include/cjson/cJSON.h
TARGETS = $(EXECUTABLES)

CJSON_VERSION = 1.7.15
CJSON_URL = https://github.com/DaveGamble/cJSON/archive/refs/tags/v$(CJSON_VERSION).tar.gz

RXILOG_VERSION = f9ea34994bd58ed342d2245cd4110bb5c6790153
RXILOG_URL = https://raw.githubusercontent.com/rxi/log.c/$(RXILOG_VERSION)/src/
RXILOG_BUILDARGS = -DLOG_USE_COLOR

GTK_FLAGS = -I/usr/include/gtk-4.0 -I/usr/include/pango-1.0 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/sysprof-4 -I/usr/include/harfbuzz -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/libmount -I/usr/include/blkid -I/usr/include/fribidi -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/graphene-1.0 -I/usr/lib/graphene-1.0/include -mfpmath=sse -msse -msse2 -pthread -lgtk-4 -lpangocairo-1.0 -lpango-1.0 -lharfbuzz -lgdk_pixbuf-2.0 -lcairo-gobject -lcairo -lgraphene-1.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0 

all:	$(TARGETS) clean_up_cJSON

clean:
	rm -rf $(DIRECTORIES)

debug/bin:
	mkdir $@ --parent

debug/lib:
	mkdir $@ --parent

debug/bin/server: debug/bin source/server.c $(LIBRARIES) $(CJSON)
	$(CC) -o $@ source/server.c $(LIBRARIES) $(CJSON) $(STD) $(DEBUG) -Ithird_party/include

debug/lib/log.o: debug/lib source/log.c
	$(CC) -c -o $@ source/log.c $(STD) $(DEBUG)

debug/lib/json_conversion.o: debug/lib source/json_conversion.c third_party/include/cjson/cJSON.h third_party/include/rxi/log.h
	$(CC) -c -o $@ source/json_conversion.c $(STD) $(DEBUG) -Ithird_party/include

third_party/lib/cjson:
	mkdir $@ --parent

third_party/include: third_party
	mkdir $@ --parent

third_party/source: third_party
	mkdir $@ --parent

third_party/lib: third_party
	mkdir $@ --parent

third_party:
	mkdir $@ --parent

third_party/cJSON.tar.gz: third_party
	wget $(CJSON_URL) --output-document $@ --no-clobber --quiet || :

third_party/source/cjson:
	mkdir $@ --parent

third_party/source/cjson/cJSON.c: third_party/cJSON.tar.gz third_party/source/cjson
	tar -xf  third_party/cJSON.tar.gz cJSON-$(CJSON_VERSION)/cJSON.c
	mv cJSON-$(CJSON_VERSION)/cJSON.c -t third_party/source/cjson 

third_party/include/cjson:
	mkdir $@ --parent

third_party/include/cjson/cJSON.h: third_party/cJSON.tar.gz third_party/include/cjson
	tar -xf  third_party/cJSON.tar.gz cJSON-$(CJSON_VERSION)/cJSON.h
	mv cJSON-$(CJSON_VERSION)/cJSON.h -t third_party/include/cjson

third_party/lib/cjson/cJSON.o: third_party/lib/cjson third_party/include/cjson/cJSON.h third_party/source/cjson/cJSON.c
	$(CC) -c -o $@ third_party/source/cjson/cJSON.c $(STD) $(DEBUG) -Ithird_party/include/cjson

clean_up_cJSON: third_party/lib/cjson/cJSON.o third_party/source/cjson/cJSON.c third_party/include/cjson/cJSON.h
	rm cJSON-$(CJSON_VERSION) -rf

third_party/source/rxi: third_party/source
	mkdir $@ --parent

third_party/include/rxi: third_party/include
	mkdir $@ --parent

third_party/lib/rxi: third_party/lib
	mkdir $@ --parent

third_party/source/rxi/log.c: third_party/source/rxi
	wget $(RXILOG_URL)/log.c --output-document $@ --no-clobber --quiet || :

third_party/include/rxi/log.h: third_party/include/rxi
	wget $(RXILOG_URL)/log.h --output-document $@ --no-clobber --quiet || :

third_party/lib/rxi/log.o: third_party/lib/rxi third_party/include/rxi/log.h third_party/source/rxi/log.c
	$(CC) -c -o $@ third_party/source/rxi/log.c $(STD) $(DEBUG) $(RXILOG_BUILDARGS) -Ithird_party/include/rxi

debug/bin/gtkclient: source/gtkclient.c
	$(CC) $< -o $@ $(GTK_FLAGS) $(DEBUG)
