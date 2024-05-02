OUT_PATH=build
SCENES_PATH=scenes
INCLUDE=-Ilurk -Ilurk/deps -I$(SCENES_PATH)
SOURCES=$(wildcard lurk/*.c) lurk/deps/gamepad/Gamepad_private.c

ifeq ($(OS),Windows_NT)
	PROG_EXT=.exe
	SOKOL_FLAGS=-O2 -DSOKOL_D3D11 -lkernel32 -luser32 -lshell32 -ldxgi -ld3d11 -lole32 -lgdi32
	ARCH=win32
	LIB_EXT=dll
	SHDC_FLAGS=hlsl5
	SOURCES:=$(SOURCES) lurk/deps/dlfcn_win32.c
else
	UNAME:=$(shell uname -s)
	PROG_EXT=
	ifeq ($(UNAME),Darwin)
		SOKOL_FLAGS=-x objective-c -DSOKOL_METAL -fno-objc-arc -framework CoreServices -framework CoreFoundation -lpthread -framework Metal -framework Cocoa -framework MetalKit -framework Quartz -framework IOKit
		ARCH:=$(shell uname -m)
		LIB_EXT=dylib
		ifeq ($(ARCH),arm64)
			ARCH=osx_arm64
		else
			ARCH=osx
		endif
		SHDC_FLAGS=metal_macos
		SOURCES:=$(SOURCES) lurk/deps/gamepad/Gamepad_macosx.c
	else ifeq ($(UNAME),Linux)
		SOKOL_FLAGS=-DSOKOL_GLCORE33 -pthread -lGL -ldl -lm -lX11 -lasound -lXi -lXcursor
		ARCH=linux
		SHDC_FLAGS=glsl330
		LIB_EXT=so
		SOURCES:=$(SOURCES) lurk/deps/gamepad/Gamepad_linux.c
	else
		$(error OS not supported by this Makefile)
	endif
endif

default: all

SCENES=$(wildcard $(SCENES_PATH)/*.c)
SCENES_OUT=$(patsubst $(SCENES_PATH)/%.c,$(OUT_PATH)/%.$(LIB_EXT), $(SCENES))

.SECONDEXPANSION:
SCENE=$(patsubst $(OUT_PATH)/%.$(LIB_EXT),$(SCENES_PATH)/%.c,$@)
SCENE_OUT=$@
%.$(LIB_EXT): $(SCENES)
	$(CC) -shared -fpic $(INCLUDE) -DSOKOL_NO_ENTRY -DLURK_SCENE -fenable-matrix $(SOKOL_FLAGS) $(SCENE) $(SOURCES) -o $(SCENE_OUT)

$(OUT_PATH):
	mkdir $(OUT_PATH)

scenes: $(OUT_PATH) $(SCENES_OUT)

program: $(OUT_PATH)
	$(CC) $(INCLUDE) -g -fenable-matrix $(SOKOL_FLAGS) $(SOURCES) -o $(OUT_PATH)/lurk_$(ARCH)$(PROG_EXT)

clean:
	rm -rf $(OUT_PATH)/ || yes

all: clean scenes program

.PHONY: default all program scenes clean
