#include <algorithm>
#include <cstdint>
#include <iostream>
#include <memory>
#include <random>
#include <vector>
#include <cmath>

#include <SFML/Graphics.hpp>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int FPS_LIMIT = 30;

const int FRAMES_PER_ANIMATION = 120; // so 4 seconds per frame
int frame = 0; // frame passed count

// global tween function
std::function<float(float, float, float)> tween = [](float a, float b, float t) {
    return (1 - t) * a + t * b;
};

void handleInput(sf::Window& window, bool& shouldQuit) {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            shouldQuit = true;
        }

        // ====== ====== ======
        // TODO: (Q2)
        //  implement key presses (1-9) that replace the tween function
        //  with different alternate tween functions.
        //  Functions can be from lecture or from https://easings.net/#
        // ====== ====== ======

        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            // Key 1: Linear: Uniform
            if (keyPressed->scancode == sf::Keyboard::Scancode::Num1) {
                tween = [](float a, float b, float t) {
                    return (1 - t) * a + t * b;
                };
            }

            //Key2: Ease in quadratic: start slowly then speed up
            else if (keyPressed->scancode == sf::Keyboard::Scancode::Num2) {
                tween = [](float a, float b, float t) {
                    t = t * t;
                    return (1 - t) * a + t * b;
                };
            }

            // Key 3: Ease out quadratic: starts quickly and slow down near the end
            else if (keyPressed->scancode == sf::Keyboard::Scancode::Num3) {
                tween = [](float a, float b, float t) {
                    t = 1 - (1 - t) * (1 - t);
                    return (1 - t) * a + t * b;
                };
            }

            // Key 4: Ease in and out quadratic: slow then fast then slow
            else if (keyPressed->scancode == sf::Keyboard::Scancode::Num4) {
                tween = [](float a, float b, float t) {
                    if (t < 0.5f) {
                        t = 2 * t * t;
                    } else {
                        t = 1 - std::pow(-2 * t + 2, 2) / 2;
                    }

                    return (1 - t) * a + t * b;
                };
            }

            // Key 5: Ease in Cubic
            else if (keyPressed->scancode == sf::Keyboard::Scancode::Num5) {
                tween = [](float a, float b, float t) {
                    t = t * t * t;
                    return (1 - t) * a + t * b;
                };
            }

            // Key 6: Ease out Cubic
            else if (keyPressed->scancode == sf::Keyboard::Scancode::Num6) {
                tween = [](float a, float b, float t) {
                    t = 1 - std::pow(1 - t, 3);
                    return (1 - t) * a + t * b;
                };
            }

            // Key 7: Ease in out cubic
            else if (keyPressed->scancode == sf::Keyboard::Scancode::Num7) {
                tween = [](float a, float b, float t) {
                    if (t < 0.5f) {
                        t = 4 * t * t * t;
                    } else {
                        t = 1 - std::pow(-2 * t + 2, 3) / 2;
                    }
                    return (1 - t) * a + t * b;
                };
            }

            // Key 8: Sin ease in out
            else if (keyPressed->scancode == sf::Keyboard::Scancode::Num8) {
                tween = [](float a, float b, float t) {
                    t = (std::sin((t - 0.5f) * 3.14159265f) + 1) / 2;
                    return (1 - t) * a + t * b;
                };
            }

            // Key 9: ease in back
            else if (keyPressed->scancode == sf::Keyboard::Scancode::Num9) {
                tween = [](float a, float b, float t) {
                    const float c1 = 1.70158;
                    const float c3 = c1 + 1;
                    t = c3 * t * t * t - c1 * t * t;
                    return (1 - t) * a + t * b;
                };
            }
        }
    }
}

void render(sf::RenderWindow& window) {
    // Clear with blue background (sky)
    window.clear(sf::Color::Black);
    // ====== ====== ======
    // TODO: (Q1) Draw circle that moves between
    // the left/right half of the screen.
    // Movement should be governed by the tween function.
    // ====== ====== ======
    frame++; //count frames

    // Convert frame number into a value between 0 and 1
    // eg: frame = 1 -> t = (1% 120)/120 = 0.008 -> start
    //.    frame = 120 -> t = 0 -> start over when reach the other side
    //.    frame = 119 -> t = 0.992 -> close to the other side
    float t = (frame % FRAMES_PER_ANIMATION / static_cast<float>(FRAMES_PER_ANIMATION));

    //Get x position using the tween function
    float x = tween(100,700,t); // move from 100 to 700
    //Create the circle
    sf::CircleShape circle(18); // radius of 18
    // Put circle about 1/3 from the top
    circle.setFillColor(sf::Color::Blue);
    circle.setPosition({x, WINDOW_HEIGHT / 3.0f});

    // Draw circle
    window.draw(circle);


    // ====== ====== ======
    // TODO: (Q3) Draw tween function graph with a dot
    // on the current portion of the curve
    // ====== ====== ======
    
    // Where the graph goes
    float graphLeft = 80;
    float graphBottom = 650;
    float graphWidth = 550;
    float graphHeight = 150;

    // Draw X axis
    sf::VertexArray xAxis(sf::PrimitiveType::Lines, 2);
    xAxis[0].position = {graphLeft, graphBottom};
    xAxis[1].position = {graphLeft + graphWidth, graphBottom};
    xAxis[0].color = sf::Color::White;
    xAxis[1].color = sf::Color::White;
    window.draw(xAxis);

    // Draw Y axis
    // Y axis
    sf::VertexArray yAxis(sf::PrimitiveType::Lines, 2);
    yAxis[0].position = {graphLeft, graphBottom};
    yAxis[1].position = {graphLeft, graphBottom - graphHeight};
    yAxis[0].color = sf::Color::White;
    yAxis[1].color = sf::Color::White;
    window.draw(yAxis);

    // Draw tween curve
    sf::VertexArray curve(sf::PrimitiveType::LineStrip);

    // Loop through each t from 0 to 1:
    for (float sampleT=0; sampleT<=1.0f; sampleT+=0.01f) {
        // Get y value from the tween function
        float sampleY = tween(0,1,sampleT);

        // Convert graph coordinates to screen coordinates
        float screenX = graphLeft + sampleT * graphWidth; // normal horizontal x as SMFL
        float screenY = graphBottom - sampleY * graphHeight; // flip Y since the higher value of sample Y,
                                                            // should be shown higher, aka lower y valu in SMFL
        curve.append(sf::Vertex({screenX, screenY}, sf::Color::Blue));
    }
    window.draw(curve);

    // Draw moving dot
    float currentY = tween(0, 1, t); // this t in the current frame
    float dotX = graphLeft + t * graphWidth;
    float dotY = graphBottom - currentY * graphHeight; //same logic as above

    sf::CircleShape dot(5);
    dot.setFillColor(sf::Color::Yellow);
    dot.setPosition({dotX-5, dotY-5}); //setPosition init the dot at top left. We want the dot appear at the center of the line
    window.draw(dot);

    window.display();
}

int main() {
    sf::RenderWindow window;

    try {
        // Initialize window
        window.create(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Tween");
        window.setFramerateLimit(FPS_LIMIT);
        // Prevent key repeats.
        window.setKeyRepeatEnabled(false);

        bool shouldQuit = false;
        // Main game loop
        while (window.isOpen()) {
            handleInput(window, shouldQuit);
            if (shouldQuit) {
                break;
            }
            render(window);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
