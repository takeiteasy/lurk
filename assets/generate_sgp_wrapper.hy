#!/usr/bin/env hy
(import subprocess)
(import json)

(setv **wrap-funcs** (.split (.read (open "assets/sgp_funcs_list.txt"))))

(defmacro run-cmd [#* args]
  `(let [result (subprocess.run (list ~args) :stdout subprocess.PIPE)]
     (result.stdout.decode "utf-8")))

(defn snake->camel [name]
  (let [parts (.split name "_")
        new-name (get parts (slice 1 None))]
    (.join "" (lfor p new-name
                    (.capitalize p)))))

(defn convert-parameters [new-name entry]
  (let [params (get entry "parameters")]
    (lfor p params
          (let [tag (get (get p "type") "tag")
                ptype (match tag
                             ":int" "int" ;; TODO: Handle :void*
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
  (print (+ "} " f"gk{new-name}Data;"))
  (blank-line))

(defn generate-funcs [new-name old-name params]
  (print (+ f"void gk{new-name}(gkState *state{(if params (+ ", " (.join ", " params)) "")}) " "{"))
  (print "    gkCommand* cmd = malloc(sizeof(gkCommand));")
  (print f"    cmd->type = gkCommand{new-name};")
  (if params
      (do
        (print f"    gk{new-name}Data* cmdData = malloc(sizeof(gk{new-name}Data));")
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
    (print f"    gkCommand{new-name},"))
  (print "} gkCommandType;")
  (blank-line))

(defn generate-handler [tree]
  (print "static void FreeCommand(gkCommand* command) {")
  (print "    gkCommandType type = command->type;")
  (print "    switch
(type) {")
  (for-entry-tree
    (when (get entry "parameters")
      (print (+ f"    case {(+ "gkCommand" new-name)}: " "{"))
      (print f"        gk{new-name}Data* data = (gk{new-name}Data*)command->data;")
      (print "        free(data);")
      (print "        break;")
      (print "    }")))
  (print "    default:")
  (print "        break;")
  (print "    }")
  (print "    free(command);")
  (print "}")
  (blank-line)
  (print "static void ProcessCommand(gkCommand* command) {")
  (print "    gkCommandType type = command->type;")
  (print "    switch (type) {")
  (for-entry-tree
    (let [params (convert-parameters new-name entry)]
      (if params
          (do
            (print (+ f"    case {(+ "gkCommand" new-name)}: " "{"))
            (print f"        gk{new-name}Data* data = (gk{new-name}Data*)command->data;")
            (let [formatted-params (.join ", " (lfor p params (+ "data->" (get (.split p " ") 1))))]
              (print f"        {old-name}({formatted-params});"))
            (print "    }"))
          (do
            (print f"    case {(+ "gkCommand" new-name)}:")
            (print f"        {old-name}();")
            (print "        break;"))))
    )
  (print "    default:")
  (print "        abort();")
  (print "    }")
  (print "    FreeCommand(command);")
  (print "}"))

(defn generate-header [tree]
  (for-entry-tree
    (let [params (convert-parameters new-name entry)]
      (if params
          (print f"EXPORT void gk{new-name}(gkState* state, {(.join ", " params)});")
          (print f"EXPORT void gk{new-name}(gkState* state);"))))
  (blank-line))

(let [tree (json.loads (run-cmd "c2ffi" "deps/sokol_gp.h"))]
  (generate-header tree)
  (generate-enums tree)
  (print "typedef struct {")
  (print "    gkCommandType type;")
  (print "    void* data;");
  (print "} gkCommand;")
  (blank-line)
  (print "static void PushCommand(gkState* state, gkCommand* command) {")
  (blank-line)
  (print "}")
  (blank-line)
  (for-entry-tree
    (let [params (convert-parameters new-name entry)]
      (when params
        (generate-struct new-name params))
      (generate-funcs new-name old-name params)))
  (generate-handler tree))
