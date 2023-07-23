# sim

**sim** or "_Sokol Immediate Mode_" is a abstraction layer on top of [sokol](https://github.com/floooh/sokol/) that mimics the stack based immediate mode style of OpenGL 1.x. The idea is inspired by [raylib's](https://github.com/raysan5/raylib) [rlgl](https://github.com/raysan5/raylib/blob/master/src/rlgl.h) module and sokol's [own immediate mode API](https://github.com/floooh/sokol/blob/master/util/sokol_gl.h). **WIP** -- see [todo](#todo) section.

## TODO

- [ ] Error handling/reporting
- [ ] Add documentation
- [X] Window management
- [X] Matrix stack
- [X] Command queue
- [X] Framebuffers
- [X] Shaders
- [X] Pipelines
- [ ] Verticies + buffers
- [ ] Texture handling
- [ ] Draw calls
- [ ] Event handling
- [ ] Split `frame` callback into `unfixed` and `fixed`
- [ ] OpenAL style `sokol_audio.h` wrapper

## LICENSE
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
