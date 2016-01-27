#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define BLOCK_SIZE 15
#define GAME_WIDTH 10
#define GAME_HEIGHT 20

SDL_Window *Window = NULL;
SDL_Renderer *Renderer = NULL;
SDL_Texture *Texture = NULL;

struct Block {
    Uint8 Red;
    Uint8 Green;
    Uint8 Blue;
};

/*
struct Piece {
    const int (*blocks)[2];
    int x;
    int y;
};
*/

Block GameBlocks[GAME_WIDTH][GAME_HEIGHT] = {};

void SetGameBlock(int x, int y, Uint8 red, Uint8 green, Uint8 blue) {
    GameBlocks[x][y].Red = red;
    GameBlocks[x][y].Green = green;
    GameBlocks[x][y].Blue = blue;
}

void ClearGameBlock(int x, int y) {
    SetGameBlock(x, y, 0, 0, 0);
}

void ClearAllGameBlocks() {    
    for(int x = 0; x < GAME_WIDTH; ++x) {
        for(int y = 0; y < GAME_HEIGHT; ++y) {
            ClearGameBlock(x, y);
        }
    }
}

void DrawGameBlocks() {
    for(int x = 0; x < GAME_WIDTH; ++x) {
        for(int y = 0; y < GAME_HEIGHT; ++y) {

            SDL_Rect fillRect = {x*BLOCK_SIZE, y*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
            SDL_SetRenderDrawColor(Renderer,
                                   GameBlocks[x][y].Red,
                                   GameBlocks[x][y].Green,
                                   GameBlocks[x][y].Blue,
                                   0xFF);
            SDL_RenderFillRect(Renderer, &fillRect);
            
        }
    }
}

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

//returns 1 if the location is empty (and on the game grid), 0 if not empty (contains a block or off the grid)
int BlockEmpty(int x, int y) {
    return x < GAME_WIDTH &&
           y < GAME_HEIGHT &&
           x >= 0 &&
           y >= 0 &&
           GameBlocks[x][y].Red == 0 &&
           GameBlocks[x][y].Green == 0 &&
           GameBlocks[x][y].Blue == 0;
}

//returns 1 if the location contains a block, 0 if empty
int ContainsBlock(int x, int y) {
    return !BlockEmpty(x, y);
}

int PieceContainsBlock(const int piece[][2], int x, int y) {
    {for(int block = 0; block < 4; ++block) {        
        if(ContainsBlock(piece[block][0]+x, piece[block][1]+y)) {
            return 1;
        }
    }}

    return 0;
}

int CanPlacePiece(const int piece[][2], int x, int y) {
    return !PieceContainsBlock(piece, x, y);
}

//{{x0, y0}, {x1, y1}, {x2, y2}, {x3, y3}}
//piece[blocknum][0=x : 1=y]
/*
  [x,y]
  
  [0,0][1,0][2,0][3,0]
  [0,1][1,1][2,1][3,1]
  [0,2][1,2][2,2][3,2]
  [0,3][1,3][2,3][3,3]
 */

/*
  [][][][]
 */
const int ItetriminoSide[][2] = {{0, 0}, {1, 0}, {2, 0}, {3, 0}};

/*
  []
  []
  []
  []
 */
const int ItetriminoUp[][2] = {{0, 0}, {0, 1}, {0, 2}, {0, 3}};

/*
    []
    []
  [][]
 */
const int JtetriminoUp[][2] = {{1, 0}, {1, 1}, {0, 2}, {1, 2}};

/*
  []
  [][][]
 */
const int JtetriminoRight[][2] = {{0, 0}, {0, 1}, {1, 1}, {2, 1}};

/*
  [][]
  []
  []
 */
const int JtetriminoDown[][2] = {{0, 0}, {1, 0}, {0, 1}, {0, 2}};

/*
  [][][]
  []
 */
const int JtetriminoLeft[][2] = {{0, 0}, {1, 0}, {2, 0}, {0, 1}};

/*
  []
  []
  [][]
 */
const int LtetriminoUp[][2] = {{0, 0}, {0, 1}, {0, 2}, {1, 2}};

/*
  [][][]
  []
 */
const int LtetriminoRight[][2] = {{0, 0}, {1, 0}, {2, 0}, {0, 1}};

/*
  [][]
    []
    []
 */
const int LtetriminoDown[][2] = {{0, 0}, {1, 0}, {1, 1}, {1, 2}};

/*
      []
  [][][]
 */
const int LtetriminoLeft[][2] = {{2, 0}, {0, 1}, {1, 1}, {2, 1}};

/*
  [][]
  [][]
 */
const int Otetrimino[][2] = {{0, 0}, {1, 0}, {0, 1}, {1, 1}};

/*
    [][]
  [][]
 */
const int StetriminoSide[][2] = {{1, 0}, {2, 0}, {0, 1}, {1, 1}};

/*
  []
  [][]
    []
 */
const int StetriminoUp[][2] = {{0, 0}, {0, 1}, {1, 1}, {1, 2}};

/*
  [][][]
    []
 */
const int TtetriminoUp[][2] = {{0, 0}, {1, 0}, {2, 0}, {1, 1}};

/*
    []
  [][]
    []
*/
const int TtetriminoRight[][2] = {{1, 0}, {0, 1}, {1, 1}, {1, 2}};

/*
    []
  [][][]
*/
const int TtetriminoDown[][2] = {{1, 0}, {0, 1}, {1, 1}, {1, 2}};

/*
  []
  [][]
  []
*/
const int TtetriminoLeft[][2] = {{0, 0}, {0, 1}, {1, 1}, {0, 2}};

/*
  [][]
    [][]
*/
const int ZtetriminoSide[][2] = {{0, 0}, {1, 0}, {1, 1}, {2, 1}};

/*
    []
  [][]
  []
*/
const int ZtetriminoUp[][2] = {{1, 0}, {0, 1}, {1, 1}, {0, 2}};




//return 1 for successfully placed, 0 for can't and didn't
int placePiece(int x, int y, const int piece[][2], int red, int green, int blue) {
    if(PieceContainsBlock(piece, x, y)) {
        return 0;
    }

    {for(int block = 0; block < 4; ++block) {
            SetGameBlock(piece[block][0]+x, piece[block][1]+y, red, green, blue);
    }}

    return 1;
}

void removePiece(int x, int y, const int piece[][2]) {
    {for(int coord = 0; coord < 4; ++coord) {        
        ClearGameBlock(piece[coord][0]+x, piece[coord][1]+y);         
    }}
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

    ClearAllGameBlocks();

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
    
    int still_playing = 1;

    SDL_Event e;

    printf("int ItetriminoUp[][2] = {{0, 0}, {1, 0}, {2, 0}, {3, 0}};\n");
    for(int i = 0; i < 4; ++i) {
        printf("ItetriminoUp[%d][0]: %d\tItetriminoUp[%d][1]: %d\n", i, ItetriminoUp[i][0],
               i, ItetriminoUp[i][1]);
    }

    const int (*CurrentPiece)[2] = ZtetriminoUp;
    int current_x = 5;
    int current_y = 5;
    int current_red = 0;
    int current_green = 0;
    int current_blue = 0;

    //use SDL_EnableKeyRepeat for key repeat for while holding button down
    
    while(still_playing) {
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) {
                still_playing = 0;
            }
             else if( e.type == SDL_KEYDOWN ) {
             //Select surfaces based on key press
                 switch( e.key.keysym.sym ) {
                     case SDLK_UP:
                         //rotate piece
                         break;
                     case SDLK_DOWN:
                         //move down
                         //see if can move down
                         //TODO don't check a pieces own blocks
                         //remove piece
                         //check if can move
                         //  if you can, add 1 to y
                         //place piece again

                         removePiece(current_x, current_y, CurrentPiece);
                         if(!PieceContainsBlock(CurrentPiece, current_x, current_y+1)) {
                         //remove current piece
                             
                         //place new piece down
                             
                                        
                         //update coor
                             ++current_y;
                         }
                         
                         placePiece(current_x, current_y, CurrentPiece,
                                    current_red, current_green, current_blue);
                         
                         break;
                     case SDLK_LEFT:
                         //move left
                         //see if can move piece left
                         //remove current piece
                         //place new piece left
                         //update coord
                         removePiece(current_x, current_y, CurrentPiece);
                         if(!PieceContainsBlock(CurrentPiece, current_x-1, current_y)) {
                         //remove current piece
                             
                         //place new piece down
                             
                                        
                         //update coor
                             --current_x;
                         }
                         
                         placePiece(current_x, current_y, CurrentPiece,
                                    current_red, current_green, current_blue);
                         break;
                     case SDLK_RIGHT:
                         //move right
                         //see if can move piece right
                         //remove current piece
                         //place new piece right
                         //update coord
                         removePiece(current_x, current_y, CurrentPiece);
                         if(!PieceContainsBlock(CurrentPiece, current_x+1, current_y)) {
                         //remove current piece
                             
                         //place new piece down
                             
                                        
                         //update coor
                             ++current_x;
                         }
                         
                         placePiece(current_x, current_y, CurrentPiece,
                                    current_red, current_green, current_blue);
                         break;
                     default://do nothing
                         break;
                 }
             }
        }

        SDL_SetRenderDrawColor(Renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(Renderer);
        
        SetGameBlock(0, 0, 255, 0, 0);
        int test = ContainsBlock(0, 0);
        if(! test){
            printf("Contains Block error at 0,0\n");
        }
        SetGameBlock(GAME_WIDTH-1, GAME_HEIGHT-1, 0, 255, 0);
        
        SetGameBlock(0, GAME_HEIGHT-1, 0, 0, 255);
        SetGameBlock(GAME_WIDTH-1, 0, 0, 50, 50);
        SetGameBlock(GAME_WIDTH / 2,
                     GAME_HEIGHT / 2,
                     0, 50, 50);

        removePiece(1, 1, ItetriminoUp);
        
        if(placePiece(1, 1, ItetriminoUp, 255, 0, 0) == 0) {
            printf("error placing I up, failed when should succeed\n");
        }
        
        if(placePiece(0, 0, ItetriminoUp, 255, 0, 0) != 0) {
            printf("error placing I up, succeed when should fail\n");
        }

        removePiece(3, 2, ItetriminoSide);
        if(placePiece(3, 2, ItetriminoSide, 255, 0, 0) == 0) {
            printf("error placing I side, fail when should succeed\n");
        }

        placePiece(current_x, current_y, CurrentPiece, 0, 255, 0);
        
        DrawGameBlocks();

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
