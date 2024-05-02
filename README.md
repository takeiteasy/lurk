# lurk

> [!WARNING]
> Work in progress

_**L**ive **U**pdating **R**endering **K**it_ is a 2D rendering framework designed for the rapid development and prototyping of games and animation. The goal of the project is a high-level API, focused on development speed and ease. While performance is important for all applications (obviously), that is not the main aim for this project.

To achieve this, the framework is built around hot-reloading; meaning code changes and asset modifications can be updated in real time - without having to rebuild and relaunch the entire game.

## Preview

<p align="center">
   <img src="https://raw.githubusercontent.com/takeiteasy/lurk/master/demo.gif">
</p>

## Features

- [X] Cross-platform (windows, linux and macos)
- [X] Simple rendering API (wrapped over sokol+sokol_gp)
- [X] Live coding environment (Changes to scenes can be reloaded without closing)
- [X] Automatic config importing/exporting (Including command line arguments)
- [ ] Hot-reload asset
- [ ] Font loading+rendering
- [ ] Input handling
  - [X] Joystick support
  - [ ] Input mapping
- [X] Integrated ECS (ezecs.h)
- [ ] Rigid-body physics (contained inside ECS)
- [ ] Animations+Events (Inspired by Apple's SpriteKit `SKAction`)
- [ ] Assets
  - [X] Images (wrapper over stb_image+stb_image_write.h+qoi.h)
    - Importing: jpeg, png, tng, bmp, psd, gif, hdr, pic, pnm, qoi
    - Exporting: png, bmp, tga, jpg, hdr, qoi
  - [ ] Fonts
    - TBD (likely ttf only, using stb_truetype.h)
- [X] Extras (see [here](https://github.com/takeiteasy/ez) for more information)
  - Image manipliation library (ezimage.h)
  - Fairly comprehensive math + linear algebra library (ezmath.h)
  - Pseudo-random number generator (ezrng.h)
  - Basic cross-platform file-system functions (ezfs.h)

## Getting Started

**NOTE**: As this project is very early in development, all this information is subject to minor or major changes and isn't permenant.

To get started, first either clone or fork this repository.

```
git clone https://github.com/takeiteasy/lurk.git
```

The layout of the project is very simple.

```
├── build/ (where your executables live)
├── lurk/ (lurk source + third party dependencies)
├── scenes/ (where your project lives)
│   ├── assets/ (where your project's assets live)
│   └── config.h (project conifg **required**)
└── tools/ (internal lurk tools)
```

You are likely only interested in the `scenes` folder. This is where your project and its assets will live. **NOTE**: The name of this folder can be easily changed by modifying the Makefile. Your projects assets will live in ```scenes/assets/```.

Now you will want to create a `scene`. Scenes are all seperate .c files and are all built into dynamic libraries which are loaded and reloaded at runtime. This is how the live coding is acheived.[†](https://github.com/skeeto/interactive-c-demo) Check out ```scenes/example.c``` for a boilerplate example. **NOTE**: Scenes must be added to ```LURK_SCENES``` macro inside ```config.h```.

Scenes have a number of optional callback/events:

- ```init```        -- Called once when scene is loaded (**required**)
- ```deinit```      -- Called once when scene is unloaded
- ```unload```      -- Called when code is modified (before new changes are loaded)
- ```reload```      -- Called when code has been modified (after new changes are loaded)
- ```event```       -- Called when window event has triggered
- ```update```      -- Called every frame (variable tick rate)
- ```fixedupdate``` -- Called every frame (fixed tick rate, set ```DEFAULT_TARGET_FPS``` macro)
- ```preframe```    -- Called at the beginning of every frame
- ```frame```       -- Called every frame (rendering should be done here) (**required**)
- ```postframe```   -- Called at the end of each frame

Every scene must declare a map for each of the callback functions. This map should be a ```lurkScene``` with the name ```scene``` like as seen below.

```
EXPORT const lurkScene scene = {
    .init = init,
    .deinit = deinit,
    .reload = reload,
    .unload = unload,
    .event = event,
    .frame = frame
};
```

Now that you have a scene and added it to your ```config.h``` file you can build. Building is handled with a simple Makefile. Running ```make all``` will build the base executable, all the scenes and cook the assets. Once this is built your executable will be located inside ```build/```.

Run the executable in a second terminal (or run it forked). Now you can modify your scene and run ```make scenes``` to rebuild. Assuming there is no compilation errors your codes should be instantly updated in the still running application.

**NOTE**: This should hopefully be enough to get you started. There is a lot not covered, but I plan to update this as much as possible. Also, there is no documentation yet, however that won't be the case forever.

## Dependencies

- [floooh/sokol](https://github.com/floooh/sokol) (zlib/libpng)
    - sokol_gfx.h
    - sokol_app.h
    - sokol_glue.h
    - sokol_args.h
    - sokol_time.h
- [edubart/sokol_gp](https://github.com/edubart/sokol_gp) (MIT)
    - sokol_gp.h
- [esr/microjson](https://gitlab.com/esr/microjson/) (BSD-2-clause)
    - mjson.h
- [tsoding/jim](https://github.com/tsoding/jim) (MIT)
    - jim.h
- [billziss-gh/imap](https://github.com/billziss-gh/imap) (MIT)
    - imap.h
- [nothings/stb](https://github.com/nothings/stb) (MIT/Public Domain)
    - stb_image.h
    - stb_image_write.h
- [phoboslab/qoi](https://github.com/phoboslab/qoi) (MIT)
    - qoi.h
- [dlfcn-win32/dlfcn-win32](https://github.com/dlfcn-win32/dlfcn-win32) (MIT)
    - dlfcn_win32.h
    - dlfcn_win32.c
- [septag/dmon](https://github.com/septag/dmon) (BSD-2-Clause)
    - dmon.h
- [win32ports/dirent_h](https://github.com/win32ports/dirent_h) (MIT)
    - dirent_win32.h
- [ThemsAllTook/libstem_gamepad](https://github.com/ThemsAllTook/libstem_gamepad) (zlib/libpng)
    - Gamepad.h
    - Gamepad_macosx.c
    - Gamepad_private.h
    - Gamepad_windows_mm.c
    - Gamepad_linux.c
    - Gamepad_private.c
    - Gamepad_windows_dinput.c

## License
```
The MIT License (MIT)

Copyright (c) 2023 George Watson

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```
