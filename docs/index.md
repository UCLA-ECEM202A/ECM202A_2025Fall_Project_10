---
layout: default
title: "Project Title"
---

# **Exploring Real-Time WebRTC Interface on MCUs & MPUs for Unitree GO2 Robots**

*ESP32 and Raspberry Pi interface for communicating with Unitree Go2 Robots*

![Project Banner](./assets/img/banner-placeholder.png)  

---

## 👥 **Team**

- Akshara Kuduvalli (akuduvalli@ucla.edu, @akkuduvalli)  

---

## 📝 **Abstract**

Provide a short paragraph (4–6 sentences) summarizing:

- The problem you addressed  
- Your approach  
- Key results  
- The significance of your findings  

This should be a standalone “TL;DR” of your project.

The motivation for this project is that there is one main Unitree GO2 Web-based Communication Interface using WebRTC (that all similar implementations are based on), but is designed to run on a full computer. The goal of this project is to establish control from a microcontroller to the Go2 Robot, which enables a MCU-based platform for interacting with Go2 robots, opening up many avenues for future Go2 robot control from a microcontroller. To connect to Unitree Go2 Robots, opening a datachannel requires establishing a WebRTC connection, which requires the use of a full WebRTC stack, Crypto stack, and a signaling server. Due to resource limits and a general lack of WebRTC support, it is not suitable to implement a full WebRTC stack on a microcontroller. Instead, I introduced a Raspberry Pi as a medium to establish a WebRTC connection and open datachannel to the Go2 robot, and transmit and forward messages to an ESP32 over UART. On the ESP32, I use the Zephyr RTOS as the OS backend to process and send messages, and developed a custom list of shell commands to interact directly to the Go2 robot from an ESP32. With establishing succesful Go2 robot control, this serves an initial microntroller based platform for Go2 robot control. 


---

## 📑 **Slides**

- [Midterm Checkpoint Slides](http://)  
- [Final Presentation Slides](http://)

---

> Delete this section if you have no videos.

## 🎛️ **Media**

- [Video Demo of Robot Control from an ESP32s3](http://)

---

# **1. Introduction**

Use the introduction to clearly set context, describe motivation, and explain the central idea behind your project.

### **1.1 Motivation & Objective**  
The Unitree Go2 Robot unofficially has one way to communicate (receive/send messages) without using the provided hardware/software tools - through establishing a WebRTC connection and opening a datachannel to subcribe to various sensor/state topics. This approach exists as a python-based open-source implementation that has been adapted for other platforms such as ROS2, but there is no official porting avenue for microcontroller based unitree Go2 robot control. 

### **1.2 State of the Art & Its Limitations**  
Previous implementations such as the Unitree Go2 ROS2 SDK Project [Nuralem] or the original WebRTC hack implementation go2-webrtc [Foldi] execute on a python-based backend, leveraging the python library (aiortc)[https://github.com/aiortc/aiortc], a complete WebRTC ecosystem. There exist little resources online for developing a WebRTC client on a microcontroller, and there are no microcontroller based solutions specifically for Unitree Go2 robots. 

### **1.3 Novelty & Rationale**  
What is new about your approach, and why do you expect it to succeed?
What is unique about this new approach is that the primary control is offloaded to the microcontroller, including message construction, message decoding, and specific control to the Unitree Go2 Robot. Because it leverages a Raspberry Pi to extablish a WebRTC connection instead of implementing it directly on a microcontroller, this solution bypasses the need to develop a full WebRTC client directly on a microcontroller, instead focusing more on developing a platform to establish Go2 robot control. 

### **1.4 Potential Impact**  
With this project, there is now a microcontroller-based platform to communicate with a Unitree Go2 robot. This opens up many avenues for developing projects that control the Unitree robots through a microcontroller, and opens up avenues for Go2 sensor processing offloaded to a microcontroller. With this platform, there are opportunities to control Go2 robots in a more compute and resource constrainsed environment, potentially valuable research with the full Go2 robot control. 

### **1.5 Challenges**  
List the main technical, practical, or methodological challenges.
The main technicaly challenge with this project is establishing a reliable way to connect via a microncontroller to the Go2 Robot. In order to connect to Go2 robots over WebRTC, the pipeline requires a full WebRTC stack, which involves 

### **1.6 Metrics of Success**  
What are the specific, measurable criteria for evaluating your project?

---

# **2. Related Work**

Summarize prior works relevant to your project.  
For each: what did the authors do, how is it related, and what gap remains?

Reference all citations in **Section 6**.

---

# **3. Technical Approach**

Describe your system, methodology, algorithms, and design choices.  
Use figures generously:

- System architecture diagram  
- Data pipeline  
- Algorithm/model block diagram  
- Hardware setup photos  

💡 Tip: Add images, diagrams, and code snippets. Make your system reproducible.

Recommended subsections:

### **3.1 System Architecture**
Include a block diagram or pipeline figure.

### **3.2 Data Pipeline**
Explain how data is collected, processed, and used.

### **3.3 Algorithm / Model Details**
Use math, pseudocode, or diagrams as needed.

### **3.4 Hardware / Software Implementation**
Explain equipment, libraries, or frameworks.

### **3.5 Key Design Decisions & Rationale**
Describe the main design decisions you made.

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

---

# **5. Discussion & Conclusions**

Synthesize the main insights from your work.

- What worked well and why?  
- What didn’t work and why?  
- What limitations remain?  
- What would you explore next if you had more time?  

This should synthesize—not merely repeat—your results.

---

# **6. References**

Provide full citations for all sources (academic papers, websites, etc.) referenced and all software and datasets uses.

---

# **7. Supplementary Material**

## **7.a. Datasets**

Describe each dataset:
* Source and URL
* Data format
* Preprocessing steps
* Labeling/annotation efforts

Include your internal dataset if you collected one.
## **7.b. Software**

List:
* External libraries or models
* Internal modules you wrote
* Links to repos or documentation

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
