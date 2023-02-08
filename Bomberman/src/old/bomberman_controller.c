#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <stdio.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>


int graphics_init(SDL_Window **window, SDL_Renderer **renderer)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return -1;
    }
    *window = SDL_CreateWindow("SDL is active!", 100, 100, 512, 512, 0);
    if (!window)
    {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        SDL_Quit();
        return -1;
    }
    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer)
    {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return -1;
    }
    return 0;
}

int main()
{
    WSADATA wsa;
    SOCKET Server_socket;
    struct sockaddr_in Server_addr;
    char *message, server_reply[2000];
    int recv_size;

    SDL_Window* window;
    SDL_Renderer* renderer;
    graphics_init(&window,&renderer);
    int running =1;
    float end_frame_time = 0;
    float x = 0;
    float y = 0;

    // Initialize Winsock
    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed. Error Code : %d", WSAGetLastError());
        return 1;
    }

    printf("Initialised.\n");


    // Create a socket
    Server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (Server_socket < 0)
    {
        printf("Could not create socket : %d", WSAGetLastError());
    }

    printf("Socket created.\n");

    inet_pton (AF_INET , "62.98.93.45" , &Server_addr.sin_addr ); // this will create a big endian 32 bit address
    Server_addr.sin_family = AF_INET ;
    Server_addr.sin_port = htons(8888);

    // Connect to remote server
    if (connect(Server_socket, (struct sockaddr *)&Server_addr, sizeof(Server_addr)) < 0)
    {
        puts("connect error");
        return 1;
    }
    puts("Connected");

    // Send some data
    // message = "ciaoAAAAAAAAAAAAAAAA";
    // if (sendto(s, message, strlen(message), 0,(struct sockaddr *)&server ,sizeof(server)) < 0)
    // {
        // puts("Send failed");
        // return 1;
    // }
    // puts("Data Send\n");
    // // Receive a reply from the server
    // if ((recv_size = recv(s, server_reply, 2000, 0)) == SOCKET_ERROR)
    // {
    //     puts("recv failed");
    // }

    // puts("Reply received\n");
    while(running)
    {
        float start_frame_time = SDL_GetTicks64();
        float delta_time = start_frame_time - end_frame_time;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = 0;
            }
        }
        SDL_PumpEvents();
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        y = (keys[SDL_SCANCODE_S] - keys[SDL_SCANCODE_W]);
        x = (keys[SDL_SCANCODE_D] - keys[SDL_SCANCODE_A]);

        char buffer_x[16];
        char buffer_y[16];
        char buffer_xy[50];
        
        sprintf(buffer_x, "%f", x);
        sprintf(buffer_y, "%f", y);
        strcpy(buffer_xy,buffer_x);
        strcat(buffer_xy,buffer_y);
        int sent_bytes = sendto(Server_socket,buffer_xy, strlen(buffer_xy), 0,(struct sockaddr *)&Server_addr, sizeof(Server_addr));
        if (sent_bytes < 0)
        {
            puts("Send failed");
            return 1;
        }
        printf("buffer_xy:%s \n",buffer_xy);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    
        SDL_RenderPresent(renderer);
        end_frame_time = start_frame_time;

        
    }

    // Add a NULL terminating character to make it a proper string before printing
    server_reply[recv_size] = '\0';
    puts(server_reply);

    // Cleanup and exit
    closesocket(Server_socket);
    WSACleanup();

    return 0;
}
// #ifdef _WIN32
// #include <WinSock2.h>
// #include <WS2tcpip.h>
// #else
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// #endif
// #include <stdio.h>
// #define SDL_MAIN_HANDLED
// #include <SDL.h>
// int s;
// void prepare_UDP_Socket(struct sockaddr_in* t)
// {
//     struct sockaddr_in sin = *t;
//     #ifdef _WIN32
//         // this part is only required on Windows: it initializes the Winsock2 dll
//         WSADATA wsa_data;
//         if (WSAStartup(0x0202, &wsa_data))
//         {
//             printf("unable to initialize winsock2 \n");
//             exit(-1);
//         }
//     #endif
    
//     s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
//     if (s < 0)
//     {
//         printf("unable to initialize the UDP socket \n");
//         exit(-1);
//     }
//     printf("socket %d created \n", s);
    
//     inet_pton(AF_INET, "192.168.1.114", &sin.sin_addr); // this will create a big endian 32 bit address
//     sin.sin_family = AF_INET;
//     sin.sin_port = htons(9999); // converts 9999 to big endian
//     //int sent_bytes = sendto(s, "CIAO", 4, 0, (struct sockaddr *)&sin, sizeof(sin));
//     //printf("sent %d bytes via UDP \n", sent_bytes);
// }
// int graphics_init(SDL_Window **window, SDL_Renderer **renderer)
// {
//     if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) != 0)
//     {
//         SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
//         return -1;
//     }
//     *window = SDL_CreateWindow("SDL is active!", 100, 100, 512, 512, 0);
//     if (!window)
//     {
//         SDL_Log("Unable to create window: %s", SDL_GetError());
//         SDL_Quit();
//         return -1;
//     }
//     *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

//     if (!renderer)
//     {
//         SDL_Log("Unable to create renderer: %s", SDL_GetError());
//         SDL_DestroyWindow(*window);
//         SDL_Quit();
//         return -1;
//     }
//     return 0;
// }
// int main(int argc , char **argv )
// {
//     struct sockaddr_in sin;
//     prepare_UDP_Socket(&sin);
//     SDL_Window* window;
//     SDL_Renderer* renderer;
//     graphics_init(&window,&renderer);
//     puts("Window");
//     int running =1;
//     float end_frame_time = 0;
//     float x =0;
//     float y =0;
//     while(running)
//     {
//         float start_frame_time = SDL_GetTicks64();
//         float delta_time = start_frame_time - end_frame_time;

//         SDL_Event event;
//         while (SDL_PollEvent(&event))
//         {
//             if (event.type == SDL_QUIT)
//             {
//                 running = 0;
//             }
//         }
//         SDL_PumpEvents();
        
//         const Uint8 *keys = SDL_GetKeyboardState(NULL);

        
//         x = (keys[SDL_SCANCODE_RIGHT] - keys[SDL_SCANCODE_LEFT]);

//         y = (keys[SDL_SCANCODE_DOWN] - keys[SDL_SCANCODE_UP]) ;
//         printf("COORDINATES ARE x:%f   y:%f \n",x,y);
//         char buffer_x[sizeof(float)];
//         char buffer_y[sizeof(float)];


//         int ret1 = snprintf(buffer_x, sizeof buffer_x, "%f", x);
//         int ret2 = snprintf(buffer_y, sizeof buffer_y, "%f", y);

//         char send_buffer [128];
//         puts("Client is running");
//         errno_t error = strcpy_s(send_buffer,(size_t)64,buffer_x);
//         if(error)
//         {
//             puts("ERROR");
//         }
//          error = strcat_s(send_buffer,(size_t)64 ," " );
        
//          error = strcat_s(send_buffer,(size_t)64 ,buffer_y);
//         if(error)
//         {
//             puts("ERROR");
//         }
        
//         int sent_bytes = sendto(s,send_buffer, strlen(send_buffer), 0, (struct sockaddr *)&sin, sizeof(sin));
//         printf("buffer is %s \n",send_buffer);
//         printf("sent bytes is  %i \n",sent_bytes);

//         SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
//         SDL_RenderClear(renderer);

        
//         SDL_RenderPresent(renderer);
//         end_frame_time = start_frame_time;
//         //puts("END OF MAIN");
//     }


//     return 0;
// }