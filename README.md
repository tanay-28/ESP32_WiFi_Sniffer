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

Sample Output:
I (6984) Wi-Fi Sniffer: [RTOS PRINT] Size: 356 bytes | RSSI: -77 dBm | MAC: 3C:46:A1:6E:05:B3
I (6994) Wi-Fi Sniffer: [RTOS PRINT] Size: 364 bytes | RSSI: -78 dBm | MAC: 3C:46:A1:6E:05:B4
I (6994) Wi-Fi Sniffer: [RTOS PRINT] Size: 119 bytes | RSSI: -79 dBm | MAC: 08:65:F0:DA:4D:63
I (7004) Wi-Fi Sniffer: [RTOS PRINT] Size: 340 bytes | RSSI: -71 dBm | MAC: 3C:46:A1:70:20:82
I (7014) Wi-Fi Sniffer: [RTOS PRINT] Size: 364 bytes | RSSI: -71 dBm | MAC: 3C:46:A1:70:20:84
I (7024) Wi-Fi Sniffer: [RTOS PRINT] Size: 340 bytes | RSSI: -78 dBm | MAC: 3C:46:A1:6D:93:92
I (7034) Wi-Fi Sniffer: [RTOS PRINT] Size: 356 bytes | RSSI: -76 dBm | MAC: 3C:46:A1:6D:93:93