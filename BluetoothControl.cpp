#include "BluetoothControl.h"

// Initialize Bluetooth serial object
BluetoothSerial SerialBT;

// Initialize global variables for ESC and servo control
int esc1Value = 1000, esc2Value = 1000, servo1Value = 90;
bool moveServoLeft = false;
bool moveServoRight = false;

// Function to initialize Bluetooth communication
void initializeBluetooth() {
    Serial.begin(115200);  // Start serial communication for debugging
    if (!SerialBT.begin("ESP32Hovercraft")) {  // Initialize Bluetooth with the name "ESP32Hovercraft"
        Serial.println("An error occurred initializing Bluetooth");
    } else {
        Serial.println("Bluetooth initialized successfully");
    }
}

// Function to handle incoming Bluetooth data
void handleBluetoothData() {
    if (SerialBT.available()) {  // Check if Bluetooth data is available
        handleBluetoothCommand();  // Process the received Bluetooth command
    }
}

// Function to process Bluetooth commands
void handleBluetoothCommand() {
    String inputString = SerialBT.readStringUntil('\n');  // Read the incoming command until newline character
    //Serial.println(inputString);  // Print the received command for debugging

    // Check the command type and process accordingly
    if (inputString.startsWith("J")) {  // Command for ESC1
        esc1Value = inputString.substring(1).toInt();  // Extract value and convert to integer
        esc1Value = map(esc1Value, 0, 180, 1000, 2000);  // Map value to ESC range
    } else if (inputString.startsWith("K")) {  // Command for ESC2
        esc2Value = inputString.substring(1).toInt();  // Extract value and convert to integer
        esc2Value = map(esc2Value, 0, 180, 1000, 2000);  // Map value to ESC range
    } else if (inputString.startsWith("L")) {  // Command to move servo left
        moveServoLeft = true;
        moveServoRight = false;
    } else if (inputString.startsWith("R")) {  // Command to move servo right
        moveServoRight = true;
        moveServoLeft = false;
    } else if (inputString.startsWith("S")) {  // Command to stop servo movement
        moveServoLeft = false;
        moveServoRight = false;
    }

    //Serial.println(servo1Value);  // Print the current servo value for debugging
}
