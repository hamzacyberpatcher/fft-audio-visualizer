#include "fftw3.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstring>

// Custom clamp if not using C++17
template <typename T>
T clamp(T val, T min, T max) {
    return std::max(min, std::min(val, max));
}

int main() {
    const int N = 1024;
    const int NUM_BARS = 128;
    const float radius = 200.f;         // Distance from center
    const float scaleFactor = 0.00015f;  // Height of bars
    const float smoothingFactor = 0.2f;

    const int winSize = 1000;
    sf::RenderWindow window(sf::VideoMode(winSize + 700, winSize), "MIKU RING AHHHH!!!");
    sf::Vector2f center((winSize + 700) / 2.f, winSize / 2.f);

    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("./Assets/Music/alucard.wav")) {
        std::cerr << "miku not found ahhhh!!!!!!!!!" << std::endl;
        return -1;
    }

    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.play();

    const sf::Int16* samples = buffer.getSamples();
    const std::size_t totalSamples = buffer.getSampleCount();
    const std::size_t sampleRate = buffer.getSampleRate();
    const std::size_t channels = buffer.getChannelCount();

    std::vector<double> monoSamples;
    for (std::size_t i = 0; i < totalSamples; i += channels)
        monoSamples.push_back(static_cast<double>(samples[i]));

    // FFTW
    double* in = (double*)fftw_malloc(sizeof(double) * N);
    fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * (N / 2 + 1));
    fftw_plan plan = fftw_plan_dft_r2c_1d(N, in, out, FFTW_ESTIMATE);

    std::vector<double> smoothedMagnitudes(N / 2 + 1, 0.0);
    std::vector<double> displayBars(NUM_BARS, 0.0);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Audio FFT
        sf::Time offset = sound.getPlayingOffset();
        std::size_t sampleIndex = static_cast<std::size_t>(offset.asSeconds() * sampleRate);
        if (sampleIndex + N >= monoSamples.size())
            continue;

        std::memcpy(in, &monoSamples[sampleIndex], sizeof(double) * N);
        fftw_execute(plan);

        for (int i = 0; i <= N / 2; ++i) {
            double real = out[i][0];
            double imag = out[i][1];
            double magnitude = std::sqrt(real * real + imag * imag);
            smoothedMagnitudes[i] = smoothingFactor * magnitude + (1.0 - smoothingFactor) * smoothedMagnitudes[i];
        }

        // Log-scale binning
        for (int i = 0; i < NUM_BARS; ++i) {
            int start = static_cast<int>(std::pow(N / 2.0 + 1, i / static_cast<double>(NUM_BARS)));
            int end = static_cast<int>(std::pow(N / 2.0 + 1, (i + 1) / static_cast<double>(NUM_BARS)));
            start = clamp(start, 0, N / 2);
            end = clamp(end, start + 1, N / 2);

            double sum = 0.0;
            for (int j = start; j < end; ++j)
                sum += smoothedMagnitudes[j];
            displayBars[i] = sum / (end - start);
        }

        // Draw
        window.clear(sf::Color::Black);

        for (int i = 0; i < NUM_BARS; ++i) {
            float angle = (i / static_cast<float>(NUM_BARS)) * 2 * 3.14159265f;
            float magnitude = static_cast<float>(displayBars[i] * scaleFactor);

            sf::Vector2f dir(std::cos(angle), std::sin(angle));
            sf::Vector2f start = center + dir * radius;
            sf::Vector2f end = center + dir * (radius + magnitude);

            sf::Vertex line[] = {
                sf::Vertex(start, sf::Color::Cyan),
                sf::Vertex(end, sf::Color::Cyan)
            };

            window.draw(line, 2, sf::Lines);
        }

        window.display();
    }

    fftw_destroy_plan(plan);
    fftw_free(in);
    fftw_free(out);

    return 0;
}
