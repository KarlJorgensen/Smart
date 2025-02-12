#!/usr/bin/make -f

# These cannot be subdirectories of the src directory!
BUILD_PATH = /tmp/build
BUILD_CACHE = /tmp/cache

SVG_HEADERS = hour_hand.h minute_hand.h

.PHONY: compile
compile: dirs $(SVG_HEADERS)
	arduino --upload \
		--pref build.path=$(BUILD_PATH) \
		--pref build.cache=$(BUILD_CACHE) \
		--pref update.check=false \
		$(wildcard *.ino)

.PHONY: upload
upload: dirs $(SVG_HEADERS)
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

%.h : %.svg svg2multiline.py
	rm -f $@.new
	./svg2multiline.py $< > $@.new
	chmod -w $@.new
	mv --force $@.new $@

.PHONY: headers
headers : $(SVG_HEADERS)
