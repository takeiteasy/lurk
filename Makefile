ifeq ($(OS),Windows_NT)
	PROG_EXT=.exe
	CFLAGS=-O2 -DSOKOL_D3D11 -lkernel32 -luser32 -lshell32 -ldxgi -ld3d11 -lole32 -lgdi32
	ARCH=win32
	LIB_EXT=.dll
	CIMGUI_FLAGS=
	CIMGUI_BACKEND=
	SHDC_FLAGS=hlsl5
else
	UNAME:=$(shell uname -s)
	PROG_EXT=
	ifeq ($(UNAME),Darwin)
		CFLAGS=-x objective-c -DSOKOL_METAL -fobjc-arc -framework Metal -framework Cocoa -framework MetalKit -framework Quartz -framework AudioToolbox
		ARCH:=$(shell uname -m)
		LIB_EXT=.dylib
		CIMGUI_FLAGS=-x objective-c++ -fobjc-arc -framework Metal -framework Quartz
		CIMGUI_BACKEND=imgui_impl_metal.mm
		ifeq ($(ARCH),arm64)
			ARCH=osx_arm64
		else
			ARCH=osx
		endif
		SHDC_FLAGS=metal_macos
	else ifeq ($(UNAME),Linux)
		CFLAGS=-DSOKOL_GLCORE33 -pthread -lGL -ldl -lm -lX11 -lasound -lXi -lXcursor
		ARCH=linux
		SHDC_FLAGS=glsl330
		LIB_EXT=.so
		CIMGUI_FLAGS=
		CIMGUI_BACKEND=
	else
		$(error OS not supported by this Makefile)
	endif
endif

CC=clang
SOURCE=$(wildcard src/*.c)
NAME=sokol
EXTRA_CFLAGS=-DWEE_ENABLE_ARGUMENTS -DWEE_ENABLE_CONFIG
INCLUDE=-Ibuild -Ideps -Ideps/cimgui -Isrc

EXE=build/$(NAME)_$(ARCH)$(PROG_EXT)
ARCH_PATH=./tools/bin/$(ARCH)

SHDC_PATH=$(ARCH_PATH)/sokol-shdc$(PROG_EXT)
SHADERS=$(wildcard assets/*.glsl)
SHADER_OUTS=$(patsubst %,%.h,$(SHADERS))

all: app

.SECONDEXPANSION:
SHADER=$(patsubst %.h,%,$@)
SHADER_OUT=$@
%.glsl.h: $(SHADERS)
	$(SHDC_PATH) -i $(SHADER) -o $(SHADER_OUT) -l $(SHDC_FLAGS)

shaders: $(SHADER_OUTS)

cimgui:
	$(CC)++ -shared -fpic -std=c++14 -Ideps/cimgui -Ideps/cimgui/imgui $(CIMGUI_FLAGS) deps/cimgui/cimgui.cpp deps/cimgui/imgui/*.cpp deps/cimgui/imgui/backends/$(CIMGUI_BACKEND) -o build/libcimgui_$(ARCH)$(LIB_EXT)

app: shaders cimgui
	$(CC) $(INCLUDE) $(EXTRA_CFLAGS) $(CFLAGS) -Lbuild/ -lcimgui_$(ARCH) tests/test.c $(SOURCE) -o $(EXE)

tools:
	cd tools && make

.PHONY: all app shaders cimgui tools
