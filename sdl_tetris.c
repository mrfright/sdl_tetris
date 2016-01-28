#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

//for the random function
#include <time.h>
#include <stdlib.h>

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
  [][][][]  []
            []
            []
            []
 */
const int ItetriminoSide[][2] = {{0, 0}, {1, 0}, {2, 0}, {3, 0}};
const int ItetriminoUp[][2] = {{0, 0}, {0, 1}, {0, 2}, {0, 3}};

/*
    []  []      [][]  [][][]
    []  [][][]  []        []
  [][]          []
 */
const int JtetriminoUp[][2] = {{1, 0}, {1, 1}, {0, 2}, {1, 2}};
const int JtetriminoRight[][2] = {{0, 0}, {0, 1}, {1, 1}, {2, 1}};
const int JtetriminoDown[][2] = {{0, 0}, {1, 0}, {0, 1}, {0, 2}};
const int JtetriminoLeft[][2] = {{0, 0}, {1, 0}, {2, 0}, {2, 1}};

/*
  []    [][][]  [][]      []
  []    []        []  [][][]
  [][]            []
 */
const int LtetriminoUp[][2] = {{0, 0}, {0, 1}, {0, 2}, {1, 2}};
const int LtetriminoRight[][2] = {{0, 0}, {1, 0}, {2, 0}, {0, 1}};
const int LtetriminoDown[][2] = {{0, 0}, {1, 0}, {1, 1}, {1, 2}};
const int LtetriminoLeft[][2] = {{2, 0}, {0, 1}, {1, 1}, {2, 1}};

/*
  [][]
  [][]
 */
const int Otetrimino[][2] = {{0, 0}, {1, 0}, {0, 1}, {1, 1}};

/*
    [][]  []
  [][]    [][]
            []
 */
const int StetriminoSide[][2] = {{1, 0}, {2, 0}, {0, 1}, {1, 1}};
const int StetriminoUp[][2] = {{0, 0}, {0, 1}, {1, 1}, {1, 2}};

/*
  [][][]   []    []    []
    []   [][]  [][][]  [][]
           []          []
 */
const int TtetriminoUp[][2] = {{0, 0}, {1, 0}, {2, 0}, {1, 1}};
const int TtetriminoRight[][2] = {{1, 0}, {0, 1}, {1, 1}, {1, 2}};
const int TtetriminoDown[][2] = {{1, 0}, {0, 1}, {1, 1}, {2, 1}};
const int TtetriminoLeft[][2] = {{0, 0}, {0, 1}, {1, 1}, {0, 2}};

