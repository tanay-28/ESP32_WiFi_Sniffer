**This is the first iteration of Wi-Fi Sniffer using only a ESP32 WROOM 32E built in ESP-IDF v6.0 using C++ and FreeRTOS.**

In this project the ESP32 intercepts raw Wi-Fi 802.11 management and data frames and captures the following key metrics:
1) RSSI (Received Signal Strength Indicator)
2) frame length
3) MAC Address

Some key features of the code are as follows:
1) Asynchronous ISR Pipeline: Utilizes a FreeRTOS queue mechanism to hand off payloads from the Wi-Fi promiscuous Interrupt Service Routine (ISR) to a low-priority processing task, ensuring thread safety and preventing CPU starvation.
2) Zero-Copy Performance Strategy: Uses pointer casting on raw incoming buffers to achieve high-throughput monitoring without unnecessary RAM-to-RAM payload copying.

Key components:
1) MCU: ESP32 WROOM 32E
2) Framework: ESP-IDF v6.0.1 (Native CMake Build Toolchain)
3) Language: C++ / C Standard Library
4) OS core: FreeRTOS Kernel