=begin
gl.rb - Generate GL loader implemetation

The MIT License (MIT)

Copyright (c) 2022 George Watson

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
=end

require "nokogiri"
require "net/http"

PRINT_TO_STDOUT=true
PRINT_SINGLE_FILE=true

# Download required files if needed
xml = ["https://registry.khronos.org/EGL/api/KHR/khrplatform.h",
       "https://raw.githubusercontent.com/KhronosGroup/OpenGL-Registry/main/xml/gl.xml"]
xml.each do |x|
    xp = File.basename(x)
    next if File.exist? xp
    resp = Net::HTTP.get_response URI(x)
    abort "ERROR: #{resp.code}" unless resp.is_a? Net::HTTPSuccess
    File.write xp, resp.body
end

# Parse gl.xml with Nokogiri
doc = Nokogiri::XML File.open("tools/gl.xml") do |config|
  config.options = Nokogiri::XML::ParseOptions::STRICT | Nokogiri::XML::ParseOptions::NOBLANKS
end

# Parse <types>
types = doc.xpath("//types").children.reject { |c| c.class.to_s == "Nokogiri::XML::Comment" }.map do |t|
    html = t.inner_html.gsub "<apientry></apientry>", "APIENTRY"
    /<name>(?<name>.*)<\/name>/ =~ html
    [name, t.text]
end.to_h.delete_if { |k, v| k.nil? }
# Parse <enums>
enums = doc.xpath("//enums").children.map { |e| [e.attr('name'), e.attr('value')] }.to_h
# Parse <commands>
commands = doc.xpath("//commands").children.map do |f|
    parts = {:params => []}
    name = nil
    f.children.each do |ff|
        case ff.to_s
        when /^<proto/
            fff = ff.children.to_a
            name = fff.pop.text
            parts[:result] = fff.map { |ffff| ffff.text.strip }.join " "
        when /^<param/
            parts[:params] << ff.children.to_a.map { |ffff| ffff.text.strip }.join(" ")
        end
    end
    parts[:params] = ["void"] if parts[:params].empty?
    [name, parts]
end.to_h

# Print to file unless PRINT_TO_STDOUT=true
$stdout.reopen("gl.h", "w") unless PRINT_TO_STDOUT

puts <<HEADER
/* gl.h -- https://github.com/takeiteasy/wee

 Inspired by https://github.com/tsherif/simple-opengl-loader + Dav1dde/glad
 Function address loading functions taken from https://github.com/Dav1dde/glad
 
 The MIT License (MIT)
 Copyright (c) 2022 George Watson
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
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */
 
#define __gl_glcorearb_h_ 1  /* Khronos core */
#define __gl_glext_h_ 1      /* Khronos compatibility */
#define __gl_h_ 1            /* Windows/Linux */
#define __GL_H__ 1           /* Windows */
#define __gl3_h_ 1           /* Mac */

#ifndef opengl_loader_h
#define opengl_loader_h
#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) && !defined(APIENTRY) && !defined(__CYGWIN__) && !defined(__SCITECH_SNAP__)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#define APIENTRY __stdcall
#endif

#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif
#ifndef GLAPI
#if defined(GLAD_GLAPI_EXPORT)
#if defined(_WIN32) || defined(__CYGWIN__)
#if defined(GLAD_GLAPI_EXPORT_BUILD)
#if defined(__GNUC__)
#define GLAPI __attribute__ ((dllexport)) extern
#else
#define GLAPI __declspec(dllexport) extern
#endif
#else
#if defined(__GNUC__)
#define GLAPI __attribute__ ((dllimport)) extern
#else
#define GLAPI __declspec(dllimport) extern
#endif
#endif
#elif defined(__GNUC__) && defined(GLAD_GLAPI_EXPORT_BUILD)
#define GLAPI __attribute__ ((visibility ("default"))) extern
#else
#define GLAPI extern
#endif
#else
#define GLAPI extern
#endif
#endif

typedef void* (*glLoadProc)(const char *name);
GLAPI int glLoadFunctions(void);
GLAPI int glLoadGLLoader(glLoadProc);

#if !defined(GL_LOAD_VERSION)
#define GL_LOAD_VERSION 1000
#endif
HEADER

# Find <feature>, remove any unused gl versions
features = doc.xpath("//feature").select { |f| f.attr('name') =~ /^GL_VERSION_\d_\d$/ }

# Print out version macros
features.each do |f|
    maj, min = f.attr('number').split('.')
    puts "#define #{f.attr 'name'} #{maj}0#{min}0"
end

# Parse and print khrplatform.h
khr = File.readlines("tools/khrplatform.h").map { |l| l.rstrip }[4..-2]
puts "\n/* khrplatform.h -- [https://registry.khronos.org/EGL/api/KHR/khrplatform.h]", khr.join("\n"), "/* end of khrplatform.h */", ""

