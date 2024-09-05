# Project Overview
This project implements a circuit where an Arduino Uno microcontroller reads temperature and humidity data from a DHT11 sensor. The system tracks and displays the maximum temperature difference and humidity recorded.

# Features
- The microcontroller takes readings every 4 seconds and records the daily minimum and maximum temperature and humidity values.
- It calculates the largest daily temperature difference and compares it to previous days.
 - The results are shown on a 7-segment display, activated by a pushbutton:
    - First press: Displays the maximum temperature difference and humidity recorded.
    - Second press: Shows the current temperature and humidity.
    - Third press: Turns off the display.
- To maximize energy efficiency, the Arduino enters an idle state when not measuring or displaying data.

![alt text](https://github.com/akourkoulos/ARDUINO-Display-Tempratur---Humidity/blob/main/circuit.png)
