// easyGui.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "SDL.h"
#include "GuiMadeEasy.h"
#include "TickRateMadeEasy.h"
#include "randomNumbers.h"

int main(int argc, char* argv[])
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create an SDL window
    SDL_Window* window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Create an SDL renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    ScreenGui testGui(500,600,renderer);

    const int ticksPerSecond = 999;
    const auto tickInterval = TRME_getTickInterval(ticksPerSecond);
    int currentTick = 0;
    auto startTime = TRME_getTimePoint();

    bool running = true;
    SDL_Event event;

    while (running) {
        startTime = TRME_getTimePoint();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            // Add more event handling as needed (e.g., keyboard input, mouse clicks)
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        currentTick++;

        if (currentTick % 1 == 0) {
            SDL_Color randomColor;
            randomColor.r = random(0, 255);
            randomColor.g = random(0, 255);
            randomColor.b = random(0, 255);
            randomColor.a = 255;  // Full opacity

            testGui.createFrame(random(1, 500), random(1, 600), random(1, 100), random(1, 100), true, randomColor);
            //testGui.zindexOut();
        }

        if (currentTick % 100 == 0) {
            testGui.objectCount();
        }

        testGui.renderDescendants();
        //testGui.objectCount();
        SDL_RenderPresent(renderer);



        TRME_sleepUntilNextTick(startTime, tickInterval);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
