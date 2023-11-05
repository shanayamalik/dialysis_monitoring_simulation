#include "dialysis_sim.h"

int main() {
    srand((unsigned int)time(NULL)); // Seed the random number generator
    
    bool graphics = initSDL(); // initialize graphics
    if (!graphics) {
        closeSDL(); // clean up if anything failed
    }
    
    printf("*** Starting dialysis process simulation. ***\n");
    bool success = controlLoop(graphics); // Start the control loop
    if (success) {
        printf("*** Dialysis process simulation completed successfully. ***\n");
    } else {
        printf("*** Dialysis process simulation ended abnormally. ***\n");
    }
    
    if (graphics) {
        printf("Press enter to close the graphics window: ");
        getchar(); // Wait for enter key from console
        closeSDL();
    }
    
    return 0; // exit
}
