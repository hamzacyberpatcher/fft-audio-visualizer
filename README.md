# MIKU RING AUDIO VISUALIZER

A high-performance real-time audio visualizer that uses **FFT (Fast Fourier Transform)** to transform sound into a circular frequency ring. This project utilizes **FFTW3** for complex signal processing and **SFML** for hardware-accelerated rendering and audio playback.

## 🚀 Features
* **FFT Spectrum Analysis:** Real-time frequency analysis using the FFTW3 library.
* **Logarithmic Scaling:** Frequency bins are mapped logarithmically for a more natural, musical visual representation.
* **Circular Visualization:** 128 dynamic bars arranged in a ring that react to the music's intensity.
* **Smooth Transitions:** Magnitude smoothing to prevent flickering and create fluid movement.

---

## 🛠 Prerequisites

You must have the development headers for both SFML and FFTW3 installed on your Linux system. Run the following:

```bash
sudo apt update
sudo apt install libsfml-dev libfftw3-dev g++
