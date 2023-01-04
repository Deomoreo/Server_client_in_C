#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <stdio.h>
#include <float.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "bomberman.h"

#define FLT_EXPO_SIZE 5
#define MSG_SIZE 50


float message_to_float(char* buffer, size_t position)
{
    char buf[sizeof(float) + 1];
    size_t j = position;
    for (size_t i = 0; i < sizeof(buf); ++i)
    {
       buf[i] = buffer[j];
       ++j;
    }
    buf[sizeof(buf) - 1] = '\0';
    return strtof(buf, NULL);
}

static void bomberman_game_mode_init(game_mode_t *game_mode)
{
    game_mode->timer = 60;
}

static void bomberman_map_init(cell_t *map)
{
}

static void bomberman_player_init(player_t *player)
{
    player->pos.x = 0;
    player->pos.y = 0;
    player->number_of_lifes = 1;
    player->number_of_bombs = 1;
    player->score = 0;
    player->speed = 100;
}

int bomberman_graphics_init(SDL_Window **window, SDL_Renderer **renderer, SDL_Texture **texture)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return -1;
    }

    *window = SDL_CreateWindow("SDL is active!", 100, 100, 512, 512, 0);
    if (!*window)
    {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!*renderer){
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return -1;
    }

    int width;
    int height;
    int channels;
    unsigned char *pixels = stbi_load("materials\\green_goblin.png", &width, &height, &channels, 4);
    if (!pixels)
    {
        SDL_Log("Unable to open image");
        SDL_DestroyRenderer(*renderer);
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return -1;
    }

    *texture = SDL_CreateTexture(*renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, width, height);
    if (!*texture)    
    {
        SDL_Log("Unable to create texture: %s", SDL_GetError());
        free(pixels);
        SDL_DestroyRenderer(*renderer);
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return -1;
    }

    SDL_UpdateTexture(*texture, NULL, pixels, width * 4);
    SDL_SetTextureAlphaMod(*texture, 255);
    SDL_SetTextureBlendMode(*texture, SDL_BLENDMODE_BLEND);
    free(pixels);
    return 0;
}

int main(int argc, char **argv)
{
    WSADATA wsa;
    SOCKET Server_socket, client_socket;
    struct sockaddr_in server, client;
    char *message, *client_message;
    // Initialize Winsock
    printf("\nInitializing Winsock...");
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0)
    {
        printf("Failed. Error Code : %d", WSAGetLastError());
        return 1;
    }
    printf("Initialized.\n");

    // Create a socket
    Server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (Server_socket < 0)
    {
        printf("Could not create socket : %d", WSAGetLastError());
    }
    printf("Socket created.\n");

    // Prepare the sockaddr_in structure
    inet_pton (AF_INET, INADDR_ANY, &server.sin_addr);
    server.sin_family = AF_INET;
    server.sin_port = htons(8888);

    // Bind the socket to the specified address and port
    if (bind(Server_socket, (struct sockaddr *)&server, sizeof(server)))
    {
        printf("Bind failed with error code : %d", WSAGetLastError());
        return 1;
    }
    printf("Bind done.\n");

    // Listen for incoming connections
    listen(Server_socket, 3);
    printf("Listening for incoming connections...\n");

    // Receive a message from the client
    client_message = (char *)malloc(MSG_SIZE);
    int client_in_size = sizeof(client);
    int len = 0;
    len = recvfrom(Server_socket, client_message, MSG_SIZE, 0, (struct sockaddr *)&client , &client_in_size);
    if(len > 0)
    {
        char addr_as_string [64];
        inet_ntop (AF_INET, &client.sin_addr , addr_as_string , 64);
    }
    

    game_mode_t game_mode;
    cell_t map[64 * 64];
    player_t player;

    bomberman_game_mode_init(&game_mode);
    bomberman_map_init(map);
    bomberman_player_init(&player);

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;

    if (bomberman_graphics_init(&window, &renderer, &texture))
    {
        return -1;
    }

    // game loop
    int running = 1;

    Uint32 currentTime, previousTime;
    float deltaTime;
    // Initialize previousTime to the current time
    previousTime = SDL_GetTicks();


    float x =0;
    float y =0;

    while (running)
    {
        client_message = (char *)malloc(MSG_SIZE);
        int client_in_size = sizeof(client);
        int len = recvfrom(Server_socket, client_message,MSG_SIZE,0,(struct sockaddr *)&client , &client_in_size);
        if (len < 0)
        {
            printf("Receive failed with error code : %d", WSAGetLastError());
            char addr_as_string[64];
            inet_ntop(AF_INET, &client.sin_addr, client_message, 64);
        }
        printf("message client :%s\n",client_message);
        y = message_to_float(client_message,8*1);
        x = message_to_float(client_message,8*0);
        
        currentTime = SDL_GetTicks();
        deltaTime = (currentTime - previousTime) / 1000.0f;
        previousTime = currentTime;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = 0;
            }
        }
        float multiplier = deltaTime * player.speed;
        SDL_PumpEvents();
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        player.pos.x += x * multiplier;
        player.pos.y += y * multiplier;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_Rect target_rect = {player.pos.x, player.pos.y, 64, 64};
        SDL_RenderCopy(renderer, texture, NULL, &target_rect);

        SDL_RenderPresent(renderer);
        
    }
    closesocket(Server_socket);
    WSACleanup();

    return 0;
}