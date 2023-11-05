#include "dialysis_sim.h"

// Patient state variables
float currentBP = BP_SETPOINT;
float currentFlowRate = FLOW_RATE_SETPOINT;
float currentO2Saturation = O2_SATURATION_SETPOINT;

// Helper function to generate random fluctuations
float generateRandom(float minValue, float maxValue) {
    return minValue + (float)rand() / ((float)RAND_MAX / (maxValue - minValue));
}

// Simulated sensor reading functions

float readBloodPressure() {
    // Add random fluctuation and simulate the response to actuator changes
    currentBP += generateRandom(-1.0, 1.0); // Small random fluctuation
    printf("Reading blood pressure: %.0f mmHg\n", currentBP);
    return currentBP;
}

float readBloodFlowRate() {
    // Add random fluctuation and simulate the response to actuator changes
    currentFlowRate += generateRandom(-5.0, 5.0); // Small random fluctuation
    printf("Reading blood flow rate: %.1f mL/min\n", currentFlowRate);
    return currentFlowRate;
}

float readOxygenSaturation() {
    // Add random fluctuation and simulate the response to actuator changes
    currentO2Saturation += generateRandom(-0.5, 0.5); // Random fluctuation
    if (currentO2Saturation > 100.0) currentO2Saturation = 100.0; // clamp to max physically possible value
    printf("Reading oxygen saturation: %.0f%%\n", currentO2Saturation);
    return currentO2Saturation;
}

bool detectBloodLeak() {
    // Simulate blood leak detection
    // For simplicity, assume a 0.1% chance of detecting a leak each 200 ms cycle
    bool detected = (rand() % 1000) == 0;
    //printf("Blood leak detected: %s\n", detected ? "true" : "false");
    return detected;
}

// Actuator control functions directly affecting patient stats and machine state

void setUltrafiltrationRate(float rate) {
    printf("Adjusting ultrafiltration rate by: %+.2f mL/h\n", rate);
    // Simulate the effect of ultrafiltration rate adjustment on blood pressure
    currentBP += rate * 0.05; // Assume a scale factor for the effect
}

void setBloodPumpRate(float rate) {
    printf("Adjusting blood pump rate by: %+.1f mL/min\n", rate);
    // Simulate the effect of blood pump rate adjustment on blood flow rate
    currentFlowRate += rate; // No conversion factor for the pump
}

void adjustOxygenDelivery(float adjustment) {
    printf("Adjusting oxygen delivery by: %+.1f L/min\n", adjustment);
    // Simulate the nasal cannula's oxygen delivery adjustment on oxygen saturation
    currentO2Saturation += adjustment * 0.02; // Assume a scale factor
}

// display graphics in view.c, called from controller.c
void showView(int sequence, float ufAdj, float rateAdj, 
        float O2Adj, bool leak) {
    displayCurrentValues(sequence, currentBP, currentFlowRate, 
        currentO2Saturation, leak, ufAdj, rateAdj, O2Adj);
}
