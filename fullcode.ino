#include <ESP32Servo.h>
#include "BluetoothSerial.h"

// const char *pin = "1234"; 
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run make menuconfig to and enable it
#endif
#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;

Servo esc1;  // Create servo object to control the ESC for thrust
Servo esc2;  // Create servo object to control the ESC for lift
Servo servo1; // Create servo object to control the servo motor

int esc1Value, esc2Value, servo1Value = 90; // Variables to store the PWM values for ESC1, ESC2, and servo

bool moveServoLeft = false;
bool moveServoRight = false;
unsigned long lastServoMoveTime = 0;

void setup() {
    Serial.begin(115200); // Initialize serial communication at 115200 baud rate for debugging
    if (!SerialBT.begin("ESP32Hovercraft")) {  // Initialize Bluetooth with the name "ESP32Hovercraft"
        Serial.println("An error occurred initializing Bluetooth");
    } else {
        Serial.println("Bluetooth initialized successfully");
    }

    esc1.attach(7); // Attach ESC1 to pin 7
    esc2.attach(6); // Attach ESC2 to pin 6
    servo1.attach(5); // Attach servo1 to pin 5

    // Set initial servo position to 90 degrees (middle)
    servo1.write(servo1Value);
}

void loop() {
    if (SerialBT.available()) { // Check if there is any data available from Bluetooth
        handleBluetoothCommand(); // Handle the received Bluetooth command
    }

    // Handle servo movement
    if (moveServoLeft && millis() - lastServoMoveTime > 250) {
        servo1Value = max(servo1Value - 5, 45); // Decrease the value by 5 degrees, not going below 45
        Serial.println(servo1Value);
        servo1.write(servo1Value); // Send the new position to the servo
        lastServoMoveTime = millis(); // Update the timestamp of the last movement
    }
    if (moveServoRight && millis() - lastServoMoveTime > 250) {
        servo1Value = min(servo1Value + 5, 135); // Increase the value by 5 degrees, not exceeding 135
        Serial.println(servo1Value);
        servo1.write(servo1Value); // Send the new position to the servo
        lastServoMoveTime = millis(); // Update the timestamp of the last movement
    }
    if (!moveServoRight && !moveServoLeft && servo1Value!=90 && millis() - lastServoMoveTime > 333){
        if (servo1Value > 90){
            servo1Value = max(servo1Value - 5, 90);
        } else {
            servo1Value = min(servo1Value + 5, 90);
        }
        servo1.write(servo1Value); // Send the new position to the servo
        Serial.println(servo1Value);
        lastServoMoveTime = millis(); // Update the timestamp of the last movement

    }
}

void handleBluetoothCommand() {
    String inputString = SerialBT.readStringUntil('\n'); // Read the incoming command until newline character
    Serial.println(inputString);

    // THRUST
    if (inputString.startsWith("J")) { // If the command is for ESC1
        esc1Value = inputString.substring(1).toInt(); // Extract the value from the command and convert it to integer
        esc1Value = map(esc1Value, 0, 180, 1000, 2000); // Map the value from 0-180 to 1000-2000 (ESC range)
        esc1.writeMicroseconds(esc1Value); // Send the PWM control signal to the ESC

    // LIFT
    } else if (inputString.startsWith("K")) { // If the command is for the joystick (ESC2)
        esc2Value = inputString.substring(1).toInt(); // Extract the value from the command and convert it to integer
        esc2Value = map(esc2Value, 0, 180, 1000, 2000); // Map the value from 0-180 to 1000-2000 (ESC range)
        esc2.writeMicroseconds(esc2Value); // Send the PWM control signal to the ESC
    }

    // Servo control
    else if (inputString.startsWith("L")) { // If the command is for the servo (left)
        moveServoLeft = true; // Start moving the servo to the left
        moveServoRight = false; // Stop moving the servo to the right
    } else if (inputString.startsWith("R")) { // If the command is for the servo (right)
        moveServoRight = true; // Start moving the servo to the right
        moveServoLeft = false; // Stop moving the servo to the left
    } else if (inputString.startsWith("S")) { // If the command is for the servo (stationary)
        moveServoLeft = false; // Stop moving the servo to the left
        moveServoRight = false; // Stop moving the servo to the right
    }

    Serial.println(servo1Value);
}

void calibrateESC(Servo& esc) {
    // Send maximum signal to ESC
    esc.writeMicroseconds(2000);
    delay(2000); // Wait for 2 seconds

    // Send minimum signal to ESC
    esc.writeMicroseconds(1000);
    delay(2000); // Wait for 2 seconds
}