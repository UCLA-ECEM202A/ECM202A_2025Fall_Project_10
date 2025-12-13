---
layout: default
title: "Exploring Real-Time WebRTC Interface on MCUs & MPUs for Unitree GO2 Robots"
---

# **Exploring Real-Time WebRTC Interface on MCUs & MPUs for Unitree GO2 Robots**

*ESP32 and Raspberry Pi interface for communicating with Unitree Go2 Robots*

![Project Banner](./assets/img/banner-placeholder.png)  

---

## 👥 **Team**

- Akshara Kuduvalli (akuduvalli@ucla.edu, @akkuduvalli)  

---

## 📝 **Abstract**

The motivation for this project is that there is one main Unitree GO2 Web-based Communication Interface using WebRTC (that all similar implementations are based on), but is designed to run on a full computer. The goal of this project is to establish control from a microcontroller to the Go2 Robot, which enables a MCU-based platform for interacting with Go2 robots, opening up many avenues for future Go2 robot control from a microcontroller. To connect to Unitree Go2 Robots, opening a datachannel requires establishing a WebRTC connection, which requires the use of a full WebRTC stack, Crypto stack, and a signaling server. Due to resource limits and a general lack of WebRTC support, it is not suitable to implement a full WebRTC stack on a microcontroller. Instead, I introduced a Raspberry Pi as a medium to establish a WebRTC connection and open datachannel to the Go2 robot, and transmit and forward messages to an ESP32 over UART. On the ESP32, I use the Zephyr RTOS as the OS backend to process and send messages, and developed a custom list of shell commands to interact directly to the Go2 robot from an ESP32. With establishing succesful Go2 robot control, this serves an initial microntroller based platform for Go2 robot control. 

---

## 📑 **Slides**

