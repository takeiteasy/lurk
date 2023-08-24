ifeq ($(OS),Windows_NT)
	PROG_EXT=.exe
	SOKOL_FLAGS=-O2 -DSOKOL_D3D11 -lkernel32 -luser32 -lshell32 -ldxgi -ld3d11 -lole32 -lgdi32
	ARCH=win32
	LIB_EXT=dll
	SHDC_FLAGS=hlsl5
else
	UNAME:=$(shell uname -s)
	PROG_EXT=
	ifeq ($(UNAME),Darwin)
		SOKOL_FLAGS=-x objective-c -DSOKOL_METAL -fobjc-arc -framework Metal -framework Cocoa -framework MetalKit -framework Quartz -framework AudioToolbox
		ARCH:=$(shell uname -m)
		LIB_EXT=dylib
		ifeq ($(ARCH),arm64)
			ARCH=osx_arm64
		else
			ARCH=osx
		endif
		SHDC_FLAGS=metal_macos
	else ifeq ($(UNAME),Linux)
		SOKOL_FLAGS=-DSOKOL_GLCORE33 -pthread -lGL -ldl -lm -lX11 -lasound -lXi -lXcursor
		ARCH=linux
		SHDC_FLAGS=glsl330
		LIB_EXT=so
	else
		$(error OS not supported by this Makefile)
	endif
endif

INCLUDE=-Ibuild -Ideps -Igame -I.
SOURCES=$(wildcard src/*.c)
ARCH_PATH=./tools/$(ARCH)

SHDC_PATH=$(ARCH_PATH)/sokol-shdc$(PROG_EXT)
SHADERS=$(wildcard assets/*.glsl)
SHADER_OUTS=$(patsubst %,%.h,$(SHADERS))

.SECONDEXPANSION:
SHADER=$(patsubst %.h,%,$@)
SHADER_OUT=$@
%.glsl.h: $(SHADERS)
	$(SHDC_PATH) -i $(SHADER) -o $(SHADER_OUT) -l $(SHDC_FLAGS)

shaders: $(SHADER_OUTS)
	mv assets/*.h build/

SCENES_PATH=game
SCENES=$(wildcard $(SCENES_PATH)/*.c)
SCENES_OUT=$(patsubst $(SCENES_PATH)/%.c,build/%.$(LIB_EXT), $(SCENES))

.SECONDEXPANSION:
SCENE=$(patsubst build/%.$(LIB_EXT),$(SCENES_PATH)/%.c,$@)
SCENE_OUT=$@
%.$(LIB_EXT): $(SCENES)
	$(CC) -shared -fpic $(INCLUDE) -DWEE_STATE -fenable-matrix $(SCENE) $(SOURCES) -o $(SCENE_OUT)

scenes: $(SCENES_OUT)

debug:
	$(CC) $(INCLUDE) -g -DWEE_DEBUG -fenable-matrix $(SOKOL_FLAGS) $(SOURCES) -o build/wee_$(ARCH)$(PROG_EXT)

release:
		$(CC) $(INCLUDE) -O3 -Wall -Werror -fenable-matrix $(SOKOL_FLAGS) $(SOURCES) -o build/wee_$(ARCH)$(PROG_EXT)

all: shaders scenes debug

default: scenes

.PHONY: all debug release shaders scenes
