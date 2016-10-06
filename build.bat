@echo off

cl -Zi sdl_tetris.c "SDL2-2.0.4\lib\x64\SDL2.lib" "SDL2-2.0.4\lib\x64\SDL2.lib" "SDL2_image-2.0.1\lib\x64\SDL2_image.lib" /DWINDOWS