- [Midterm Checkpoint Slides](http://)  
- [Final Presentation Slides](http://)

---

## 🎛️ **Media**

- [Video Demo of Robot Control from an ESP32s3](http://)

---

# **1. Introduction**

Use the introduction to clearly set context, describe motivation, and explain the central idea behind your project.

### **1.1 Motivation & Objective**  
The Unitree Go2 Robot unofficially has one way to communicate (receive/send messages) without using the provided hardware/software tools - through establishing a WebRTC connection and opening a datachannel to subcribe to various sensor/state topics. This approach exists as a python-based open-source implementation that has been adapted for other platforms such as ROS2, but there is no official porting avenue for microcontroller based unitree Go2 robot control. 

### **1.2 State of the Art & Its Limitations**  
Previous implementations such as the Unitree Go2 ROS2 SDK Project [Nuralem] or the original WebRTC hack implementation go2-webrtc [Foldi] execute on a python-based backend, leveraging the python library [aiortc](https://github.com/aiortc/aiortc), a complete WebRTC ecosystem. There exist little resources online for developing a WebRTC client on a microcontroller, and there are no microcontroller based solutions specifically for Unitree Go2 robots. 

### **1.3 Novelty & Rationale**  
What is unique about this new approach is that the primary control is offloaded to the microcontroller, including message construction, message decoding, and specific control to the Unitree Go2 Robot. Because it leverages a Raspberry Pi to extablish a WebRTC connection instead of implementing it directly on a microcontroller, this solution bypasses the need to develop a full WebRTC client directly on a microcontroller, instead focusing more on developing a platform to establish Go2 robot control. 

### **1.4 Potential Impact**  
With this project, there is now a microcontroller-based platform to communicate with a Unitree Go2 robot. This opens up many avenues for developing projects that control the Unitree robots through a microcontroller, and opens up avenues for Go2 sensor processing offloaded to a microcontroller. With this platform, there are opportunities to control Go2 robots in a more compute and resource constrainsed environment, potentially valuable research with the full Go2 robot control. 

### **1.5 Challenges**  
The main technicaly challenge with this project is establishing a reliable way to connect via a microncontroller to the Go2 Robot. In order to connect to Go2 robots over WebRTC, the pipeline requires a full WebRTC stack, which involves ICE (candidate gathering, connectivity checks), DTLS (certificate exchange, handshake, encryption), SCTP over DTLS (for data channels), and SRTP / RTP (for video). WebRTC is fundamentally too heavy, stateful, and cryptography-intensive for a microcontroller-class device like the ESP32, especially under an RTOS, meant to be lightweight. Another challenge is implementing WebRTC on the Raspberry Pi, simply because of its complexity. 

### **1.6 Metrics of Success**  
The specific metrics used to evaluate this project is functionally being able to control the Go2 robot through an ESP32, being able to execute a variety of commands, measuring the limits to the UART message passing rate between the Raspberry Pi and the ESP32, and measuring the UART loopback latency from the Raspberry Pi to the ESP32 back to the Raspberry Pi to evaluate the overhead from Pi <-> ESP32 message passing over UART. 

---

# **2. Related Work**

The original Go2 WebRTC implementation is [go2-webrtc](https://github.com/tfoldi/go2-webrtc) [Foldi], from which all similar platforms are based upon. go2-webrtc leverages the `aiortc` Python library to execute the full WebRTC stack to be able to establish a connection and open a datachannel to the Go2 robot, for video streaming, sensor streaming, and robot actuation control. Based on this approach is the [go2-ros-sdk](https://github.com/abizovnuralem/go2_ros2_sdk/tree/master) [Nuralem], which leverages the original framework to expand into a full SDK in ROS2, subscribing to various sensor topics including lidar, IMU, etc. and camera streaming in order to create a complete application with a full Go2 robot view. Another robust Python API that uses the Go2 WebRTC driver to communicate is [unitree-webrtc-connect](https://github.com/legion1581/unitree_webrtc_connect), which also contains audio and video support. All of these frameworks are python-based, and all leverage the `aiortc` WebRTC library. I used all of these as references to leverage when developing a WebRTC platform connection on the Raspberry Pi to connect to the Go2 Robot. 

---

# **3. Technical Approach**

### **3.1 Hardware and Software Design**
The microcontroller that I chose was the ESP32-S3-N16R8 board, equipped with built-in Wi-Fi with additional Antenna, 8MB PSRAM, 16MB Flash, and 2 preconfigured UART ports. For software, it is built on the Zephyr RTOS platform, which contains many useful features for this project, including a robust scheduling platform, existing ESP32s3 driver support, JSON parsing, multithreading, WiFi mgmst, DTLS support, and WebSockets. Zephyr is also designed to be very modular, so it should be easily able to port from ESP32s3 to other MCUs for other work, requiring only devicetree overlay and configuration changes. The Raspberry Pi uses Python to leverage the existing WebRTC Go2 support, using the `aiortc` library and `pyserial` for UART communication. 

### **3.2 High Level System Architecture**

![System](./assets/img/system.png)  

The high level architecture of the system involves using the Raspberry Pi to establish a connection and open a datachannel over WebRTC to the robot. The Raspberry Pi launches a UART RX thread and establishes connection to the Go2 via a client class containing an asynchornous event loop for being notified of WebRTC connection related messages, including validation, message reception, and opening the datachannel. The UART RX thread registers incoming input from the UART RX port on the Raspberry Pi, forwarding the message exactly as it was received directly over the WebRTC datachannel port to the Go2 robot. 

### **3.3 WebRTC Data Pipeline**

![WebRTC pipeline](./assets/img/webrtc.jpeg)  


### **3.4 Raspberry Pi to ESP32 UART Pipeline**
![UART](./assets/img/uart_design.jpeg)  
The reason I chose UART as the communication protocl between the Pi and the ESP32 is that is simple, has separate TX/RX lines (as opposed to I2C), and it is relatively easy to design a packet protocol for both directions to accommodate larger packets using UART. There is also robust serial support in both Zephyr and in Python. I had also looked into SPI, but the ESP32 SPI device does not have peripheral mode support (in hardware / in the Zephyr driver), and neither does Raspberry Pi, so it wasn’t feasible, as the controller-peripheral pipeline is imperative to SPI. 

For the UART packet design protocol, the packets are sent and received as follows:

* Start Byte `0xAA`
* Length - 2 bytes, Big Endian
* Message Type Byte 
* Variable Length Payload
* CRC byte (XOR all bytes following the start byte)

This packet structure allows asynchronous message passing and receival, perfect for the bidirectional link between the Pi and ESP32, for which messaging intervals are not determined.

### **3.5 Zephyr Application Design **

![Zephyr](./assets/img/zephyr.jpeg)  
The Zephyr main application (`main.c`) launches two threads to `process_messages.c` and `uart_rx_tx.c`, which is the main UART processing thread. The main application also defines a `json_rx_queue` and a `json_tx_queue`. The UART thread waits on messages to the `json_tx_queue`, which are messages pushed in the Zephyr application that are intended to be sent out to the Raspberry Pi. The UART thread also continuously waits on messages from the UART RX port, and pushed them to the `json_rx_queue`, which notifies the process_messages thread for further processing, such as JSON decoding. 

### **3.6 Zephyr Shell Design **

![Zephyr Shell](./assets/img/zephyr_shell.png)  
To test this platform and establish control to the Go2 robots from the ESP32, I developed a Zephyr Shell application that uses a UART backend to be able to send custom messages to the Go2 such as 

`go2 standup` 

`go2 standown`

`go2 send Hello`

After receiving a command, using  `command_generator.c`, the correct JSON messages are created according to the Go2 JSON messaging structure, including type of command, command id (pseudo random created from the kernel timestamp), API id, and parameters. It then pushes this complete JSON message to the `json_tx_queue`, which notifies the UART thread that it should send out a message to the Raspberry Pi. 

### **3.7 Key Design Decisions & Rationale**
Organizing the Zephyr applications into multiple threads allows for clean execution and separation of processes, perfect of a real-time control system like this. Also, the UART packet structure is designed to indicate message passing errors (with CRC), allowing for robust message passing. The Go2 Zephyr shell provides a simple and robust medium to test many different types of commands to send to the Go2. 

---

# **4. Evaluation & Results**

Present experimental results with clarity and professionalism.

Include:

- Plots (accuracy, latency, energy, error curves)  
- Tables (comparisons with baselines)  
- Qualitative visualizations (spectrograms, heatmaps, bounding boxes, screenshots)  
- Ablation studies  
- Error analysis / failure cases

Each figure should have a caption and a short interpretation.

Add some message dropping results, also see if you can finally get the loopback test working 
And also add the Zephyr usage diagram, which was helpful to see it was a pretty lighweight applciation

---

# **5. Discussion & Conclusions**

What went well in this project was the Zephyr based control and message construction and UART TX, which was a relatively robust and reliable pipeline to be able to send messages to the Go2 robot, executing pretty instantaneously. I think that the design of separating the WebRTC platform was a novel way to still be able to control the Go2 Robot on a microncontroller without needing the WebRTC stack directly on it as well. 

I encountered many challenges through this project, especially with understanding how WebRTC works. 
Establishing the WebRTC datachannel from the Go2 Robot to the Raspberry Pi was also initially quiite difficult to debug, which ended up being
mismatches in the local offer made to the robot, requiring extensive debugging. Designing and choosing a communication interface between the Raspberry Pi and the ESP32 was also something that I would consider changing in the future, as there are latency and throughput limits witht he current state of the UART bus that could be mitigated. Furthermore, processing data from the Go2 robot in Zephyr is still in an early state and requires more custom processing. 

With more time on this project, the future work includes establishing more robust sensor data processing in Zephyr, as currently this application does not do much post processing in the `process_messages` thread in Zephyr. It would especially be good to look into Lidar data processing on the ESP32, which would require message decoding in Zephyr, which would have been interesting to explore. Also, creating complete applications using the communication platform for simultaneous robot control based on sensor input is another large step for future work with this platform, as well as porting this to other MCUs other than the ESP32, perhaps with more compute power and memory. 

Also, if I had more time, I would continue researching the potential of establishing a direct MCU to Go2 robot control over WebRTC. 

---

# **6. References**

Provide full citations for all sources (academic papers, websites, etc.) referenced and all software and datasets uses.

---

# **7. Supplementary Material**

## **7.a. Software**

List:
* External libraries or models
* Internal modules you wrote
* Links to repos or documentation

Basically the software folder design and how to reproduce it 

---

> [!NOTE] 
> Read and then delete the material from this line onwards.

# 🧭 **Guidelines for a Strong Project Website**

- Include multiple clear, labeled figures in every major section.  
- Keep the writing accessible; explain acronyms and algorithms.  
- Use structured subsections for clarity.  
- Link to code or datasets whenever possible.  
- Ensure reproducibility by describing parameters, versions, and preprocessing.  
- Maintain visual consistency across the site.

---

# 📊 **Minimum vs. Excellent Rubric**

| **Component**        | **Minimum (B/C-level)**                                         | **Excellent (A-level)**                                                                 |
|----------------------|---------------------------------------------------------------|------------------------------------------------------------------------------------------|
| **Introduction**     | Vague motivation; little structure                             | Clear motivation; structured subsections; strong narrative                                |
| **Related Work**     | 1–2 citations; shallow summary                                 | 5–12 citations; synthesized comparison; clear gap identification                          |
| **Technical Approach** | Text-only; unclear pipeline                                  | Architecture diagram, visuals, pseudocode, design rationale                               |
| **Evaluation**       | Small or unclear results; few figures                          | Multiple well-labeled plots, baselines, ablations, and analysis                           |
| **Discussion**       | Repeats results; little insight                                | Insightful synthesis; limitations; future directions                                      |
| **Figures**          | Few or low-quality visuals                                     | High-quality diagrams, plots, qualitative examples, consistent style                      |
| **Website Presentation** | Minimal formatting; rough writing                           | Clean layout, good formatting, polished writing, hyperlinks, readable organization        |
| **Reproducibility**  | Missing dataset/software details                               | Clear dataset description, preprocessing, parameters, software environment, instructions   |
