# Demo Application for Android V8 Embedded

Demo Application for embedding V8 engine into android project 
with a V8 hello world example

## V8 version

v7.2


## Cloning the code

You need git-lfs to clone this code

## build config for v8

```
android_unstripped_runtime_outputs = true
v8_use_external_startup_data = false
is_debug = false
symbol_level = 1
target_cpu = "arm"
target_os = "android"
use_goma = false
v8_enable_i18n_support = false
v8_static_library = true
is_component_build = false
v8_monolithic = true
v8_android_log_stdout = true
```

## NDK Version
r19

