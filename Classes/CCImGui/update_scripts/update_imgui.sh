#!/usr/bin/env bash

source utility.sh

# ImGui core and demo:
download 'http://raw.githubusercontent.com/ocornut/imgui/master/imconfig.h'../imgui
download 'http://raw.githubusercontent.com/ocornut/imgui/master/imgui.cpp'../imgui
download 'http://raw.githubusercontent.com/ocornut/imgui/master/imgui.h'../imgui
download 'http://raw.githubusercontent.com/ocornut/imgui/master/imgui_demo.cpp'../imgui
download 'http://raw.githubusercontent.com/ocornut/imgui/master/imgui_draw.cpp'../imgui
download 'http://raw.githubusercontent.com/ocornut/imgui/master/imgui_internal.h'../imgui

# OpenGL 2.0 code:
download 'http://raw.githubusercontent.com/ocornut/imgui/master/examples/opengl2_example/imgui_impl_glfw.cpp'../imgui
download 'http://raw.githubusercontent.com/ocornut/imgui/master/examples/opengl2_example/imgui_impl_glfw.h'../imgui

# the stb code:
download 'http://raw.githubusercontent.com/ocornut/imgui/master/stb_rect_pack.h' ../imgui
download 'http://raw.githubusercontent.com/ocornut/imgui/master/stb_textedit.h' ../imgui
download 'http://raw.githubusercontent.com/ocornut/imgui/master/stb_truetype.h' ../imgui

# Classes that extend imgui
# imgui_extend.cpp'
# imgui_extend.h'


# Platform specific source
# imgui_impl_glfw_linux.cpp'
# imgui_impl_glfw_mac.cpp'
# imgui_impl_glfw_win32.cpp'

# Lua stuff -- there are some other bindings here: https://github.com/patrickriordan/imgui_lua_bindings
# imgui_lua.cpp'
# imgui_lua.hpp'
