# ⛏️ Coal Mine Surveillance Robot

An ESP32-based mobile surveillance robot developed for monitoring environmental and safety conditions in simulated coal-mine environments.

The robot combines environmental sensing, obstacle detection, orientation monitoring, live video streaming, remote control, and TinyML-based condition analysis in a single robotic platform.

## 📌 Project Overview

Coal-mine environments can involve hazardous gases, poor visibility, uneven surfaces, and areas that may be difficult or unsafe for direct human inspection.

This project focuses on developing a mobile robotic system that can be remotely operated to monitor important environmental and safety parameters and provide information to an operator.

The robot uses an ESP32 as the main controller and integrates multiple sensors and communication technologies for monitoring, navigation, and control.

## 🎯 Objectives

* Develop a mobile robotic platform for coal-mine surveillance
* Monitor methane and carbon monoxide levels
* Monitor temperature and humidity
* Detect obstacles during movement
* Monitor robot orientation and tilt
* Provide live video using ESP32-CAM
* Provide remote monitoring and control through Blynk
* Support manual and automatic operating modes
* Integrate TinyML for mine-condition classification
* Provide visual and audible alerts for abnormal conditions

## ⚙️ System Features

### Environmental Monitoring

The robot monitors important environmental parameters using:

* **MQ-4** — Methane detection
* **MQ-7** — Carbon monoxide detection
* **DHT11** — Temperature and humidity monitoring

### Obstacle Detection and Navigation

An ultrasonic sensor mounted with a servo mechanism is used to scan the surrounding area.

The robot can detect obstacles and, in automatic mode, change its movement based on the available path.

### Manual and Automatic Operation

The robot supports two operating modes:

**Manual Mode**

The operator can control the robot remotely through the Blynk dashboard.

**Automatic Mode**

The robot uses sensor information for obstacle detection and automatic movement.

### Live Video Monitoring

An **ESP32-CAM** is used to provide a live video feed so that the operator can observe the surroundings of the robot.

### Orientation Monitoring

An **ADXL345 accelerometer** is used to monitor the orientation and tilt of the robot.

This can help identify situations where the robot becomes excessively tilted or changes its orientation unexpectedly.

### TinyML-Based Condition Analysis

A TinyML model developed using **Edge Impulse** is integrated into the ESP32 system.

The model is used to analyse sensor data and classify mine-condition patterns based on the trained model.

### Alert System

The robot provides local alerts using:

* Red, yellow and green LEDs
* Headlight LEDs
* Buzzer

These indicators are used to represent different system or warning conditions.

## 🔩 Hardware Components

| Component      | Purpose                         |
| -------------- | ------------------------------- |
| ESP32          | Main controller                 |
| ESP32-CAM      | Live video streaming            |
| MQ-4           | Methane detection               |
| MQ-7           | Carbon monoxide detection       |
| DHT11          | Temperature and humidity        |
| HC-SR04        | Obstacle detection              |
| Servo Motor    | Ultrasonic sensor scanning      |
| ADXL345        | Orientation and tilt monitoring |
| Motor Driver   | DC motor control                |
| DC Motors      | Robot movement                  |
| LEDs           | Status and warning indication   |
| Buzzer         | Audible alert                   |
| Battery Pack   | Robot power supply              |
| Buck Converter | Voltage regulation              |
| Capacitors     | Power supply stabilization      |

## 🧠 Software and Technologies

* Arduino IDE
* ESP32
* ESP32-CAM
* Embedded C/C++
* Blynk IoT
* Edge Impulse
* TinyML
* Wi-Fi communication
* Sensor interfacing
* Motor control
* Obstacle avoidance

## 🏗️ System Architecture

The overall system can be viewed as five main sections:

```text
                    Blynk IoT Dashboard
                            │
                          Wi-Fi
                            │
                            ▼
                    ESP32 Main Controller
                     │       │       │
          ┌──────────┘       │       └──────────┐
          ▼                  ▼                  ▼
   Environmental       Navigation &        Robot Motion
      Sensors              Safety             Control
          │                  │                  │
   MQ-4 / MQ-7          HC-SR04 + Servo      Motor Driver
      DHT11                ADXL345            + DC Motors
          │
          ▼
       TinyML
   Edge Impulse Model

                    ESP32-CAM
                         │
                         ▼
                  Live Video Feed
```

