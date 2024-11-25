#include <SDL.h>
#include <SDL_ttf.h> 
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <map>
#include <iostream>
using namespace std;

// Constants
const int SCREEN_WIDTH = 840;
const int SCREEN_HEIGHT = 480;
const int TILE_SIZE = 30;
const int GRID_WIDTH = SCREEN_WIDTH / TILE_SIZE;
const int GRID_HEIGHT = SCREEN_HEIGHT / TILE_SIZE;

map<string, pair<int, int>> direction = { {"UP", {0, -1}}, {"DOWN", {0, 1}}, {"LEFT", {-1, 0}}, {"RIGHT", {1, 0}} };

void drawText(SDL_Renderer* renderer, TTF_Font* font, string& text, int x, int y, SDL_Color color) {

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);

    if (!textSurface) {
        cerr << "Failed to create text surface: " << TTF_GetError() << endl;
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    if (!textTexture) {
        cerr << "Failed to create text texture: " << SDL_GetError() << endl;
        SDL_FreeSurface(textSurface);
        return;
    }
    SDL_Rect textRect = { x, y, textSurface->w, textSurface->h };
    //SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    if (SDL_RenderCopy(renderer, textTexture, NULL, &textRect) != 0) {
        cerr << "Failed to render text: " << SDL_GetError() << endl;
    }

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}
int main(int argc, char* argv[]) {

    // Initialize SDL and SDL_ttf

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "Failed to initialize SDL: " << SDL_GetError() << endl;
        return -1;
    }
    if (TTF_Init() == -1) {
        cerr << "Failed to initialize TTF: " << TTF_GetError() << endl;
        SDL_Quit();
        return -1;
    }
    
    SDL_Window* window = SDL_CreateWindow("Snake Game by Nabila", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    
    if (!window) {
        cerr << "Failed to create window: " << SDL_GetError() << endl;
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer) {
        cerr << "Failed to create renderer: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }
    // Load font
    TTF_Font* font = TTF_OpenFont("Roboto-Black.ttf", 24);

    if (!font) {
        cerr << "Failed to load font: " << TTF_GetError() << endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    srand(time(0));

    vector<pair<int, int>> snake;

    snake.push_back({ GRID_WIDTH / 2, GRID_HEIGHT / 2 });

    string currentDirection = "RIGHT";
    //food variables
    int foodX = rand() % GRID_WIDTH;

    int foodY = rand() % GRID_HEIGHT;
    //bonus food variables
    int bonusfoodx = -1;

    int bonusfoody = -1;

    bool running = true;

    bool bonuspresent = false;

    int bonustimer = 0;

    const int bonus_interval = 20;

    const int bonus_duration = 40;
    //speed variables
    int current_speed = 150;

    const int max_speed = 50;

    SDL_Event event;
    //scores variable
    int score = 0;

    int high_score = 0;

    //building obstacle logic

    vector<vector<pair<int, int>>> obstacles;

    int no_of_obstacles = 4;

    int max_length = 10;

    for (int i = 0; i < no_of_obstacles; ++i) {
        int strt_x = rand() % GRID_WIDTH;
        int strt_y = rand() % GRID_HEIGHT;
        int brick_lenght = 2 + rand() % (max_length - 1);
        bool is_vertical = rand() % 2;
        vector <pair<int, int>>seg;
        for (int j = 0; j < brick_lenght; ++j)
        {
            int seg_x = strt_x + j;
            int seg_y = strt_y + j;
            if (is_vertical && seg_y < GRID_HEIGHT)
            {
                seg.push_back({ strt_x,seg_y });
            }
            else if (!is_vertical && seg_x < GRID_WIDTH)
            {
                seg.push_back({ seg_x,strt_y });
            }
        }
        obstacles.push_back(seg);
    }

    // Game loop

    while (running) {

        // Handle events

        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {

                running = false;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_UP && currentDirection != "DOWN")
                    currentDirection = "UP";
                else if (event.key.keysym.sym == SDLK_DOWN && currentDirection != "UP")
                    currentDirection = "DOWN";
                else if (event.key.keysym.sym == SDLK_LEFT && currentDirection != "RIGHT")
                    currentDirection = "LEFT";
                else if (event.key.keysym.sym == SDLK_RIGHT && currentDirection != "LEFT")
                    currentDirection = "RIGHT";
            }
        }
        if (score > high_score)
        {
            high_score = score;
        }
        // Move snake
        pair<int, int> new_head = snake.front();
        new_head.first += direction[currentDirection].first;
        new_head.second += direction[currentDirection].second;
        //if the snake can travel through boundaries
        if (new_head.first < 0)//crosses left boundary
            new_head.first = GRID_WIDTH - 1;
        else if (new_head.first >= GRID_WIDTH)//crosses right boundary
            new_head.first = 0;
        else if (new_head.second < 0)//crosses upward boundary
            new_head.second = GRID_HEIGHT - 1;
        else if (new_head.second >= GRID_HEIGHT)
            new_head.second = 0;
        snake.insert(snake.begin(), new_head);

        //check if its the time to generate bonus food
        if (score % bonus_interval == 0 && score > 0 && !bonuspresent) {
            bonusfoodx = rand() % GRID_WIDTH;
            bonusfoody = rand() % GRID_HEIGHT;
            bonuspresent = true;
            bonustimer = bonus_duration; // Reset timer for bonus food visibility
        }
        //bonus food duration
        if (bonuspresent)
        {
            bonustimer-=2;
            if (bonustimer == 0)
            {
                bonusfoodx = -1;
                bonusfoody = -1;
                bonuspresent = false;
            }
        }
        //consumption of bonus food
        if (new_head.first == bonusfoodx && new_head.second == bonusfoody) {
            score += 10;
            bonuspresent = false;
            bonusfoodx = -1;
            bonusfoody = -1;
            current_speed = max(current_speed - 5, max_speed);
        }

        // Food consumption
        if (new_head.first == foodX && new_head.second == foodY) {
            score += 5;
            foodX = rand() % GRID_WIDTH;
            foodY = rand() % GRID_HEIGHT;
            current_speed = max(current_speed - 2, max_speed);
        }

        else {
            snake.pop_back();
        }

        // Collision detection
        /*if (new_head.first < 0 || new_head.second < 0 || new_head.first >= GRID_WIDTH || new_head.second >= GRID_HEIGHT) {
            running = false;*/
            //}
        for (int i = 1; i < snake.size(); ++i) {
            if (new_head == snake[i]) {
                running = false;
            }
        }
        for (auto obstacle : obstacles) {
            for (auto tile : obstacle)
            {
                if (new_head == tile)
                {
                    running = false;
                }
            }
        }
        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 60, 255);
        SDL_RenderClear(renderer);

        // Draw food (circular food)
        int centerX = foodX * TILE_SIZE + TILE_SIZE / 2;
        int centerY = foodY * TILE_SIZE + TILE_SIZE / 2;
        int radius = TILE_SIZE / 3;
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Red color
        for (int w = 0; w < radius * 2; w++) {
            for (int h = 0; h < radius * 2; h++) {
                int dx = radius - w;
                int dy = radius - h;
                if ((dx * dx + dy * dy) <= (radius * radius)) {
                    SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
                }
            }
        }
        //Draw bonus food
        if (bonuspresent) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Blue color
            int bonusCenterX = bonusfoodx * TILE_SIZE + TILE_SIZE / 2;
            int bonusCenterY = bonusfoody * TILE_SIZE + TILE_SIZE / 2;
            int bonusradius = TILE_SIZE / 2;
            for (int w = 0; w < bonusradius * 2; w++) {
                for (int h = 0; h < bonusradius * 2; h++) {
                    int dx = bonusradius - w;
                    int dy = bonusradius - h;
                    if ((dx * dx + dy * dy) <= (bonusradius * bonusradius)) {
                        SDL_RenderDrawPoint(renderer, bonusCenterX + dx, bonusCenterY + dy);
                    }
                }
            }
        }
        // Draw obstacles
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Brown color for walls/bricks
        for (auto& obstacle : obstacles) {
            for (auto& segment : obstacle) {
                SDL_Rect obstacleRect = { segment.first * TILE_SIZE, segment.second * TILE_SIZE, TILE_SIZE, TILE_SIZE };
                SDL_RenderFillRect(renderer, &obstacleRect);
            }
        }
        // Draw snake
        for (int i = 0; i < snake.size(); ++i) {
            pair<int, int> segment = snake[i];
            SDL_Rect segmentRect = { segment.first * TILE_SIZE, segment.second * TILE_SIZE, TILE_SIZE, TILE_SIZE };

            // Head: Bright color
            if (i == 0) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow
            }
            // Body: Alternating shades
            else if (i % 2 == 0) {
                SDL_SetRenderDrawColor(renderer, 0, 200, 0, 200); // Dark green
            }
            else {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Light green
            }

            SDL_RenderFillRect(renderer, &segmentRect);
        }

        // Draw score
        SDL_Color white = { 255, 255, 255, 255 };
        string score_txt = "Score: " + to_string(score);
        drawText(renderer, font, score_txt, 350, 20, white);
        SDL_RenderPresent(renderer);
        SDL_Delay(current_speed);

    }
    // Display Game Over/Final Score/New High Score Screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background

    SDL_RenderClear(renderer);

    SDL_Color red = { 255, 0, 0, 255 };

    string exit_txt = "GAME OVER!!!";

    string finalscore_txt = "Final Score : " + to_string(score);

    string newhigh_score_txt = "New High Score: " + to_string(high_score);

    drawText(renderer, font, exit_txt, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 80, red);
   
    drawText(renderer, font, finalscore_txt, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 30, red);
    
    drawText(renderer, font, newhigh_score_txt, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 20, red);
    
    SDL_RenderPresent(renderer);

    SDL_Delay(3000); 

    // Clean up
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