/*
  [][]      []
    [][]  [][]
          []
*/
const int ZtetriminoSide[][2] = {{0, 0}, {1, 0}, {1, 1}, {2, 1}};
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
    srand(time(NULL));
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

    const int (*CurrentPiece)[2] = NULL;
    int current_x = 5;
    int current_y = 5;
    int current_red = 0;
    int current_green = 0;
    int current_blue = 0;
    int ticks = SDL_GetTicks();

    //update ticks to current time when moved down

    //try to auto move down when ticks gets to a certain size (then update ticks back to current)
    //if can't auto move down, then block is place, move to block starting over again

    //use SDL_EnableKeyRepeat for key repeat for while holding button down
    
    while(still_playing) {
        if(CurrentPiece == NULL) {
            //make new piece

            current_x = 5;
            current_y = 0;
            ticks = SDL_GetTicks();

            //i j l s z o t
            switch(rand()%7) {
                case 0: {
                    CurrentPiece = ItetriminoUp;
                    current_red = 255;
                    current_green = 0;
                    current_blue = 0;
                } break;
                
                case 1: {
                    CurrentPiece = JtetriminoUp;
                    current_red = 0;
                    current_green = 255;
                    current_blue = 0;
                } break;
                
                case 2: {
                    CurrentPiece = LtetriminoUp;
                    current_red = 0;
                    current_green = 0;
                    current_blue = 255;
                } break;
                
                case 3: {
                    CurrentPiece = StetriminoUp;
                    current_red = 255;
                    current_green = 255;
                    current_blue = 0;
                } break;
                
                case 4: {
                    CurrentPiece = ZtetriminoUp;
                    current_red = 255;
                    current_green = 0;
                    current_blue = 255;
                } break;
                
                case 5: {
                    CurrentPiece = Otetrimino;
                    current_red = 0;
                    current_green = 255;
                    current_blue = 255;
                } break;
                
                case 6: {
                    CurrentPiece = TtetriminoUp;
                    current_red = 50;
                    current_green = 50;
                    current_blue = 50;
                } break;                                                
            }
                        
            //if can't place new piece, then game over and restart
            if(PieceContainsBlock(CurrentPiece, current_x, current_y)) {
                ClearAllGameBlocks();
            }

            //place the new piece
            placePiece(current_x, current_y, CurrentPiece,
                       current_red, current_green, current_blue);
        }
        
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT) {
                still_playing = 0;
            }
             else if( e.type == SDL_KEYDOWN ) {
                 switch( e.key.keysym.sym ) {
                     case SDLK_UP: {
                         //rotate piece - up->right->down->left->up

                         //remove piece
                         //if rotated piece can be placed then keep it
                         //if not then go back to original piece
                         //place whatever piece back

                         removePiece(current_x, current_y, CurrentPiece);
                         const int (*TempPiece)[2] = CurrentPiece;
                         
                         if(CurrentPiece == ItetriminoUp) {
                             TempPiece = ItetriminoSide;
                         }
                         else if(CurrentPiece == ItetriminoSide) {
                             TempPiece = ItetriminoUp;
                         }
                         else if(CurrentPiece == ZtetriminoUp) {
                             TempPiece = ZtetriminoSide;
                         }
                         else if(CurrentPiece == ZtetriminoSide) {
                             TempPiece = ZtetriminoUp;
                         }
                         else if(CurrentPiece == JtetriminoUp) {
                             TempPiece = JtetriminoRight;
                         }
                         else if(CurrentPiece == JtetriminoRight) {
                             TempPiece = JtetriminoDown;
                         }
                         else if(CurrentPiece == JtetriminoDown) {
                             TempPiece = JtetriminoLeft;
                         }
                         else if(CurrentPiece == JtetriminoLeft) {
                             TempPiece = JtetriminoUp;
                         }
                         else if(CurrentPiece == LtetriminoUp) {
                             TempPiece = LtetriminoRight;
                         }
                         else if(CurrentPiece == LtetriminoRight) {
                             TempPiece = LtetriminoDown;
                         }
                         else if(CurrentPiece == LtetriminoDown) {
                             TempPiece = LtetriminoLeft;
                         }
                         else if(CurrentPiece == LtetriminoLeft) {
                             TempPiece = LtetriminoUp;
                         }
                         //Otetrimino: no rotation
                         else if(CurrentPiece == StetriminoUp) {
                             TempPiece = StetriminoSide;
                         }
                         else if(CurrentPiece == StetriminoSide) {
                             TempPiece = StetriminoUp;
                         }
                         else if(CurrentPiece == TtetriminoUp) {
                             TempPiece = TtetriminoRight;
                         }
                         else if(CurrentPiece == TtetriminoRight) {
                             TempPiece = TtetriminoDown;
                         }
                         else if(CurrentPiece == TtetriminoDown) {
                             TempPiece = TtetriminoLeft;
                         }
                         else if(CurrentPiece == TtetriminoLeft) {
                             TempPiece = TtetriminoUp;
                         }
                                                  
                         if(PieceContainsBlock(TempPiece, current_x , current_y)) {
                             TempPiece = CurrentPiece;
                         }

                         CurrentPiece = TempPiece;
                         if(placePiece(current_x, current_y, TempPiece,
                                       current_red, current_green, current_blue) == 0) {
                             printf("placing rotated piece failed\n");
                         }
                         
                         break;
                     }
                     case SDLK_DOWN: {
                         removePiece(current_x, current_y, CurrentPiece);
                         if(!PieceContainsBlock(CurrentPiece, current_x, current_y+1)) {
                             ++current_y;
                             ticks = SDL_GetTicks();
                             //only update ticks if actually moved, so can't
                             //indefinitely stay at bottom if holding down button
                         }
                         
                         placePiece(current_x, current_y, CurrentPiece,
                                    current_red, current_green, current_blue);
                         
                         break;
                     }
                     case SDLK_LEFT: {
                         removePiece(current_x, current_y, CurrentPiece);
                         if(!PieceContainsBlock(CurrentPiece, current_x-1, current_y)) {
                             --current_x;
                         }
                         
                         placePiece(current_x, current_y, CurrentPiece,
                                    current_red, current_green, current_blue);
                         break;
                     }                         
                     case SDLK_RIGHT: {
                         removePiece(current_x, current_y, CurrentPiece);
                         if(!PieceContainsBlock(CurrentPiece, current_x+1, current_y)) {
                             ++current_x;
                         }
                         
                         placePiece(current_x, current_y, CurrentPiece,
                                    current_red, current_green, current_blue);
                         break;
                     }                         
                     default: {//do nothing
                     } break;
                 }
             }
        }        

        SDL_SetRenderDrawColor(Renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(Renderer);        
        
        placePiece(current_x, current_y, CurrentPiece, 0, 255, 0);

        if(SDL_GetTicks() - ticks >= 1000) {
            removePiece(current_x, current_y, CurrentPiece);
            if(!PieceContainsBlock(CurrentPiece, current_x, current_y+1)) {
                ++current_y;
                placePiece(current_x, current_y, CurrentPiece,
                       current_red, current_green, current_blue);
            }
            else {//can't auto go down, so keep piece here
                placePiece(current_x, current_y, CurrentPiece,
                       current_red, current_green, current_blue);
                CurrentPiece = NULL;

                //check for any full lines
                //start at the bottom
                //can stop when reach a row that has no blocks                
                {for(int row = GAME_HEIGHT-1; row >= 0; --row) {
                    int no_blocks = 1;
                    int full_row = 1;
                    {for(int col = 0; col < GAME_WIDTH; ++col) {
                        if(ContainsBlock(col, row)) {
                            no_blocks = 0;
                        }
                        else {
                            full_row = 0;
                        }
                    }}

                    if(no_blocks) {
                        break;
                    }
                    else if(full_row) {
                        //move what ever is above to below                                
                        //continue until you get a row that is empty
                
                        {for(int drop_row = row; drop_row >= 0; --drop_row) {
                            int no_more_blocks_to_drop = 1;
                            {for(int drop_col = 0; drop_col < GAME_WIDTH; ++drop_col) {
                                GameBlocks[drop_col][drop_row] = GameBlocks[drop_col][drop_row-1];
                                if(ContainsBlock(drop_col, drop_row-1)) {
                                    no_more_blocks_to_drop = 0;
                                }
                            }}

                            if(no_more_blocks_to_drop) {
                                break;
                            }
                        }}

                        //if a row does disappear, then need to do that row again
                        //(row++ so next way around it gets --row to be same
                        ++row;
                    }
                }}
            }            

            ticks = SDL_GetTicks();
        }        
        
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