## 🔄 Working Principle

1. The robot is powered using its battery supply and the required voltages are provided through the buck converter.

2. The ESP32 initializes the connected sensors, motor driver, indicators, communication and control functions.

3. The MQ-4 and MQ-7 sensors monitor methane and carbon monoxide levels.

4. The DHT11 measures temperature and humidity.

5. The ultrasonic sensor detects obstacles in front of the robot. The servo allows the sensor to scan different directions.

6. The ADXL345 provides orientation and tilt information.

7. The ESP32-CAM provides a live video feed for remote observation.

8. Sensor information is processed by the ESP32 and displayed through the Blynk dashboard where applicable.

9. In manual mode, the operator controls the robot through the Blynk interface.

10. In automatic mode, the robot uses obstacle information to assist its movement and navigation.

11. The integrated Edge Impulse TinyML model analyses the trained sensor data and provides the corresponding condition classification.

12. LEDs and the buzzer provide local status and warning indications.

## 📂 Repository Structure

```text
Coal-Mine-Surveillance-Robot/
│
├── documentation/
│   ├── reports
│   └── presentations
│
├── firmware/
│   └── main robot firmware
│
├── hardware/
│   └── hardware diagrams and references
│
├── tinyml/
│   └── TinyML / Edge Impulse files
│
├── ei-mine-ai-arduino-1.0.1/
│   └── Edge Impulse inference library
│
└── README.md
```

## 🚀 Setup and Usage

### 1. Clone the Repository

```bash
git clone https://github.com/Aathikesavan-stack/Coal-Mine-Surveillance-Robot.git
cd Coal-Mine-Surveillance-Robot
```

### 2. Open the Firmware

Open the required firmware from the `firmware/` directory using Arduino IDE.

### 3. Install Required Libraries

Install the libraries required by the firmware, including the libraries for:

* ESP32
* Blynk
* DHT11
* ADXL345
* Servo
* Ultrasonic sensor
* Edge Impulse

### 4. Configure Wi-Fi

Enter your own Wi-Fi credentials in the firmware.

```cpp
#define WIFI_SSID "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
```

### 5. Configure Blynk

Enter your own Blynk configuration.

```cpp
#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "YOUR_TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"
```

**Do not upload personal Wi-Fi passwords or Blynk authentication tokens to a public repository.**

### 6. Upload the Firmware

Connect the ESP32 to the computer, select the correct board and COM port in Arduino IDE, and upload the firmware.

### 7. Run the Robot

After powering the robot:

1. Check the sensor readings.
2. Verify the Blynk connection.
3. Check motor operation.
4. Check obstacle detection.
5. Verify the ESP32-CAM video feed.
6. Check LED and buzzer indications.
7. Test manual and automatic modes.

## 📊 Project Scope

This project is developed as an **academic engineering prototype** for studying embedded systems, robotics, IoT, environmental sensing and TinyML.

The prototype is intended for controlled or simulated environments and should **not be considered a certified safety device for deployment in active underground coal mines** without appropriate industrial testing, certification, intrinsically safe hardware, and compliance with applicable mining safety standards.

## 🎓 Project Type

**Project:** Coal Mine Surveillance Robot
**Type:** Final Year Engineering Project
**Domain:** Embedded Systems, IoT, Robotics and TinyML
**Main Controller:** ESP32

## 📚 Applications

The concepts demonstrated in this project can be explored for:

* Hazardous-environment monitoring
* Remote inspection
* Industrial surveillance
* Environmental monitoring
* Robotic navigation
* Embedded IoT systems
* Edge-AI/TinyML applications

## 📄 License

This project is intended for educational, research and development purposes.

If you reuse or modify this project, please provide appropriate attribution to the original repository.
