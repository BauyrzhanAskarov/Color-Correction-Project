# Color Correction for Protanopia Using Augmented Reality (HoloLens)
### Final Project - Real-Time Color Correction for Color Vision Deficiency

## Project Overview
This project aims to assist individuals with color vision deficiency (CVD), specifically protanopia (red-green color blindness), by leveraging Augmented Reality (AR) with Microsoft HoloLens (1st Gen). The system applies real-time color correction algorithms to enhance color perception for users with protanopia.

The solution is built using Python, OpenCV, and Microsoft’s HololensForCV library, incorporating image processing algorithms to modify RGB values dynamically.

---

## Features

- ✅ **Real-Time Protanopia Simulation** – Converts a normal scene into the way a protanopic person would perceive it.
- ✅ **Color Correction Algorithm** – Enhances red-green color differentiation for users with protanopia.
- ✅ **HoloLens Integration** – Works seamlessly with HoloLens (1st Gen.) using Microsoft's HololensForCV library.
- ✅ **Adaptive Image Processing** – Implements LMS Daltonization for improved color distinction.

---

## Technologies Used

- **Microsoft HoloLens 1st Gen**
- **Python, OpenCV**
- **[Microsoft HololensForCV GitHub repository for 1st generation](https://github.com/microsoft/HoloLensForCV.git)**
- **C++, C# (For AR Integration)**
- **MongoDB (For Data Storage)**
- **Unity3D (For HoloLens UI Development)**

---

## Installation & Setup

### Prerequisites

Ensure you have the following tools installed:

- ✔️ Microsoft Visual Studio 2017/2019
- ✔️ HoloLens Research Mode Enabled
- ✔️ Python 3.8+
- ✔️ OpenCV, NumPy

---

### Choose Mode

1. **Simulation Mode**: Displays how colorblind people perceive colors.  
2. **Correction Mode**: Adjusts colors for colorblind users in real-time.  
3. **Interact using Hand Gestures**  
4. **Two-finger tap** → Activates AR visualization  
5. **Gaze & Air Tap** → Select objects for color adjustment  
6. **Code Attribution & Modifications**

---

## Code Attribution & Modifications

This project is based on open-source code from Microsoft HololensForCV. The original source code can be found at Microsoft’s Official Repository.

### Modified Components

- 📌 **Correction of Protanopia** (See `Correction_of_protanopia.txt`)
- 📌 **Protanopia Simulation** (See `Protanopia_simulation.txt`)

#### Key Changes Include:
- Implemented custom LMS Daltonization algorithm for color transformation.
- Modified matrix operations to improve accuracy.
- Optimized real-time AR rendering on HoloLens.

---

## Results & Performance

- 📊 **Accuracy**:
  - ✔️ 85% Success Rate on Ishihara Tests (Pre/Post Correction).
  - ✔️ Reduced Processing Time for real-time rendering by 40%.

- 👓 **User Feedback**:
  - ✔️ 3 Colorblind Participants reported a significant improvement in distinguishing colors.

---

## Future Improvements

- 🔹 Implement more color correction algorithms (Deuteranopia, Tritanopia).
- 🔹 Support HoloLens 2 with improved performance.
- 🔹 Enhance gesture-based interaction.

---

## Contributors

- 👤 **Bauyrzhan Askarov**
- 👤 **Dias Akimbay**
- 👤 **Bekmurat Amangeldiyev**
- 👤 **Rakhat Ospanov**
- 👤 **Zhanat Kappassov**
