CC = gcc
CJSON = third_party/lib/cjson/cJSON.o
STD = --std=c18
DEBUG = -g -Wall

DIRECTORIES = debug third_party
LIBRARIES = debug/lib/log.o debug/lib/json_conversion.o third_party/lib/rxi/log.o
EXECUTABLES = debug/bin/server
HEADERS = third_party/include/cjson/cJSON.h
TARGETS = $(EXECUTABLES)

CJSON_VERSION = 1.7.15
CJSON_URL = https://github.com/DaveGamble/cJSON/archive/refs/tags/v$(CJSON_VERSION).tar.gz

RXILOG_VERSION = f9ea34994bd58ed342d2245cd4110bb5c6790153
RXILOG_URL = https://raw.githubusercontent.com/rxi/log.c/$(RXILOG_VERSION)/src/

all:	$(TARGETS)

clean:
	rm -rf $(DIRECTORIES)

debug/bin:
	mkdir $@ --parent

debug/lib:
	mkdir $@ --parent

debug/bin/server: debug/bin source/server.c $(LIBRARIES) $(CJSON)
	$(CC) -o $@ source/*.c $(CJSON) $(STD) $(DEBUG) -Ithird_party/include

debug/lib/log.o: debug/lib source/log.c
	$(CC) -c -o $@ source/log.c $(STD) $(DEBUG)

debug/lib/json_conversion.o: debug/lib source/json_conversion.c
	$(CC) -c -o $@ source/log.c $(STD) $(DEBUG)

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

third_party/source/cjson/cJSON.c: third_party/cJSON.tar.gz third_party/source
	tar -xf  third_party/cJSON.tar.gz cJSON-$(CJSON_VERSION)/cJSON.c
	mv cJSON-$(CJSON_VERSION)/ -T third_party/source/cjson 2> /dev/null || rm cJSON-$(CJSON_VERSION)/ -rf

third_party/include/cjson/cJSON.h: third_party/cJSON.tar.gz third_party/include
	tar -xf  third_party/cJSON.tar.gz cJSON-$(CJSON_VERSION)/cJSON.h
	mv cJSON-$(CJSON_VERSION)/ -T third_party/include/cjson 2> /dev/null || rm cJSON-$(CJSON_VERSION)/ -rf

third_party/lib/cjson/cJSON.o: third_party/lib/cjson third_party/include/cjson/cJSON.h third_party/source/cjson/cJSON.c
	$(CC) -c -o $@ third_party/source/cjson/cJSON.c $(STD) $(DEBUG) -Ithird_party/include/cjson

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
	$(CC) -c -o $@ third_party/source/rxi/log.c $(STD) $(DEBUG) -Ithird_party/include/rxi
