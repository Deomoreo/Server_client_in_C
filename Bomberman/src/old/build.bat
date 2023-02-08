clang  -I include\ -I include\SDL2-2.26.0\include -o bin\bomberman.exe src\old\bomberman.c -L include\SDL2-2.26.0\lib\x64 -lSDL2 -lws2_32
clang  -I include\ -I include\SDL2-2.26.0\include -o bin\bomberman_controller.exe src\old\bomberman_controller.c -L include\SDL2-2.26.0\lib\x64 -lSDL2 -lws2_32
clang  -I include\ -I include\SDL2-2.26.0\include -o bin\bomberman_client.exe src\bomberman_client.c -L include\SDL2-2.26.0\lib\x64 -lSDL2 -lws2_32

