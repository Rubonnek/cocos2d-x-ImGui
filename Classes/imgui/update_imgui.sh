#!/usr/bin/env bash

function download ()
{
	wget -N $1
}

download 'http://raw.githubusercontent.com/ocornut/imgui/master/imconfig.h'
download 'http://raw.githubusercontent.com/ocornut/imgui/master/imgui.cpp'
download 'http://raw.githubusercontent.com/ocornut/imgui/master/imgui.h'
download 'http://raw.githubusercontent.com/ocornut/imgui/master/imgui_demo.cpp'
download 'http://raw.githubusercontent.com/ocornut/imgui/master/imgui_draw.cpp'
download 'http://raw.githubusercontent.com/ocornut/imgui/master/imgui_extend.cpp'
download 'http://raw.githubusercontent.com/ocornut/imgui/master/imgui_extend.h'
download 'http://raw.githubusercontent.com/ocornut/imgui/master/imgui_impl_glfw.cpp'
download 'http://raw.githubusercontent.com/ocornut/imgui/master/imgui_impl_glfw.h'
download 'http://raw.githubusercontent.com/ocornut/imgui/master/imgui_impl_glfw_linux.cpp'
download 'http://raw.githubusercontent.com/ocornut/imgui/master/imgui_impl_glfw_mac.cpp'
download 'http://raw.githubusercontent.com/ocornut/imgui/master/imgui_impl_glfw_win32.cpp'
download 'http://raw.githubusercontent.com/ocornut/imgui/master/imgui_internal.h'
download 'http://raw.githubusercontent.com/ocornut/imgui/master/imgui_lua.cpp'
download 'http://raw.githubusercontent.com/ocornut/imgui/master/imgui_lua.hpp'
