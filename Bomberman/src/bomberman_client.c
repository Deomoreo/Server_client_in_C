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
#include <stdbool.h>
#include "byte_converter.h"

#define SDL_MAIN_HANDLED

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "bomberman.h"
#include "LinkedList.h"


#define FLT_EXPO_SIZE 5
#define MSG_SIZE 50
#define NETWORK_PERIOD 0.2f
#define TIMER_DROP_EGGBOMB 5.f


WSADATA wsa;
SOCKET Server_socket, self_socket;
struct sockaddr_in Server_addr;
unsigned char *message, server_reply[2000];
int recv_size, running;
player_t player;
eggbomb_t eggbomb;
SDL_Window* window;
SDL_Renderer* renderer;
Uint64 currentTime, previousTime;
float delta_time, FPS, secondAccumulator,timer_drop_eggbomb = 0;
fd_set read_fds;
int server_socket, ret;
list_double_node* obj_list; 
int bind_error;
player_t* online_players[2];
SDL_Rect* map_obstacle;
const Uint8* Keys;

SDL_Texture *Get_Texture(char *path)
{
    int width;
    int height;
    int channels;
    unsigned char *pixels = stbi_load(path, &width, &height, &channels, 4);
    if (!pixels)
    {
        SDL_Log("Unable to open image");

        return NULL;
    }

    SDL_Log("Image width: %d height: %d channels: %d", width, height, channels);
    SDL_Texture *tex;
    tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, width, height);
    if (!tex)
    {
        SDL_Log("Unable to create texture: %s", SDL_GetError());
        free(pixels);

        return NULL;
    }

    SDL_UpdateTexture(tex, NULL, pixels, width * 4);
    SDL_SetTextureAlphaMod(tex, 255);
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    free(pixels);
    return tex;
}
player_t *InstantiatePlayer(int ID)
{
    player_t *player = (player_t *)calloc(1, sizeof(player_t));
    player->position.x = 0;
    player->position.y = 0;
    player->number_of_lifes = 1;
    player->number_of_bombs = 1;
    player->score = 0;
    player->speed = 100;
    player->ID = ID;
    player->texture = Get_Texture("materials\\paperella.png");
    return player;
}
eggbomb_t* InstantiateEggbomb()
{
    eggbomb_t* eggbomb = (eggbomb_t*)calloc(1,sizeof(eggbomb_t));
    eggbomb->position.x = player.position.x;
    eggbomb->position.y = player.position.y;
    eggbomb->state = 0;
    eggbomb->next = NULL;
    eggbomb->texture = Get_Texture("materials\\EGGOLD.png");
    list_double_node* newNode = (list_double_node*)calloc(1,sizeof(list_double_node));
    newNode->value = eggbomb;
    doublelist_append(&obj_list,newNode);
    return eggbomb;
}
void DestroyPlayer(player_t *to_destroy)
{
    SDL_free(to_destroy->texture);
    free(to_destroy);
}
void ExplodeEggBomb()
{
    SDL_Rect wallee = {eggbomb.position.x - 225 * 0.5f, eggbomb.position.y + 20, 300, 20}; //left //up //lenght //hight
    map_obstacle[28] = wallee;
    SDL_SetRenderDrawColor(renderer, 0xAA, 0xAA, 0xAA, 0xFF);

    // SDL_Rect walleee = {eggbomb.position.x, eggbomb.position.y , 150, 20}; //left //up //lenght //hight
    // map_obstacle[29] = walleee;
}
void DestroyEggTexture(eggbomb_t* to_destroy)
{
    ExplodeEggBomb();
    SDL_free(to_destroy->texture);
}
void UpdatePlayer(player_t *player_to_update)
{
    player_to_update->lerp_accumulator += delta_time * (1 / NETWORK_PERIOD);
    player_to_update->position = vector2_lerp(player_to_update->old_position, player_to_update->latest_position, player_to_update->lerp_accumulator);
    SDL_Rect player_rect = {player_to_update->position.x, player_to_update->position.y, 64, 64};
    SDL_RenderCopy(renderer, player_to_update->texture, NULL, &player_rect);
}
static void bomberman_map_init()
{
    map_obstacle = (SDL_Rect*)calloc(30,sizeof(SDL_Rect));

    SDL_Rect wallLeft = {-1, 0, 30, 1024};
    SDL_Rect wallRight = {1011, 0, 32, 1024};
    SDL_Rect wallUp = {0, -1, 1012, 30};
    SDL_Rect wallDown = {0, 738, 1024, 30};

    SDL_Rect wall = {110, 115, 75, 75}; //left //up //lenght //hight
    SDL_Rect wallBig = {110, 265, 75, 75};    //////////////////////////////////
    SDL_Rect wallSmall = {110, 415, 75, 75};
    SDL_Rect wall1 = {110, 565, 75, 75};

    SDL_Rect wall2 = {260, 115, 75, 75};
    SDL_Rect wall3 = {260, 265, 75, 75};
    SDL_Rect wall4 = {260, 415, 75, 75};
    SDL_Rect wall5 = {260, 565, 75, 75};

    SDL_Rect wall6 = {410, 115, 75, 75};                
    SDL_Rect wall7 = {410, 265, 75, 75};
    SDL_Rect wall8 = {410, 415, 75, 75};
    SDL_Rect wall9 = {410, 565, 75, 75};

    SDL_Rect wall10 = {560, 115, 75, 75};
    SDL_Rect wall11 = {560, 265, 75, 75};
    SDL_Rect wall12 = {560, 415, 75, 75};
    SDL_Rect wall13 = {560, 565, 75, 75};

    SDL_Rect wall14 = {710, 115, 75, 75};
    SDL_Rect wall15 = {710, 265, 75, 75};
    SDL_Rect wall16 = {710, 415, 75, 75};
    SDL_Rect wall17 = {710, 565, 75, 75};

    SDL_Rect wall18 = {860, 115, 75, 75};
    SDL_Rect wall19 = {860, 265, 75, 75};
    SDL_Rect wall20 = {860, 415, 75, 75};
    SDL_Rect wall21 = {860, 565, 75, 75};

    
    map_obstacle[0] = wallLeft;         
    map_obstacle[1] = wallRight;
    map_obstacle[2] = wallUp;
    map_obstacle[3] = wallDown;

    map_obstacle[4] = wall;
    map_obstacle[5] = wallBig;
    map_obstacle[6] = wallSmall;
    map_obstacle[7] = wall1;

    map_obstacle[8] = wall2;
    map_obstacle[9] = wall3;
    map_obstacle[10] = wall4;
    map_obstacle[11] = wall5;

    map_obstacle[12] = wall6;
    map_obstacle[13] = wall7;
    map_obstacle[14] = wall8;
    map_obstacle[15] = wall9;

    map_obstacle[16] = wall10;
    map_obstacle[17] = wall11;
    map_obstacle[18] = wall12;
    map_obstacle[19] = wall13;

    map_obstacle[20] = wall14;
    map_obstacle[21] = wall15;
    map_obstacle[22] = wall16;
    map_obstacle[23] = wall17;

    map_obstacle[24] = wall18;
    map_obstacle[25] = wall19;
    map_obstacle[26] = wall20;
    map_obstacle[27] = wall21;
}
static void bomberman_player_init(player_t *player)
{
    player->position.x = 0;
    player->position.y = 0;
    player->number_of_lifes = 1;
    player->number_of_bombs = 1;
    player->score = 0;
    player->speed = 100;
}
void spawn_eggbomb_init(eggbomb_t* eggbomb)
{
    eggbomb->position.x = player.position.x;
    eggbomb->position.y = player.position.y;
}
int bomberman_graphics_init(SDL_Window **window, SDL_Renderer **renderer, SDL_Texture **tex)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return -1;
    }

    *window = SDL_CreateWindow("SDL is active!", 100, 100, 1044, 768, 0);
    if (!*window)
    {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!*renderer)
    {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return -1;
    }

    int width;
    int height;
    int channels;
    unsigned char *pixels = stbi_load("materials\\paperella.png", &width, &height, &channels, 4);
    if (!pixels)
    {
        SDL_Log("Unable to open image");
        SDL_DestroyRenderer(*renderer);
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return -1;
    }

    SDL_Log("Image width: %d height: %d channels: %d", width, height, channels);
    *tex = SDL_CreateTexture(*renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, width, height);
    if (!*tex)
    {
        SDL_Log("Unable to create texture: %s", SDL_GetError());
        free(pixels);
        SDL_DestroyRenderer(*renderer);
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return -1;
    }
    
    SDL_UpdateTexture(*tex, NULL, pixels, width * 4);
    SDL_SetTextureAlphaMod(*tex, 255);
    SDL_SetTextureBlendMode(*tex, SDL_BLENDMODE_BLEND);
    free(pixels);
    return 0;
}
void bomberman_init_network(char *ip, unsigned int port)
{

    // Initialize Winsock
    printf("\nInitializing Winsock...");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed. Error Code : %d", WSAGetLastError());
        exit(-1);
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
    memset(&Server_addr, 0, sizeof(Server_addr));
    Server_addr.sin_family = AF_INET;

    printf("\n%d\n", port);
    Server_addr.sin_port = htons((int)port);
    inet_pton(AF_INET, ip, &Server_addr.sin_addr);

    // Connect to server
    bind_error = connect(Server_socket, (struct sockaddr *)&Server_addr, sizeof(struct sockaddr_in));
    if (bind_error != 0)
    {
        printf("Connect failed with error code : %d", WSAGetLastError());
        exit(-1);
    }
    printf("Connected.\n");
    char data[] = {'0', '0', '0', '0'};
    int sent_bytes = sendto(Server_socket, data, 4, 0, (struct sockaddr *)&Server_addr, sizeof(Server_addr));
    int server_size = sizeof(Server_addr);
    sent_bytes = recvfrom(Server_socket, data, 4, 0, (struct sockaddr *)&Server_addr, &server_size);
    int id = bytes_to_int(data);
    printf(" MY PLAYER-ID = %i\n", id);
    player.ID = id;

    message = calloc(1, 50);
}
int send_to_server(unsigned char *data)
{
    int sent_bytes = sendto(Server_socket, (char *)data, 50, 0, (struct sockaddr *)&Server_addr, sizeof(Server_addr));
    if (sent_bytes < 0)
    {
        puts("Send failed");
        return 1;
    }
    return 0;
}
void player_drop_eggbomb()
{
    if (timer_drop_eggbomb > TIMER_DROP_EGGBOMB)
    {
        spawn_eggbomb_init(&eggbomb);
        unsigned char msg[50];
        unsigned char* byteX;
        unsigned char* byteY;

        unsigned char byteID[4];
        unsigned char byteCommand[4];

        int_to_bytes(player.ID,byteID);
        int_to_bytes(3,byteCommand);
        byteX = float_to_bytes(player.position.x);
        byteY = float_to_bytes(player.position.y);
        bytes_append(msg,50,0,byteID,4);
        bytes_append(msg,50,4,byteCommand,4);
        bytes_append(msg,50,8,byteX,4);
        bytes_append(msg,50,12,byteY,4);
        send_to_server(msg);

        timer_drop_eggbomb = 0;
    }
}
const Uint8 *manage_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            running = 0;
            break;
        case SDL_KEYUP:
            if (event.key.keysym.sym == SDLK_SPACE)
            {
                player_drop_eggbomb();
            }
            break;
        }
    }
    SDL_PumpEvents();
    Keys = SDL_GetKeyboardState(NULL);
    return Keys;
}
void manage_network_operations()
{
    unsigned char *ID = calloc(1, sizeof(float));
    unsigned char *command = calloc(1, sizeof(float));

    unsigned char *buffer_x;
    unsigned char *buffer_y;

    secondAccumulator += delta_time;
    if (secondAccumulator > NETWORK_PERIOD)
    {
        secondAccumulator = 0;
        int_to_bytes(player.ID, ID);
        buffer_x = float_to_bytes(player.position.x);
        buffer_y = float_to_bytes(player.position.y);

        memcpy(message, ID, sizeof(int));
        bytes_append(message, 50, 4, command, sizeof(int));

        bytes_append(message, 50, 8, buffer_x, sizeof(float));
        bytes_append(message, 50, 12, buffer_y, sizeof(float));

        send_to_server(message);
    }
    // get info from server
    unsigned long msg_length;
    int error = ioctlsocket(Server_socket, FIONREAD, &msg_length);
    if (msg_length == 0 || error != 0)
        return;

    int server_size = sizeof(Server_addr);
    int n = recvfrom(Server_socket, message, 50, 0, (struct sockaddr *)&Server_addr, &server_size);
    if (n == 0)
        return;
    printf("Received %d bytesfrom %s : %d : %.*s \n", n, inet_ntoa(Server_addr.sin_addr), ntohs(Server_addr.sin_port), n, message);
    for (int i = 0; i < n; ++i)
    {
        printf("%x", message[i]);
    }

    int player_id = bytes_to_int2(message, 0);
    printf("\n RECEIVED PLAYERID %i \n", player_id);

    int commandCode = bytes_to_int2(message, 4);
    printf("\n RECEIVED COMMAND %i \n", commandCode);

    if (commandCode == 0)
    { // command 0 updates player

        float printX, printY;
        printX = bytes_to_float(message, 8);
        printY = bytes_to_float(message, 12);

        // printf("\n PLAYER %i X IS %.2f\n", player_id, printX);
        // printf("\n PLAYER %i Y IS %.2f\n", player_id, printY);

        online_players[player_id - 1]->old_position.x = online_players[player_id - 1]->latest_position.x;
        online_players[player_id - 1]->old_position.y = online_players[player_id - 1]->latest_position.y;
        online_players[player_id - 1]->latest_position.x = printX;
        online_players[player_id - 1]->latest_position.y = printY;
        online_players[player_id - 1]->lerp_accumulator = 0;
    }
    if (commandCode == 1)
    { // command 1 is new player
        // puts("#################################################");
        // puts("NEW PLAYER");
        // puts("#################################################");
        //printf("\n New Player ID = %i \n", player_id);
        player_t *newPlayer = InstantiatePlayer(player_id);
        online_players[newPlayer->ID - 1] = newPlayer;
    }
    if (commandCode == 2)
    {
        // puts("#################################################");
        // puts("PLAYER DIED");
        // puts("#################################################");
        DestroyPlayer(online_players[player_id - 1]);
        online_players[player_id] = NULL;
    }
    if (commandCode == 3)
    {
        // puts("#################################################");
        // puts("PLAYER CACA OVO ORO");
        // puts("#################################################");
        eggbomb_t *newEggbomb = InstantiateEggbomb();
    }
}
float DeltaTimeUpdate()
{

    currentTime = SDL_GetPerformanceCounter();
    delta_time = (float)(currentTime - previousTime) / (float)SDL_GetPerformanceFrequency();

    previousTime = currentTime;
    timer_drop_eggbomb += delta_time;

    float fps = (1.f / delta_time);
    FPS = fps;
    char *windowTitle = calloc(1, 50);
    sprintf(windowTitle, "DeltaTime : %f  FPS: %f", delta_time, fps);
    SDL_SetWindowTitle(window, windowTitle);
    return fps;
}
void map_update()
{
    SDL_Rect target_rect = {player.position.x, player.position.y, 64, 64};

    for (int i = 0; i < 30; i++)
    {
        SDL_Rect current = map_obstacle[i];
        SDL_Rect target_player_rect = {player.position.x, player.position.y, 64, 64};
        SDL_RenderFillRect(renderer,&current);
        if (SDL_HasIntersection(&target_player_rect, &current)) 
        {
            //SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
            // SDL_RenderDrawLine(renderer, target_rect.x, target_rect.y, target_rect.x + wall.w, wall.y + wall.h);
            // SDL_RenderDrawLine(renderer, target_rect.x + target_rect.w, target_rect.y, wall.x, wall.y + wall.h);
            if(current.x < target_rect.x)//right
            {
                player.speedXnegative = 0;
            }
            if(current.x > target_rect.x)//left
            {
                player.speedX = 0;
            }
            if(current.y < target_rect.y)//down
            {
                player.speedYnegative = 0;
            }
            if(current.y > target_rect.y)//UP
            {
                player.speedY = 0;
            }
        }
        else 
        {
            player.speedX = 1;
            player.speedXnegative = 1;
            player.speedY = 1;
            player.speedYnegative = 1;
        }

        if (SDL_HasIntersection(&target_player_rect, &current))
        {
            int w, h;
            SDL_IntersectRect(&target_player_rect, &current, &current);
            w = current.w;
            h = current.h;
            if (w > h)
            {
                if (target_player_rect.y < current.y)
                {
                    player.position.y = current.y - 64;
                }
                else
                {
                    player.position.y = current.y + current.h;
                }
            }
            else
            {
                if (target_player_rect.x < current.x)
                {
                    player.position.x = current.x - 64;
                }
                else
                {
                    player.position.x = current.x + current.w;
                }
            }
        }
    }
}
void objects_update()
{
    list_double_node* head = obj_list;
    
    
    if (head == NULL)
    {
        return;
    }
    while (head)
    {
        eggbomb_t* egg = (eggbomb_t*)head->value;
        SDL_Rect eggbomb_rect = {egg->position.x + 15, egg->position.y + 15, 40 , 48};
        SDL_RenderCopy(renderer,egg->texture, NULL, &eggbomb_rect);
        head = head->next;
        if (timer_drop_eggbomb < TIMER_DROP_EGGBOMB)
        {
        }
        else
        {
            obj_list->next = head;
            DestroyEggTexture(egg);
        }
    }
}
int main(int argc, char **argv)
{
    char *server_ip = argv[1];
    char *test = argv[2];
    int port = atoi(test);
    //printf("port IS %d\n", port);
    bomberman_map_init();
    bomberman_player_init(&player);
    
    bomberman_init_network(server_ip, port);
    if (bomberman_graphics_init(&window, &renderer, &player.texture))
    {
        return -1;
    }

    running = 1;
    
    //game loop
    while (running)
    {
        DeltaTimeUpdate();
        manage_network_operations();
        SDL_SetRenderDrawColor(renderer, 0xAA, 0xFF, 0xAA, 0xFF);
        SDL_RenderClear(renderer);
        for (int i = 0; i < 2; i++)
        {
            player_t *current_player_to_update = online_players[i];
            if (current_player_to_update != NULL)
            {
                UpdatePlayer(current_player_to_update);
            }
        }
        SDL_SetRenderDrawColor(renderer, 0xAA, 0xAA, 0xFF, 0xFF);
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xAA, 0xAA, 0xFF);
        map_update();
        objects_update();
        const Uint8* keys = manage_input();
        player.position.x += (keys[SDL_SCANCODE_RIGHT] * player.speedX - keys[SDL_SCANCODE_LEFT] * player.speedXnegative) * delta_time * player.speed;
        player.position.y += (keys[SDL_SCANCODE_DOWN] * player.speedY  - keys[SDL_SCANCODE_UP] * player.speedYnegative ) * delta_time * player.speed;
        SDL_Rect target_rect = {player.position.x, player.position.y, 64, 64};
        //SDL_Rect bomb_rect = {eggbomb.position.x, eggbomb.position.y, 64, 64};
        SDL_RenderCopy(renderer, player.texture, NULL, &target_rect);
        SDL_RenderPresent(renderer);
    }
    server_reply[recv_size] = '\0';
    closesocket(Server_socket);
    WSACleanup();
    return 0;
}