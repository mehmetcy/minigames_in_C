#include <SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <SDL_ttf.h>

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600
#define CELL_SIZE (WINDOW_WIDTH / 3)

typedef enum { EMPTY, X, O } Cell;
typedef enum { MENU, PLAYING, GAME_OVER } GameState;

Cell board[3][3];
GameState game_state = MENU;
bool x_turn = true;
bool game_over = false;
Cell winner = EMPTY;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;
SDL_Surface* surface = NULL;

// Global start button rect for menu
SDL_Rect start_button_rect = { WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 30, 200, 60 };

void draw_board() 
{
    // Clear screen with white
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    // Draw grid lines
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int i = 1; i < 3; ++i) 
    {
        SDL_RenderDrawLine(renderer, i * CELL_SIZE, 0, i * CELL_SIZE, WINDOW_HEIGHT);
        SDL_RenderDrawLine(renderer, 0, i * CELL_SIZE, WINDOW_WIDTH, i * CELL_SIZE);
    }

    // Draw Xs and Os
    for (int row = 0; row < 3; ++row) 
    {
        for (int col = 0; col < 3; ++col) 
        {
            int cx = col * CELL_SIZE + CELL_SIZE / 2;
            int cy = row * CELL_SIZE + CELL_SIZE / 2;

            if (board[row][col] == X) 
            {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_RenderDrawLine(renderer, cx - 50, cy - 50, cx + 50, cy + 50);
                SDL_RenderDrawLine(renderer, cx - 50, cy + 50, cx + 50, cy - 50);
            }
            else if (board[row][col] == O) 
            {
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                for (int r = 45; r < 50; r++) {
                    for (int angle = 0; angle < 360; angle++) {
                        int x = cx + r * cos(angle * M_PI / 180);
                        int y = cy + r * sin(angle * M_PI / 180);
                        SDL_RenderDrawPoint(renderer, x, y);
                    }
                }
            }
        }
    }

    if (game_over && winner != EMPTY) 
    {
        // Draw yellow banner
        SDL_Rect banner = { 0, WINDOW_HEIGHT / 2 - 30, WINDOW_WIDTH, 60 };
        SDL_SetRenderDrawColor(renderer, 200, 200, 0, 255);
        SDL_RenderFillRect(renderer, &banner);

        // Draw the winner symbol in the center of the banner
        int centerX = WINDOW_WIDTH / 2;
        int centerY = WINDOW_HEIGHT / 2;

        if (winner == X) 
        {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red

            for (int offset = -2; offset <= 2; ++offset) {
                SDL_RenderDrawLine(renderer, centerX - 30 + offset, centerY - 30, centerX + 30 + offset, centerY + 30);
                SDL_RenderDrawLine(renderer, centerX - 30 + offset, centerY + 30, centerX + 30 + offset, centerY - 30);
            }
        }
        else if (winner == O) 
        {
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue
            for (int r = 25; r < 30; r++) {
                for (int angle = 0; angle < 360; angle++) {
                    int x = centerX + r * cos(angle * M_PI / 180);
                    int y = centerY + r * sin(angle * M_PI / 180);
                    SDL_RenderDrawPoint(renderer, x, y);
                }
            }
        }
    }

    SDL_RenderPresent(renderer);
}

void draw_menu() 
{
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255); // Dark gray background
    SDL_RenderClear(renderer);

    // Draw the red Start Game button rectangle
    SDL_Rect startRect = { WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 30, 200, 60 };
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red
    SDL_RenderFillRect(renderer, &startRect);

    // Draw a white play icon (triangle) inside the button
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White
    SDL_Point triangle[4];
    triangle[0].x = startRect.x + 50;
    triangle[0].y = startRect.y + 15;

    triangle[1].x = startRect.x + 50;
    triangle[1].y = startRect.y + 45;

    triangle[2].x = startRect.x + 90;
    triangle[2].y = startRect.y + 30;

    triangle[3] = triangle[0]; // close the triangle

    SDL_RenderDrawLines(renderer, triangle, 4);
    SDL_RenderPresent(renderer);
}

Cell check_winner() 
{
    for (int i = 0; i < 3; ++i) 
    {
        if (board[i][0] != EMPTY &&
            board[i][0] == board[i][1] &&
            board[i][1] == board[i][2])
            return board[i][0];

        if (board[0][i] != EMPTY &&
            board[0][i] == board[1][i] &&
            board[1][i] == board[2][i])
            return board[0][i];
    }

    if (board[0][0] != EMPTY &&
        board[0][0] == board[1][1] &&
        board[1][1] == board[2][2])
        return board[0][0];

    if (board[0][2] != EMPTY &&
        board[0][2] == board[1][1] &&
        board[1][1] == board[2][0])
        return board[0][2];

    return EMPTY;
}

bool is_board_full() 
{
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c)
            if (board[r][c] == EMPTY)
                return false;
    return true;
}

void reset_game() 
{
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c)
            board[r][c] = EMPTY;

    x_turn = true;
    game_over = false;
}

int main(int argc, char* argv[]) 
{
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    window = SDL_CreateWindow("Tic Tac Toe", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    reset_game();

    bool running = true;
    SDL_Event event;

    while (running) 
    {
        while (SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_QUIT) 
            {
                running = false;
            }

            switch (game_state) 
            {
                case MENU:
                    if (event.type == SDL_MOUSEBUTTONDOWN) 
                    {
                        int mx = event.button.x;
                        int my = event.button.y;

                        SDL_Rect startRect = { WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 30, 200, 60 };
                        if (mx >= startRect.x && mx <= startRect.x + startRect.w &&
                            my >= startRect.y && my <= startRect.y + startRect.h) 
                        {
                            reset_game();
                            game_state = PLAYING;
                        }
                    }
                    else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) 
                    {
                        running = false;
                    }
                    break;

                case PLAYING:
                    if (event.type == SDL_MOUSEBUTTONDOWN && !game_over) 
                    {
                        int x = event.button.x / CELL_SIZE;
                        int y = event.button.y / CELL_SIZE;

                        if (board[y][x] == EMPTY) 
                        {
                            board[y][x] = x_turn ? X : O;
                            x_turn = !x_turn;

                            winner = check_winner();
                            if (winner != EMPTY || is_board_full()) 
                            {
                                game_state = GAME_OVER;
                                game_over = true;
                            }
                        }
                    } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) 
                    {
                        game_state = MENU;
                    }
                    break;

                case GAME_OVER:
                    if (event.type == SDL_KEYDOWN) 
                    {
                        if (event.key.keysym.sym == SDLK_r) 
                        {
                            reset_game();
                            game_state = PLAYING;
                        } else if (event.key.keysym.sym == SDLK_ESCAPE) 
                        {
                            game_state = MENU;
                        }
                    }
                    break;
            }
        }

        switch (game_state) 
        {
            case MENU:
                draw_menu();
                break;
            case PLAYING:
            case GAME_OVER:
                draw_board();
                break;
        }

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    TTF_Quit();
    return 0;
}
