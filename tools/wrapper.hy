#!/usr/bin/env hy
(import subprocess)
(import json)
(import argparse)
(import sys [exit])
(import re)

(setv **header-input** None
      **list-input** None
      **prefix** None)

(let [parser (argparse.ArgumentParser :description "Wrap a library for lurk"
                                      :formatter-class argparse.ArgumentDefaultsHelpFormatter)]
  (parser.add-argument "--header" :help "Path to header file")
  (parser.add-argument "--list" :help "Path to list of functions to wrap")
  (parser.add-argument "--prefix" :help "Remove prefix (optional)")
  (let [args (parser.parse-args)]
    (if (or (not args.header)
            (not args.list))
        (do
          (print parser)
          (exit 1))
        (setv **header-input** args.header
              **list-input** args.list
              **prefix** args.prefix))))

(setv **wrap-funcs** (.split (.read (open **list-input**))))

(defmacro run-cmd [#* args]
  `(let [result (subprocess.run (list ~args) :stdout subprocess.PIPE)]
     (result.stdout.decode "utf-8")))

(defmacro if-match [string pattern pos-body neg-body]
  `(let [$ (re.match ~pattern ~string)]
     (if $
       ~pos-body
       ~neg-body)))

(defn snake->camel [name]
  (let [out (if-match name r"^[a-z_]+$"
                      (.join "" (lfor p (.split name "_") (.capitalize p)))
                      name)]
    (if **prefix**
        (re.sub f"^{**prefix**}" "" out)
        out)))

(defn convert-parameters [new-name entry]
  (let [params (get entry "parameters")]
    (lfor p params
          (let [tag (get (get p "type") "tag")
                ptype (match tag
                             ":int" "int"
                             ":float" "float"
                             ":pointer" (let [sub-tag (get (get (get p "type") "type") "tag")]
                                          (if (= sub-tag ":void")
                                              "void*"
                                              (+ sub-tag "*")))
                             "uint32_t" "int"
                             default tag)]
            f"{ptype} {(get p "name")}"))))

(defn blank-line []
  (print ""))

(defn generate-struct [new-name params]
  (print "typedef struct {")
  (for [p params]
    (print f"    {p};"))
  (print (+ "} " f"lurk{new-name}Data;"))
  (blank-line))

(defn generate-funcs [new-name old-name params]
  (print (+ f"void lurk{new-name}(lurkState *state{(if params (+ ", " (.join ", " params)) "")}) " "{"))
  (print "    lurkCommand* cmd = malloc(sizeof(lurkCommand));")
  (print f"    cmd->type = lurkCommand{new-name};")
  (if params
      (do
        (print f"    lurk{new-name}Data* cmdData = malloc(sizeof(lurk{new-name}Data));")
        (for [p params]
          (let [pname (get (.split p " ") 1)]
            (print f"    cmdData->{pname} = {pname};")))
        (print "    cmd->data = cmdData;"))
      (print f"    cmd->data = NULL;"))
  (print "    PushCommand(state, cmd);")
  (print "}")
  (blank-line))

(defmacro for-entry-tree [#* body]
  `(for [entry tree]
     (let [old-name (get entry "name")
           new-name (snake->camel old-name)]
       (when (in old-name **wrap-funcs**)
         (do
           ~@body)))))


(defn generate-enums [tree]
  (print "typedef enum {")
  (for-entry-tree
    (print f"    lurkCommand{new-name},"))
  (print "} lurkCommandType;")
  (blank-line))

(defn generate-handler [tree]
  (print "static void FreeCommand(lurkCommand* command) {")
  (print "    lurkCommandType type = command->type;")
  (print "    switch
(type) {")
  (for-entry-tree
    (when (get entry "parameters")
      (print (+ f"    case {(+ "lurkCommand" new-name)}: " "{"))
      (print f"        lurk{new-name}Data* data = (lurk{new-name}Data*)command->data;")
      (print "        free(data);")
      (print "        break;")
      (print "    }")))
  (print "    default:")
  (print "        break;")
  (print "    }")
  (print "    free(command);")
  (print "}")
  (blank-line)
  (print "static void ProcessCommand(lurkCommand* command) {")
  (print "    lurkCommandType type = command->type;")
  (print "    switch (type) {")
  (for-entry-tree
    (let [params (convert-parameters new-name entry)]
      (if params
          (do
            (print (+ f"    case {(+ "lurkCommand" new-name)}: " "{"))
            (print f"        lurk{new-name}Data* data = (lurk{new-name}Data*)command->data;")
            (let [formatted-params (.join ", " (lfor p params (+ "data->" (get (.split p " ") 1))))]
              (print f"        {old-name}({formatted-params});"))
            (print "        break;")
            (print "    }"))
          (do
            (print f"    case {(+ "lurkCommand" new-name)}:")
            (print f"        {old-name}();")
            (print "        break;"))))
    )
  (print "    default:")
  (print "        abort();")
  (print "    }")
  (print "}"))

(defn generate-header [tree]
  (for-entry-tree
    (let [params (convert-parameters new-name entry)]
      (if params
          (print f"EXPORT void lurk{new-name}(lurkState* state, {(.join ", " params)});")
          (print f"EXPORT void lurk{new-name}(lurkState* state);"))))
  (blank-line))

(let [tree (json.loads (run-cmd "c2ffi" **header-input**))]
  (generate-header tree)
  (generate-enums tree)
  (print "typedef struct {")
  (print "    lurkCommandType type;")
  (print "    void* data;");
  (print "} lurkCommand;")
  (blank-line)
  (print "static void PushCommand(lurkState* state, lurkCommand* command) {")
  (print "    ezStackAppend(&state->commandQueue, command->type, (void*)command);")
  (print "}")
  (blank-line)
  (for-entry-tree
    (let [params (convert-parameters new-name entry)]
      (when params
        (generate-struct new-name params))
      (generate-funcs new-name old-name params)))
  (generate-handler tree))
