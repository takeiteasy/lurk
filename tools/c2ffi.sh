#!/usr/bin/env sh

c2ffi "$@" | jq -r 'map(select(.tag == "function").name).[]'
