#!/usr/bin/make -f

# These cannot be subdirectories of the src directory!
BUILD_PATH = /tmp/build
BUILD_CACHE = /tmp/cache

LINE_HEADERS = hour_hand-lines.h minute_hand-lines.h
PIXMAP_HEADERS = watchface-pixmap.h

HEADERS = $(LINE_HEADERS) $(PIXMAP_HEADERS)
IMAGES = $(PIXMAP_HEADERS:%.h=%.xbm)  $(PIXMAP_HEADERS:%.h=%.png)

.PHONY: upload
upload: dirs $(HEADERS)
	arduino --upload \
		--pref build.path=$(BUILD_PATH) \
		--pref build.cache=$(BUILD_CACHE) \
		--pref update.check=false \
		$(wildcard *.ino)

.PHONY: dirs
dirs: $(BUILD_PATH)/.stamp $(BUILD_CACHE)/.stamp

$(BUILD_PATH)/.stamp :
	mkdir --parents $(dir $@)
	touch $@

$(BUILD_CACHE)/.stamp :
	mkdir --parents $(dir $@)
	touch $@

.PHONY: headers
headers : $(HEADERS)

%-lines.h : %-lines.svg svg2multiline.py
	rm -f $@.new
	./svg2multiline.py $< > $@.new
	chmod -w $@.new
	mv --force $@.new $@

%-pixmap.png : %-pixmap.svg
	rm -f $@
	inkscape --export-filename=$@ --export-area-drawing --export-width=200 --export-height=200 --export-margin=0 $<
	chmod -w $@

.PRECIOUS: %-pixmap.xbm %-pixmap.png
%-pixmap.xbm : %-pixmap.png Makefile
	rm -f $@
	pngtopam -background=white -mix < $< | \
		pamthreshold | pamtopnm | pbmtoxbm \
		 > $@.new
	chmod -w $@.new
	mv -f $@.new $@

%-pixmap.h : %-pixmap.xbm Makefile
	rm -f $@
	sed < $< -e 's/noname/$*/g' -e 's/static char/static PROGMEM const uint8_t/' > $@.new
	chmod -w $@.new
	mv -f $@.new $@

.PHONY: images
images: $(IMAGES)