# Define GL_ALL_EXTENSIONS to enable loading all extensions
extensions = {}
puts "#if defined(GL_ALL_EXTENSIONS)"
doc.xpath("//extensions").each do |e|
    e.children.each do |ee|
        next if ee.children.length == 0
        name = ee.attr("name")
        next unless ee.attr("supported").split("|").include? "glcore"
        extensions[name] = []
        puts "#define GL_EXT_#{name[3..]}"
    end
end
puts "#endif // GL_ALL_EXTENSIONS", ""

# Format features for each gl version
defined = []
functions = {}
features.each do |f|
    puts "#if GL_LOAD_VERSION >= #{f.attr 'name'}"
    ver = f.attr 'number'
    functions[ver] = []
    
    f.children.each do |ff|
        ff.children.each do |fff|
            name = fff.attr("name")
            if fff.to_s =~ /^<command/
                commands[name][:params].each do |p|
                    t = p.split(" ").select! { |pp| pp =~ /GL/ }
                    unless t.nil?
                        t.each do |tt|
                            unless defined.include? tt
                                puts types[tt]
                                defined << tt
                            end
                        end
                    end
                end
            end
        end
    end
    
    f.children.each do |ff|
        ff.children.each do |fff|
            name = fff.attr("name")
            next if defined.include? name
            case fff.to_s
            when /^<type/
                puts types[name]
            when /^<enum/
                puts "#define #{name} #{enums[name]}"
            when /^<command/
                proc = "PFN#{name.upcase}PROC"
                puts "typedef #{commands[name][:result]} (APIENTRYP #{proc})(#{commands[name][:params].join ', '});"
                puts "#define #{name} gll_#{name}"
                functions[ver].append [proc, name]
            end
            defined << name
        end
    end
    puts "#endif"
    puts
end

# Store functions in macros for later
functions.each do |k, v|
    puts "", "#define GL_FUNCTIONS_#{k.gsub '.', '_'} \\"
    v.each do |vv|
        puts "\tX(#{vv[0]}, #{vv[1]}) \\"
    end
end

# Define functions externs
puts "", "#define X(T, N) extern T gll_##N;"
functions.each do |k, v|
    maj, min = k.split '.'
    puts "#if GL_LOAD_VERSION >= GL_VERSION_#{maj}_#{min}"
    puts "GL_FUNCTIONS_#{maj}_#{min}"
    puts "#endif"
end
puts "#undef X", ""

# Format extensions
doc.xpath("//extensions").each do |e|
    e.children.each do |ee|
        next if ee.children.length == 0
        name = ee.attr("name")
        next unless ee.attr("supported").split("|").include? "glcore"
        ext = "GL_EXT_#{name[3..]}"
        puts "#if defined(#{ext})"
        ee.children.each do |eee|
            eee.children.each do |eeee|
                fn = eeee.attr("name")
                case eeee.to_s
                when /^<enum/
                    puts "#define #{fn} #{enums[fn]}" unless defined.include? fn
                when /^<command/
                    proc = "PFN#{fn.upcase}PROC"
                    puts "typedef #{commands[fn][:result]} (APIENTRYP #{proc})(#{commands[fn][:params].join ', '});"
                    puts "#define #{fn} gll_#{fn}"
                    extensions[name].append [proc, fn]
                when /^<type/
                    puts types[fn]
                end
                defined << fn
            end
        end
        puts "#endif // #{ext}", ""
    end
end
extensions

# Store extension functions in macros for later
extensions.each do |k, v|
    puts "#define #{k}_FUNCTIONS \\"
    v.each do |vv|
        puts "\tX(#{vv[0]}, #{vv[1]}) \\"
    end
    puts
end

# Define extension function externs
puts "#define X(T, N) extern T gll_##N;"
extensions.each do |k, v|
    puts "#if defined(GL_EXT_#{k[3..]})"
    puts "#{k}_FUNCTIONS"
    puts "#endif"
end
puts "#undef X"

puts <<FOOTER

#ifdef __cplusplus
}
#endif
#endif // opengl_loader_h
FOOTER

# Print to file unless PRINT_TO_STDOUT=true
# Print to .h if PRINT_SINGLE_FILE=true, .c if false
unless PRINT_TO_STDOUT
    $stdout.reopen("gl.c", "w") unless PRINT_SINGLE_FILE
end

if PRINT_SINGLE_FILE
    puts "", "#if defined(OPENGL_LOADER_IMPLEMENTATION)"
else
    puts <<CHEADER
