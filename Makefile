#!/usr/bin/make -f

# These cannot be subdirectories of the src directory!
BUILD_PATH = /tmp/build
BUILD_CACHE = /tmp/cache

LINE_HEADERS = hour_hand-lines.h minute_hand-lines.h
PIXMAP_HEADERS = watchface-pixmap.h

HEADERS = $(LINE_HEADERS) $(PIXMAP_HEADERS) boxes.h
IMAGES = $(PIXMAP_HEADERS:%.h=%.xbm)  $(PIXMAP_HEADERS:%.h=%.png)

SERIAL_PORT=/dev/ttyACM0

# SERIAL_RATE should match SERIAL_SPEED in settings.h
SERIAL_RATE=921600

.PHONY: verify
verify: dirs $(HEADERS)
	arduino --verify \
		--pref build.path=$(BUILD_PATH) \
		--pref build.cache=$(BUILD_CACHE) \
		--pref update.check=false \
		$(wildcard *.ino)

.PHONY: debug
debug : upload
	stty --file=$(SERIAL_PORT) $(SERIAL_RATE)
	tail --follow $(SERIAL_PORT)

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

boxes.h : watchface-pixmap.svg svg2boxes.py
	rm -f $@
	./svg2boxes.py $< > $@.new
	chmod -w $@.new
	mv -f $@.new $@

.PHONY: images
images: $(IMAGES)

# Convenience rules for generating HTML versions of the Markdown
# files. The HTML files are not published, but are useful for spotting
# formatting errors and the like.
#
.PHONY: readmes

readmes : README.html README-development.html

%.html : %.md
	rm -f $@
	markdown $< > $@.new
	chmod -w $@.new
	mv -f $@.new $@
