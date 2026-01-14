#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

// --- SETTINGS ---
const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 600;
const int GRID_SIZE = 20; // Size of one grid cell (snake part)
const int GRID_COUNT_X = SCREEN_WIDTH / GRID_SIZE;
const int GRID_COUNT_Y = SCREEN_HEIGHT / GRID_SIZE;

// --- COLORS (RGBA) ---
const SDL_Color COLOR_BLACK = {0, 0, 0, 255};
const SDL_Color COLOR_WHITE = {255, 255, 255, 255};
const SDL_Color COLOR_GREEN = {0, 255, 0, 255};
const SDL_Color COLOR_RED   = {255, 0, 0, 255};

// --- STRUCTURES ---
typedef struct {
    int x;
    int y;
} Point;

typedef enum {
    UP, DOWN, LEFT, RIGHT
} Direction;

// --- GLOBAL VARIABLES ---
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;

Point snake[400]; // Max snake length
int snakeLength = 3;
Point food;
Direction dir = RIGHT;
bool gameRunning = true;
bool gameOver = false;
int score = 0;

// --- FUNCTION PROTOTYPES ---
bool init();
void close_sdl();
void spawn_food();
void handle_input(SDL_Event* e);
void update();
void render();
void render_text(const char* text, int x, int y, SDL_Color color);
void reset_game();

int main(int argc, char* argv[]) {
    // Suppress unused parameter warnings
    (void)argc;
    (void)argv;

    if (!init()) {
        printf("Initialization failed!\n");
        return 1;
    }

    reset_game();

    SDL_Event e;
    while (gameRunning) {
        // 1. Event Polling
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                gameRunning = false;
            }
            handle_input(&e);
        }

        // 2. Update Game Logic
        if (!gameOver) {
            update();
        }

        // 3. Render
        render();

        // 4. Frame Delay (Controls Game Speed)
        SDL_Delay(100); 
    }

    close_sdl();
    return 0;
}

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL Error: %s\n", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow("Snake Game - C & SDL2",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (window == NULL) {
        printf("Window Error: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer Error: %s\n", SDL_GetError());
        return false;
    }

    if (TTF_Init() == -1) {
        printf("TTF Error: %s\n", TTF_GetError());
        return false;
    }

    // Load font (Make sure arial.ttf is in the same directory)
    font = TTF_OpenFont("arial.ttf", 24);
    if (font == NULL) {
        printf("Failed to load font! Check arial.ttf. Error: %s\n", TTF_GetError());
        // Continuing without font (text won't render)
    }

    srand(time(NULL)); // Seed random number generator
    return true;
}

void reset_game() {
    snakeLength = 3;
    // Start in the middle
    snake[0].x = GRID_COUNT_X / 2;
    snake[0].y = GRID_COUNT_Y / 2;
    snake[1].x = snake[0].x - 1; snake[1].y = snake[0].y;
    snake[2].x = snake[0].x - 2; snake[2].y = snake[0].y;
    
    dir = RIGHT;
    score = 0;
    gameOver = false;
    spawn_food();
}

void spawn_food() {
    bool onSnake;
    do {
        onSnake = false;
        food.x = rand() % GRID_COUNT_X;
        food.y = rand() % GRID_COUNT_Y;

        // Ensure food doesn't spawn on the snake body
        for (int i = 0; i < snakeLength; i++) {
            if (snake[i].x == food.x && snake[i].y == food.y) {
                onSnake = true;
                break;
            }
        }
    } while (onSnake);
}

void handle_input(SDL_Event* e) {
    if (e->type == SDL_KEYDOWN) {
        switch (e->key.keysym.sym) {
            case SDLK_UP:    if (dir != DOWN) dir = UP; break;
            case SDLK_DOWN:  if (dir != UP) dir = DOWN; break;
            case SDLK_LEFT:  if (dir != RIGHT) dir = LEFT; break;
            case SDLK_RIGHT: if (dir != LEFT) dir = RIGHT; break;
            case SDLK_r:     if (gameOver) reset_game(); break; // Restart
            case SDLK_ESCAPE: gameRunning = false; break;
        }
    }
}

void update() {
    // 1. Move body (shift segments from tail to head)
    for (int i = snakeLength - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }

    // 2. Move head
    switch (dir) {
        case UP:    snake[0].y--; break;
        case DOWN:  snake[0].y++; break;
        case LEFT:  snake[0].x--; break;
        case RIGHT: snake[0].x++; break;
    }

    // 3. Wall Collision
    if (snake[0].x < 0 || snake[0].x >= GRID_COUNT_X ||
        snake[0].y < 0 || snake[0].y >= GRID_COUNT_Y) {
        gameOver = true;
    }

    // 4. Self Collision
    for (int i = 1; i < snakeLength; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            gameOver = true;
        }
    }

    // 5. Eat Food
    if (snake[0].x == food.x && snake[0].y == food.y) {
        score += 10;
        snakeLength++;
        spawn_food();
    }
}

void render() {
    // Clear background (Black)
    SDL_SetRenderDrawColor(renderer, COLOR_BLACK.r, COLOR_BLACK.g, COLOR_BLACK.b, COLOR_BLACK.a);
    SDL_RenderClear(renderer);

    // Draw Food (Red)
    SDL_SetRenderDrawColor(renderer, COLOR_RED.r, COLOR_RED.g, COLOR_RED.b, COLOR_RED.a);
    SDL_Rect foodRect = { food.x * GRID_SIZE, food.y * GRID_SIZE, GRID_SIZE, GRID_SIZE };
    SDL_RenderFillRect(renderer, &foodRect);

    // Draw Snake (Green)
    SDL_SetRenderDrawColor(renderer, COLOR_GREEN.r, COLOR_GREEN.g, COLOR_GREEN.b, COLOR_GREEN.a);
    for (int i = 0; i < snakeLength; i++) {
        SDL_Rect snakeRect = { snake[i].x * GRID_SIZE, snake[i].y * GRID_SIZE, GRID_SIZE, GRID_SIZE };
        SDL_RenderFillRect(renderer, &snakeRect);
        
        // Optional: Draw outline for segments
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &snakeRect);
        SDL_SetRenderDrawColor(renderer, COLOR_GREEN.r, COLOR_GREEN.g, COLOR_GREEN.b, COLOR_GREEN.a);
    }

    // Score Text
    char scoreText[32];
    sprintf(scoreText, "Score: %d", score);
    render_text(scoreText, 10, 10, COLOR_WHITE);

    // Game Over Text
    if (gameOver) {
        render_text("GAME OVER! Press 'R' to Restart", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2, COLOR_WHITE);
    }

    SDL_RenderPresent(renderer);
}

void render_text(const char* text, int x, int y, SDL_Color color) {
    if (font == NULL) return;

    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    if (surface == NULL) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dstRect = { x, y, surface->w, surface->h };

    SDL_RenderCopy(renderer, texture, NULL, &dstRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void close_sdl() {
    if (font) TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}