/* gl.c -- https://github.com/takeiteasy/wee

 Inspired by https://github.com/tsherif/simple-opengl-loader + Dav1dde/glad
 Function address loading functions taken from https://github.com/Dav1dde/glad
 
 The MIT License (MIT)
 Copyright (c) 2022 George Watson
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
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include "glLoader.h"
CHEADER
end

# Assign function + extension function pointers to NULL
puts "", "#define X(T, N) T gll_##N = (T)((void *)0);"
functions.each do |k, v|
    maj, min = k.split '.'
    puts "#if GL_LOAD_VERSION >= GL_VERSION_#{maj}_#{min}"
    puts "GL_FUNCTIONS_#{maj}_#{min}"
    puts "#endif"
end
puts
extensions.each do |k, v|
    puts "#if defined(GL_EXT_#{k[3..]})"
    puts "#{k}_FUNCTIONS"
    puts "int EXT_#{k}_loaded = 0;"
    puts "#endif"
end
puts "#undef X", ""

puts <<LOADER
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32) || defined(__CYGWIN__)
#ifndef _WINDOWS_
#undef APIENTRY
#endif
#include <windows.h>
static HMODULE libGL;

typedef void* (APIENTRYP PFNWGLGETPROCADDRESSPROC_PRIVATE)(const char*);
static PFNWGLGETPROCADDRESSPROC_PRIVATE glLoaderGetProcAddressPtr;

#ifdef _MSC_VER
#ifdef __has_include
#if __has_include(<winapifamily.h>)
#define HAVE_WINAPIFAMILY 1
#endif
#elif _MSC_VER >= 1700 && !_USING_V110_SDK71_
#define HAVE_WINAPIFAMILY 1
#endif
#endif

#ifdef HAVE_WINAPIFAMILY
#include <winapifamily.h>
#if !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP) && WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)
#define IS_UWP 1
#endif
#endif

static int OpenGLLibrary(void) {
#ifndef IS_UWP
    if ((libGL = LoadLibraryW(L"opengl32.dll"))) {
        void(*tmp)(void);
        tmp = (void(*)(void)) GetProcAddress(libGL, "wglGetProcAddress");
        glLoaderGetProcAddressPtr = (PFNWGLGETPROCADDRESSPROC_PRIVATE)tmp;
        return glLoaderGetProcAddressPtr != NULL;
    }
#endif
    return 0;
}

static void CloseGLLibrary(void) {
    if (libGL) {
        FreeLibrary((HMODULE)libGL);
        libGL = NULL;
    }
}
#else
#include <dlfcn.h>
static void* libGL;

#if !defined(__APPLE__) && !defined(__HAIKU__)
typedef void* (APIENTRYP PFNGLXGETPROCADDRESSPROC_PRIVATE)(const char*);
static PFNGLXGETPROCADDRESSPROC_PRIVATE glLoaderGetProcAddressPtr;
#endif

static int OpenGLLibrary(void) {
#ifdef __APPLE__
    static const char *NAMES[] = {
        "../Frameworks/OpenGL.framework/OpenGL",
        "/Library/Frameworks/OpenGL.framework/OpenGL",
        "/System/Library/Frameworks/OpenGL.framework/OpenGL",
        "/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL"
    };
#else
    static const char *NAMES[] = {"libGL.so.1", "libGL.so"};
#endif

    unsigned int index = 0;
    for (index = 0; index < (sizeof(NAMES) / sizeof(NAMES[0])); index++) {
        if ((libGL = dlopen(NAMES[index], RTLD_NOW | RTLD_GLOBAL))) {
#if defined(__APPLE__) || defined(__HAIKU__)
            return 1;
#else
            glLoaderGetProcAddressPtr = (PFNGLXGETPROCADDRESSPROC_PRIVATE)dlsym(libGL,
                "glXGetProcAddressARB");
            return glLoaderGetProcAddressPtr != NULL;
#endif
        }
    }

    return 0;
}

static void CloseGLLibrary(void) {
    if (libGL) {
        dlclose(libGL);
        libGL = NULL;
    }
}
#endif

static void* GetGLProc(const char *namez) {
    void* result = NULL;
    if (libGL == NULL)
        return NULL;

#if !defined(__APPLE__) && !defined(__HAIKU__)
    if (glLoaderGetProcAddressPtr)
        result = glLoaderGetProcAddressPtr(namez);
#endif
    if (!result) {
#if defined(_WIN32) || defined(__CYGWIN__)
        result = (void*)GetProcAddress((HMODULE)libGL, namez);
#else
        result = dlsym(libGL, namez);
#endif
    }

    return result;
}

int glLoadFunctions(void) {
    int status = 0;

    if (OpenGLLibrary()) {
        status = glLoadGLLoader(&GetGLProc);
        CloseGLLibrary();
    }

    return status;
}

static int max_loaded_major = 1;
static int max_loaded_minor = 0;
static const char *exts = NULL;
static int num_exts_i = 0;
static char **exts_i = NULL;

static int GetExtensions(void) {
#if GL_LOAD_VERSION >= GL_VERSION_3_0
    if (max_loaded_major < 3) {
#endif
        exts = (const char*)glGetString(GL_EXTENSIONS);
#if GL_LOAD_VERSION >= GL_VERSION_3_0
    } else {
        unsigned int index;

        num_exts_i = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &num_exts_i);
        if (num_exts_i > 0)
            exts_i = malloc((size_t)num_exts_i * (sizeof *exts_i));

        if (exts_i == NULL) {
            return 0;
        }

        for (index = 0; index < (unsigned)num_exts_i; index++) {
            const char *gl_str_tmp = (const char*)glGetStringi(GL_EXTENSIONS, index);
            size_t len = strlen(gl_str_tmp);

            char *local_str = malloc((len+1) * sizeof(char));
            if (local_str != NULL)
                memcpy(local_str, gl_str_tmp, (len+1) * sizeof(char));
            exts_i[index] = local_str;
        }
    }
#endif
    return 1;
}

static void FreeExtensions(void) {
    if (exts_i) {
        int index;
        for (index = 0; index < num_exts_i; index++)
            free((char *)exts_i[index]);
        free((void *)exts_i);
        exts_i = NULL;
    }
}

static int FindExtension(const char *ext) {
#if GL_LOAD_VERSION >= GL_VERSION_3_0
    if (max_loaded_major < 3) {
#endif
        const char *extensions;
        const char *loc;
        const char *terminator;
        extensions = exts;
        if (!extensions || !ext)
            return 0;

        while(1) {
            if (!(loc = strstr(extensions, ext)))
                return 0;

            terminator = loc + strlen(ext);
            if ((loc == extensions || *(loc - 1) == ' ') && (*terminator == ' ' || *terminator == '\\0'))
                return 1;
            extensions = terminator;
        }
#if GL_LOAD_VERSION >= GL_VERSION_3_0
    } else {
        int index;
        if (exts_i == NULL) return 0;
        for (index = 0; index < num_exts_i; index++) {
            const char *e = exts_i[index];

            if (exts_i[index] != NULL && strcmp(e, ext) == 0)
                return 1;
        }
    }
#endif

    return 0;
}

static void FindGLCoreVersion(void) {
    /* Thank you @elmindreda
     * https://github.com/elmindreda/greg/blob/master/templates/greg.c.in#L176
     * https://github.com/glfw/glfw/blob/master/src/context.c#L36
     */
    const char* prefixes[] = {
        "OpenGL ES-CM ",
        "OpenGL ES-CL ",
        "OpenGL ES ",
        NULL
    };

    const char *version = (const char*)glGetString(GL_VERSION);
    if (!version)
        return;

    for (int i = 0;  prefixes[i];  i++) {
        const size_t length = strlen(prefixes[i]);
        if (strncmp(version, prefixes[i], length) == 0) {
            version += length;
            break;
        }
    }

