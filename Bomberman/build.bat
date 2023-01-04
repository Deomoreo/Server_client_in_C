clang.exe  -I include\ -I include\SDL2-2.26.0\include -o bin\bomberman.exe src\bomberman.c -L include\SDL2-2.26.0\lib\x64 -lSDL2 -lws2_32
clang.exe  -I include\ -I include\SDL2-2.26.0\include -o bin\bomberman_client.exe src\bomberman_client.c -L include\SDL2-2.26.0\lib\x64 -lSDL2 -lws2_32
.\bin\bomberman.exe