#include "dialysis_sim.h"

// PID control gains for each control variable -- placeholder values for now
#define Kp_BP 0.1
#define Ki_BP 0.05
#define Kd_BP 0.01

#define Kp_FLOW 0.1
#define Ki_FLOW 0.05
#define Kd_FLOW 0.01

#define Kp_O2 0.1
#define Ki_O2 0.05
#define Kd_O2 0.01

// Error sums for integral control
float errorSum_BP = 0;
float errorSum_FLOW = 0;
float errorSum_O2 = 0;

// Previous errors for derivative control
float previousError_BP = 0;
float previousError_FLOW = 0;
float previousError_O2 = 0;

// PID control function for blood pressure
float controlBloodPressure() {
    float bp = readBloodPressure();
    float error = BP_SETPOINT - bp;
    errorSum_BP += error;
    float dError = error - previousError_BP;
    previousError_BP = error;

    float adjustment = Kp_BP * error + Ki_BP * errorSum_BP + Kd_BP * dError;
    setUltrafiltrationRate(adjustment);
    return adjustment;
}

// PID control function for blood flow rate
float controlBloodFlowRate() {
    float flowRate = readBloodFlowRate();
    float error = FLOW_RATE_SETPOINT - flowRate;
    errorSum_FLOW += error;
    float dError = error - previousError_FLOW;
    previousError_FLOW = error;

    float adjustment = Kp_FLOW * error + Ki_FLOW * errorSum_FLOW + Kd_FLOW * dError;
    setBloodPumpRate(adjustment);
    return adjustment;
}

// Control function for oxygen saturation
float controlOxygenSaturation() {
    float o2Saturation = readOxygenSaturation();
    float error = O2_SATURATION_SETPOINT - o2Saturation;
    errorSum_O2 += error;
    float dError = error - previousError_O2;
    previousError_O2 = error;

    float adjustment = Kp_O2 * error + Ki_O2 * errorSum_O2 + Kd_O2 * dError;
    adjustOxygenDelivery(adjustment);
    return adjustment;
}

bool controlLoop(bool graphics) {
    int sequence = 0;

    while (sequence++ <= 2000) { // probably won't go to 2000 each blood leak chance is 0.1%
        printf("--- Iteration sequence number %d ---\n", sequence);
        
        float ufAdj = controlBloodPressure();
        float rateAdj = controlBloodFlowRate();
        float O2Adj = controlOxygenSaturation();
        bool leak = detectBloodLeak();

        if (graphics) { // ask model to display status graphics
            showView(sequence, ufAdj, rateAdj, O2Adj, leak);
        }
        
        if (leak) {
            printf("Blood leak detected! Stopping dialysis machine.\n");
            return false; // Exit the loop if a blood leak is detected
        }

        // Wait 200 milliseconds before the next control cycle
        if (graphics) usleep(150 * 1000); // Use 150 to compensate for inherent delay in processing
    }
    return true;
}