/* PR #18 */
#ifdef _MSC_VER
    sscanf_s(version, "%d.%d", &max_loaded_major, &max_loaded_minor);
#else
    sscanf(version, "%d.%d", &max_loaded_major, &max_loaded_minor);
#endif
}

#define X(T, N)             \\
    if (!(gll_##N = (T)load(#N))) \\
        failures++;
int glLoadGLLoader(glLoadProc load) {
    int failures = 0;
    if (!(glGetString = (PFNGLGETSTRINGPROC)load("glGetString")))
        return 1;
    if (!glGetString(GL_VERSION))
        return 1;
    FindGLCoreVersion();
                      
LOADER

# Load gl functions
functions.each do |k, v|
    maj, min = k.split '.'
    puts "#if GL_LOAD_VERSION >= GL_VERSION_#{maj}_#{min}"
    puts "\tGL_FUNCTIONS_#{maj}_#{min}"
    puts "#endif"
end

puts <<EXTLOADER
#undef X

    if (!GetExtensions())
        return failures + 1;

#define X(T, N)                   \\
    if (!(gll_##N = (T)load(#N))) \\
        failures++;               \\
    else                          \\
        EXT_##N##_loaded = 1;
EXTLOADER

# Load gl extension functions
extensions.each do |k, v|
    puts "#if defined(GL_EXT_#{k[3..]})"
    puts "\tif ((EXT_#{k}_loaded = FindExtension(\"#{k}\"))) {"
    puts "\t\t#{k}_FUNCTIONS"
    puts "\t}"
    puts "#endif"
end

                      
puts <<FINISH
    FreeExtensions();
                      
    return failures;
}
#undef X
FINISH

puts "#endif // OPENGL_LOADER_IMPLEMENTATION" if PRINT_SINGLE_FILE
