cmake_minimum_required(VERSION 3.30)
project(archivos C)

set(CMAKE_C_STANDARD 11)

add_executable(archivos main.c sala.c sala.h mini_shell.c mini_shell.h sucursal.c sucursal.h)
