#include "ServoControl.h"
#include "BluetoothControl.h" // Include to access shared variables

// Initialize servo objects
Servo esc1;
Servo esc2;
Servo servo1;

// Initialize timestamp for servo movements
unsigned long lastServoMoveTime = 0;

// Function to initialize servo motors
void initializeServos() {
    esc1.attach(7);  // Attach ESC1 to pin 7
    esc2.attach(6);  // Attach ESC2 to pin 6
    servo1.attach(5);  // Attach servo1 to pin 5
    servo1.write(servo1Value);  // Set initial servo position to 90 degrees (middle)
    calibrateESC(esc1);  // Calibrate ESC1
    calibrateESC(esc2);  // Calibrate ESC2
}

// Function to handle servo movements based on commands
void handleServoMovement() {
    // Check if servo should move left
    if (moveServoLeft && millis() - lastServoMoveTime > 30) {
        servo1Value = max(servo1Value - 1, 45);  // Decrease value, not below 45
        //Serial.println(servo1Value);  // Print the current servo value for debugging
        servo1.write(servo1Value);  // Move the servo
        lastServoMoveTime = millis();  // Update timestamp
    }

    // Check if servo should move right
    if (moveServoRight && millis() - lastServoMoveTime > 30) {
        servo1Value = min(servo1Value + 1, 135);  // Increase value, not above 135
        //Serial.println(servo1Value);  // Print the current servo value for debugging
        servo1.write(servo1Value);  // Move the servo
        lastServoMoveTime = millis();  // Update timestamp
    }

    // Check if servo should return to center
    if (!moveServoRight && !moveServoLeft && servo1Value != 90 && millis() - lastServoMoveTime > 30) {
        if (servo1Value > 90) {
            servo1Value = max(servo1Value - 1, 90);  // Move back to center from right
        } else {
            servo1Value = min(servo1Value + 1, 90);  // Move back to center from left
        }
        servo1.write(servo1Value);  // Move the servo
        //Serial.println(servo1Value);  // Print the current servo value for debugging
        lastServoMoveTime = millis();  // Update timestamp
    }

    //Printing all data in format : "ESC1: 0 || ESC2: 0 || Servo: 90°"
    Serial.print("Thrust: ");
    Serial.print((esc1Value-1000)/(10.0));
    Serial.print(" ||  Lift: ");
    Serial.print((esc2Value-1000)/(10.0));
    Serial.print(" ||  Servo: ");
    Serial.print(servo1Value);
    Serial.println("°");
    
}

// Function to calibrate an ESC
void calibrateESC(Servo& esc) {
    esc.writeMicroseconds(2000);  // Send maximum signal
    delay(2000);  // Wait for 2 seconds
    esc.writeMicroseconds(1000);  // Send minimum signal
    delay(2000);  // Wait for 2 seconds
}
