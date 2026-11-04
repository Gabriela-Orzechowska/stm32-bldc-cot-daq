# STM32 BLDC COT DAQ
[![MIT License](https://img.shields.io/badge/MIT%20License-FFFFFF)](#) [![C++](https://img.shields.io/badge/C++-%2300599C.svg?logo=c%2B%2B&logoColor=white)](#) [![C](https://img.shields.io/badge/C-00599C?logo=c&logoColor=white)](#) [![STM32](https://img.shields.io/badge/STM32-03234B?logo=stmicroelectronics&logoColor=white)](#)

Firmware and PC software for a measurement system for electric BLDC motor speed fluctuation monitoring 
and data acquisition for order tracking for diagnostic purposes.

Currently the project is in experimental and development stage with basic functionality working.

## Hardware
The firmware is designed to work with STM32H753ZI Nucleo board. Allows for measuring speed fluctuations and up to 4 ADC channels. It utilitizes Ethernet connection to transfer data from the device to PC. 

As of currently:
- Hardcoded IP and Port: `192.168.8.130:5000`
- Encoder pins: PA15, PB3
- ADC pins: PC0, PC2, PC3, PF10

### Building
The firmware is a standard STM32CubeIDE 1.19 project.

## PC Application

PC application allows for reading the live data, saving to the drive, basic analisis, and device configuration. 

TODO: I will add a screenshot here once I get measurements again.

### Building
The software is written in C++23 and build using CMake in `app` folder
```
cmake -B build -S .
cmake --build build
```

## License
This software is licensed under MIT. See `LICENSE` for details.

