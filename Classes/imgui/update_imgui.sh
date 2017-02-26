#!/usr/bin/env bash

function download ()
{
	wget -N $1
}

# ImGui core and demo:
download 'http://raw.githubusercontent.com/ocornut/imgui/master/imconfig.h'
download 'http://raw.githubusercontent.com/ocornut/imgui/master/imgui.cpp'
download 'http://raw.githubusercontent.com/ocornut/imgui/master/imgui.h'
download 'http://raw.githubusercontent.com/ocornut/imgui/master/imgui_demo.cpp'
download 'http://raw.githubusercontent.com/ocornut/imgui/master/imgui_draw.cpp'
download 'http://raw.githubusercontent.com/ocornut/imgui/master/imgui_internal.h'

# OpenGL 2.0 code:
download 'http://raw.githubusercontent.com/ocornut/imgui/master/examples/opengl2_example/imgui_impl_glfw.cpp'
download 'http://raw.githubusercontent.com/ocornut/imgui/master/examples/opengl2_example/imgui_impl_glfw.h'


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
