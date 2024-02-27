# MQTT Ring Light Control

This repository contains code for controlling a ring light via MQTT (Message Queuing Telemetry Transport) protocol. The code is designed to work with an MQTT broker and allows users to remotely control the brightness of the ring light.

## Requirements

To use this code, you'll need the following:

- An MQTT broker: You can use any MQTT broker of your choice. Popular options include Mosquitto, HiveMQ, and Cloud-based services like AWS IoT or Google Cloud IoT.
- A microcontroller: This code is designed to run on microcontrollers compatible with the Arduino framework, such as Arduino Uno, ESP8266, ESP32, etc.
- A ring light: The code assumes control over a ring light that is connected to the microcontroller.

## Setup

1. **Install Arduino IDE**: If you haven't already, install the Arduino IDE from [arduino.cc](https://www.arduino.cc/en/software).

2. **Install Libraries**: Install the required libraries using the Library Manager in Arduino IDE. The required libraries are:
   - `PubSubClient`: This library provides a client for doing simple publish/subscribe messaging with a server that supports MQTT.
   - Any additional libraries required for your specific microcontroller or ring light setup.

3. **Configure the Code**:
   - Set the appropriate Wi-Fi credentials if using Wi-Fi connection.
   - Set the MQTT broker address, port, and topic names as per your MQTT broker setup.
   - Adjust any pin configurations or hardware specifics according to your microcontroller and ring light setup.

4. **Upload the Code**: Connect your microcontroller to your computer, open the `.ino` file in Arduino IDE, select the correct board and port, and upload the code to your microcontroller.

5. **Connect the Hardware**: Ensure your ring light is properly connected to the microcontroller according to the hardware setup defined in the code.

6. **Power On**: Power on your microcontroller and ensure it's connected to the Wi-Fi network (if applicable) and the MQTT broker.

7. **Testing**: Test the setup by publishing MQTT messages to control the brightness of the ring light. You can use MQTT client applications or command-line tools to publish messages.

## MQTT Topics

The code subscribes to the following MQTT topics:

- `ring/brightness`: To control the brightness of the ring light. Messages should contain a numerical value representing the brightness level (e.g., `0` for off, `255` for full brightness).
- `ring/status`: To receive status updates from the ring light.

## Example Usage

Here's an example of how you can control the ring light using MQTT:

- To set the brightness to 50%, publish a message to the topic `ring/brightness` with payload `128`.

