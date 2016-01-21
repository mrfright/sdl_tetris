#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define BLOCK_SIZE 15
#define GAME_WIDTH 10
#define GAME_HEIGHT 20

SDL_Window *Window = NULL;
SDL_Renderer *Renderer = NULL;
SDL_Texture *Texture = NULL;

SDL_Texture* loadTexture(char *path) {
    SDL_Texture *newTexture = NULL;
    SDL_Surface* loadedSurface = IMG_Load(path);

    if(loadedSurface == NULL) {
        printf("unable to load image %s: %s\n", path, IMG_GetError());
        return NULL;
    }

    newTexture = SDL_CreateTextureFromSurface(Renderer, loadedSurface);
    if(newTexture == NULL) {
        printf("unable to create surface from %s: %s\n", path, SDL_GetError());
        return NULL;
    }

    SDL_FreeSurface(loadedSurface);
    return newTexture;
}

int main(void) {

    //init
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL couldn't initialize: %s\n", SDL_GetError());
        return -1;
    }

    if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
        printf("warning: linear texture filtering not enabled\n");
    }

    Window = SDL_CreateWindow("SDL Tetris",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              BLOCK_SIZE * GAME_WIDTH,
                              BLOCK_SIZE * GAME_HEIGHT,
                              SDL_WINDOW_SHOWN);

    if(Window == NULL) {
        printf("window couldn't be created: %s\n", SDL_GetError());
        return -1;
    }

    Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);
    if(Renderer == NULL) {
        printf("renderer couldn't be created: %s\n", SDL_GetError());
        return -1;
    }

    SDL_SetRenderDrawColor(Renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("SDL_image could not initialize: %s\n", IMG_GetError());
        return -1;
    }

    //load media
/*    Texture = loadTexture("tetris.png");
    if(Texture == NULL) {
        printf("failed to load texture image\n");
        return -1;
    }
not using for now*/
    
    int still_playing = 1;

    SDL_Event e;
    while(still_playing) {
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) {
                still_playing = 0;
            }
        }

        SDL_SetRenderDrawColor(Renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(Renderer);
        //SDL_RenderCopy(Renderer, Texture, NULL, NULL);

        SDL_Rect fillRect = {0, 0, BLOCK_SIZE, BLOCK_SIZE};
        SDL_SetRenderDrawColor(Renderer, 0x00, 0xFF, 0x00, 0xFF);
        SDL_RenderFillRect(Renderer, &fillRect);

        SDL_RenderPresent(Renderer);
    }

    SDL_DestroyTexture(Texture);
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);
    Window = NULL;
    Renderer = NULL;
    IMG_Quit();
    SDL_Quit();
    
    return(0);
}
