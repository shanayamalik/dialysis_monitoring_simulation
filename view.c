#include "dialysis_sim.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// Window dimension constants
const int SCREEN_WIDTH = 350;
const int SCREEN_HEIGHT = 380;

SDL_Color textColor = {0x00, 0x00, 0x00, 0xFF}; // black; also used for graphs
SDL_Color bgColor = {0x9C, 0xE8, 0x23, 0xFF}; // light yellow-green

SDL_Window* gWindow = NULL; // The window to render
SDL_Renderer* gRenderer = NULL; // The renderer for the window
TTF_Font *gFont = NULL; // The display font

bool initSDL() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    // Initialize fonts
    if (TTF_Init() != 0)
    {
        printf("SDL_ttf could not initialize! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    // Create window
    gWindow = SDL_CreateWindow("Dialysis Simulation", SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == NULL) {
        printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
        return false;
    }
    
    // Create renderer for window
    gRenderer = SDL_CreateRenderer(gWindow, -1, 0);
    if (gRenderer == NULL) {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    // Open the font
    gFont = TTF_OpenFont("bits.ttf", 20); // LCD display fixed-pitch font
    if (gFont == NULL) {
        printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        return false;
    }

    return true; // success
}

void closeSDL() {
    // Destroy window, close fonts, and stop SDL
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;
    TTF_CloseFont(gFont);
    SDL_Quit();
}


// how many data points to store
#define NUM_DAT 1000

void plotGraph(SDL_Rect bar, float* data, int seq, float maxDataValue, float minDataValue) {
    int graphHeight = bar.h;
    int graphWidth = bar.w;
    float d; // datum

    // Calculate scale factors
    float scaleY = graphHeight / (maxDataValue - minDataValue);

    // Plot the data points
    for (int i = 0; i < graphWidth / 2; i++) {
        if (seq < graphWidth) {
            d = data[i*2 % NUM_DAT];
        } else {
            d = data[(i*2 + seq - graphWidth) % NUM_DAT];
        }
        if (d != 0) { // Assuming 0 is uninitialized data
            if (d > maxDataValue) d = maxDataValue;
            if (d < minDataValue) d = minDataValue;
            int x = bar.x + (int)i*2;
            int y = bar.y + graphHeight - (int)((d - minDataValue) * scaleY);
            SDL_RenderDrawPoint(gRenderer, x, y);
        }
    }
}

void updateView(int sequence, float bp, float flowRate, float o2Saturation,
        bool leak, float ufAdj, float rateAdj, float o2Adj) {

    // storage for historic variables
    static float bps[NUM_DAT], frs[NUM_DAT], o2s[NUM_DAT]; // initially zeros
    bps[sequence % NUM_DAT] = bp;
    frs[sequence % NUM_DAT] = flowRate;
    o2s[sequence % NUM_DAT] = o2Saturation;
    
    // Clear screen
    SDL_SetRenderDrawColor(gRenderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderClear(gRenderer);

    // Define the starting position for the text
    int x = 10;
    int y = 10;
    int lineHeight = 0; // Will be set based on text height if 0

    // Prepare the text for display
    char text[1000]; // character buffer
    int mins = sequence / (5 * 60); // minutes
    float secs = (sequence % (5 * 60)) / 5.0; // seconds
    sprintf(text, "Elapsed time:%3d min %04.1f sec\n \n \n \n"
                  "Blood pressure: %6.0f mmHg\n"
                  "Ultrafilt. adj.: %+5.1f mL/h\n \n \n \n"
                  "Blood flow rate: %5.1f mL/min\n"
                  "Pump adjustment: %+5.1f mL/min\n \n \n \n"
                  "Blood O2 sat.:     %3.0f %%\n"
                  "O2 adjustment:   %+5.1f L/min\n \n"
                  "Status: %s",
        mins, secs, bp, ufAdj, flowRate, rateAdj, o2Saturation, o2Adj,
        leak ? "LEAK DETECTED, halted" : "       Nominal");
    
    char* line = strtok(text, "\n"); // parse the string into lines

    while (line != NULL) {
        if (strlen(line) > 1) { // don't render blank lines
            // Render text to a surface and texture
            SDL_Surface* ts = TTF_RenderText_Solid(gFont, line, textColor);
            SDL_Texture* tt = SDL_CreateTextureFromSurface(gRenderer, ts);
            
            // Render the texture
            SDL_Rect renderQuad = {x, y, ts->w, ts->h};
            SDL_RenderCopy(gRenderer, tt, NULL, &renderQuad);
            
            if (lineHeight == 0) lineHeight = ts->h; // set line height

            // Free texture and surfact
            SDL_DestroyTexture(tt);
            SDL_FreeSurface(ts);
        }
        
        // Move the position down by one line
        y += lineHeight;
        
        // Get the next line
        line = strtok(NULL, "\n");
    }

    int maxGraphWidth = SCREEN_WIDTH - 20; // 20 pixels padding
    SDL_SetRenderDrawColor(gRenderer, textColor.r, textColor.g, textColor.b, textColor.a);
    SDL_Rect bpBar = {10, 20 + lineHeight * 1, maxGraphWidth, 45};
    plotGraph(bpBar, bps, sequence, 200.0, 50.0);
    SDL_Rect flowBar = {10, 20 + lineHeight * 6, maxGraphWidth, 45};
    plotGraph(flowBar, frs, sequence, 350.0, 250.0);
    SDL_Rect o2Bar = {10, 20 + lineHeight * 11, maxGraphWidth, 45};
    plotGraph(o2Bar, o2s, sequence, 100.0, 80.0);
    
    // Update screen
    SDL_RenderPresent(gRenderer);
}

// Call this function in model.c to update the view
void displayCurrentValues(int sequence, float bp, float flowRate,
        float o2Saturation, bool leak, 
        float ufAdj, float rateAdj, float o2Adj) {
    
    // Handle events on SDL queue
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        // User requests quit by clicking [X] on the window decoration
        if (e.type == SDL_QUIT) {
            printf("*** Graphics window closed by user; exiting. ***\n");
            closeSDL();
            exit(0);
        }
    }

    // Update the view with the current values
    updateView(sequence, bp, flowRate, o2Saturation, leak, ufAdj, rateAdj, o2Adj);
}
