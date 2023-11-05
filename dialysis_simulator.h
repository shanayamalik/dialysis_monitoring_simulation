#ifndef DIALYSIS_SIM_H
#define DIALYSIS_SIM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

// System target measurement constants
#define BP_SETPOINT 120 // Desired blood pressure in mmHg
#define FLOW_RATE_SETPOINT 300 // Desired blood flow rate in mL/min
#define O2_SATURATION_SETPOINT 95 // Desired oxygen saturation in percent

// model.c prototypes

float readBloodPressure();
float readBloodFlowRate();
float readOxygenSaturation();
bool detectBloodLeak();

void setUltrafiltrationRate(float rate);
void setBloodPumpRate(float rate);
void adjustOxygenDelivery(float adjustment);

void showView(int sequence, float ufAdj, float rateAdj, float O2Adj, bool leak);

// view.c prototypes

bool initSDL();
void closeSDL();
void displayCurrentValues(int sequence, float bp, float flowRate, float o2Saturation, bool leak, float ufAdj, float rateAdj, float O2Adj);

// controller.c function prototype

bool controlLoop(bool graphics); // called from main() in main.c

#endif
