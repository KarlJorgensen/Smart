#!/usr/bin/make -f

# These cannot be subdirectories of the src directory!
BUILD_PATH = /tmp/build
BUILD_CACHE = /tmp/cache

.PHONY: compile
compile: dirs
	arduino --upload \
		--pref build.path=$(BUILD_PATH) \
		--pref build.cache=$(BUILD_CACHE) \
		--pref update.check=false \
		$(wildcard *.ino)

.PHONY: upload
upload: dirs
	arduino --upload \
		--pref build.path=$(BUILD_PATH) \
		--pref build.cache=$(BUILD_CACHE) \
		--pref update.check=false \
		$(wildcard *.ino)

dirs: $(BUILD_PATH)/.stamp $(BUILD_CACHE)/.stamp

$(BUILD_PATH)/.stamp :
	mkdir --parents $(dir $@)
	touch $@

 $(BUILD_CACHE)/.stamp :
	mkdir --parents $(dir $@)
	touch $@
