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
int x_score = 0;
int o_score = 0;

// Button hover variables
bool playAgainHover = false;
bool backToMenuHover = false;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;
SDL_Surface* surface = NULL;

// Global buttons rect for menu
SDL_Rect start_button_rect = { WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 30, 200, 60 };
SDL_Rect playAgainButton = { WINDOW_WIDTH / 2 - 100, 350, 200, 50 };
SDL_Rect backToMenuButton = { WINDOW_WIDTH / 2 - 100, 420, 200, 50 };

void draw_board() 
{
    // Clear screen with white
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    //////////////////////// Draw score display at top /////////////////////////////////
    TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
    if (font)
    {
        char score_text[64];
        snprintf(score_text, sizeof(score_text), "X: %d    O: %d", x_score, o_score);

        SDL_Color color = { 0, 0, 0 };
        SDL_Surface* surface = TTF_RenderText_Solid(font, score_text, color);

        if (surface)
        {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect rect = { 10, 10, surface->w, surface->h }; // top-left
            SDL_RenderCopy(renderer, texture, NULL, &rect);
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surface);
        }
        TTF_CloseFont(font);
    }
    //////////////////////////////////////////////////////////////

    // draw grid lines
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

    if (game_over) 
    {
        // Draw yellow banner
        SDL_Rect banner = { 0, WINDOW_HEIGHT / 2 - 30, WINDOW_WIDTH, 60 };
        SDL_SetRenderDrawColor(renderer, 200, 200, 0, 255);
        SDL_RenderFillRect(renderer, &banner);

        // Text logic
        TTF_Font* font = TTF_OpenFont("arial.ttf", 36);

        if (font) 
        {
            const char* message = "";
            SDL_Color color = { 0, 0, 0 };

            if (winner == X) 
            {
                message = "X Wins!";
                color = (SDL_Color){ 255, 0, 0 }; // red
            }

            else if (winner == O) 
            {
                message = "O Wins!";
                color = (SDL_Color){ 0, 0, 255 }; // blue
            }

            else 
            {
                message = "Draw!";
                color = (SDL_Color){ 0, 0, 0 }; // black
            }

            SDL_Surface* text_surface = TTF_RenderText_Solid(font, message, color);

            if (text_surface) 
            {
                SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
                int text_w = 0, text_h = 0;
                SDL_QueryTexture(text_texture, NULL, NULL, &text_w, &text_h);
                SDL_Rect text_rect = { (WINDOW_WIDTH - text_w) / 2, (WINDOW_HEIGHT - text_h) / 2, text_w, text_h };
                SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
                SDL_DestroyTexture(text_texture);
                SDL_FreeSurface(text_surface);
            }
            TTF_CloseFont(font);
        }

        // hover movement color change
        if (playAgainHover)
            SDL_SetRenderDrawColor(renderer, 180, 255, 180, 255); // green
        else
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // gray
        SDL_RenderFillRect(renderer, &playAgainButton);

        if (backToMenuHover)
            SDL_SetRenderDrawColor(renderer, 255, 200, 200, 255); //light red
        else
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); //gray
        SDL_RenderFillRect(renderer, &backToMenuButton);

        // button words
        TTF_Font* font2 = TTF_OpenFont("arial.ttf", 24);
        if (font2)
        {
            SDL_Color textColor = { 0, 0, 0 };
            SDL_Surface* playSurface = TTF_RenderText_Solid(font2, "Play Again", textColor);
            SDL_Surface* menuSurface = TTF_RenderText_Solid(font2, "Back to Menu", textColor);

            if (playSurface && menuSurface)
            {
                SDL_Texture* playTex = SDL_CreateTextureFromSurface(renderer, playSurface);
                SDL_Texture* menuTex = SDL_CreateTextureFromSurface(renderer, menuSurface);

                SDL_Rect playTextRect = { playAgainButton.x + 30, playAgainButton.y + 10, playSurface->w, playSurface->h };
                SDL_Rect menuTextRect = { backToMenuButton.x + 20, backToMenuButton.y + 10, menuSurface->w, menuSurface->h };

                SDL_RenderCopy(renderer, playTex, NULL, &playTextRect);
                SDL_RenderCopy(renderer, menuTex, NULL, &menuTextRect);

                SDL_FreeSurface(playSurface);
                SDL_FreeSurface(menuSurface);
                SDL_DestroyTexture(playTex);
                SDL_DestroyTexture(menuTex);
            }
            TTF_CloseFont(font2);
        }

    }
    else if (game_over && winner == EMPTY)
    {
        // Draw yellow banner for draw
        SDL_Rect banner = { 0, WINDOW_HEIGHT / 2 - 30, WINDOW_WIDTH, 60 };
        SDL_SetRenderDrawColor(renderer, 200, 200, 0, 255);
        SDL_RenderFillRect(renderer, &banner);
        // Render "Draw!" text in the center
        TTF_Font* font = TTF_OpenFont("arial.ttf", 36);

        if (font) 
        {
            SDL_Color color = { 0, 0, 0 }; // black text
            SDL_Surface* text_surface = TTF_RenderText_Solid(font, "Draw!", color);

            if (text_surface) 
            {
                SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
                int text_w = 0, text_h = 0;
                SDL_QueryTexture(text_texture, NULL, NULL, &text_w, &text_h);
                SDL_Rect text_rect = { (WINDOW_WIDTH - text_w) / 2, (WINDOW_HEIGHT - text_h) / 2, text_w, text_h };
                SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
                SDL_DestroyTexture(text_texture);
                SDL_FreeSurface(text_surface);
            }
            TTF_CloseFont(font);
        }
    }


    //  Add play again and back to menu buttons if game over
    if (game_over) 
    {
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // Button background
        SDL_RenderFillRect(renderer, &playAgainButton);
        SDL_RenderFillRect(renderer, &backToMenuButton);

        TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
        if (font) 
        {
            SDL_Color textColor = { 0, 0, 0 };
            SDL_Surface* playSurface = TTF_RenderText_Solid(font, "Play Again", textColor);
            SDL_Surface* menuSurface = TTF_RenderText_Solid(font, "Back to Menu", textColor);

            if (playSurface && menuSurface) 
            {
                SDL_Texture* playTex = SDL_CreateTextureFromSurface(renderer, playSurface);
                SDL_Texture* menuTex = SDL_CreateTextureFromSurface(renderer, menuSurface);

                SDL_Rect playTextRect = { playAgainButton.x + 30, playAgainButton.y + 10, playSurface->w, playSurface->h };
                SDL_Rect menuTextRect = { backToMenuButton.x + 20, backToMenuButton.y + 10, menuSurface->w, menuSurface->h };

                SDL_RenderCopy(renderer, playTex, NULL, &playTextRect);
                SDL_RenderCopy(renderer, menuTex, NULL, &menuTextRect);

                SDL_FreeSurface(playSurface);
                SDL_FreeSurface(menuSurface);
                SDL_DestroyTexture(playTex);
                SDL_DestroyTexture(menuTex);
            }
            TTF_CloseFont(font);
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
                            if (winner == X)
                            {
                                x_score++;
                            }
                            else if (winner == O)
                            {
                                o_score++;
                            }
                        }
                    } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) 
                    {
                        game_state = MENU;
                    }
                    break;

                case GAME_OVER:
                    if (event.type == SDL_MOUSEMOTION)
                    {
                        int mx = event.motion.x;
                        int my = event.motion.y;

                        playAgainHover = (mx >= playAgainButton.x && mx <= playAgainButton.x + playAgainButton.w &&
                            my >= playAgainButton.y && my <= playAgainButton.y + playAgainButton.h);

                        backToMenuHover = (mx >= backToMenuButton.x && mx <= backToMenuButton.x + backToMenuButton.w &&
                            my >= backToMenuButton.y && my <= backToMenuButton.y + backToMenuButton.h);
                    }

                    else if (event.type == SDL_MOUSEBUTTONDOWN) 
                    {
                        int mx = event.button.x;
                        int my = event.button.y;

                        if (mx >= playAgainButton.x && mx <= playAgainButton.x + playAgainButton.w &&
                            my >= playAgainButton.y && my <= playAgainButton.y + playAgainButton.h) 
                        {
                            reset_game();
                            game_state = PLAYING;
                        }
                        else if (mx >= backToMenuButton.x && mx <= backToMenuButton.x + backToMenuButton.w &&
                            my >= backToMenuButton.y && my <= backToMenuButton.y + backToMenuButton.h) 
                        {
                            reset_game();
                            game_state = MENU;
                        }
                    }
                    else if (event.type == SDL_KEYDOWN) 
                    {
                        if (event.key.keysym.sym == SDLK_r) 
                        {
                            reset_game();
                            game_state = PLAYING;
                        }
                        else if (event.key.keysym.sym == SDLK_ESCAPE) 
                        {
                            reset_game();
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

        //draw_board();                  
        //SDL_RenderPresent(renderer);   

    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    TTF_Quit();
    return 0;
}
