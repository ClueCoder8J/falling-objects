#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cstdlib>
#include <ctime>
#include <string>
#include <iostream>

sf::Color randomColorNoGray() {
    sf::Uint8 r, g, b, a;
    do {
        r = std::rand() % 256;
        g = std::rand() % 256;
        b = std::rand() % 256;
        a = 255; 
    } while ((r > 100 && r < 160) && (g > 100 && g < 160) && (b > 100 && b < 160)); // No gray
    return sf::Color(r, g, b, a);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800,600), "Catch the Falling Objects"); // Creating a Window
    window.setFramerateLimit(60); // Setting the max frame rate

    // Creating a basket to catch objects
    sf::RectangleShape basket(sf::Vector2f(100.f, 20.f));
    basket.setFillColor(sf::Color(120, 0, 120));
    basket.setPosition(350.f, 550.f);

    // Seeding Random
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Creating a falling object
    sf::CircleShape fallingObject(15.f);
    fallingObject.setFillColor(randomColorNoGray());
    fallingObject.setPosition(static_cast<float>(std::rand() % 750), 0.f);

    // Creating special falling object
    sf::RectangleShape specialObject(sf::Vector2f(15.f, 15.f));
    specialObject.setFillColor(sf::Color(2, 5, 72, 50));
    specialObject.setPosition(-50.f, -50.f);

    float fallSpeed = 5.f; // Variable for object's fall speed
    float moveSpeed = 8.f;
    int score = 0;
    int lives = 3;
    bool gameOver = false;
    bool flashVisible = true;
    bool specialObjectActive = false;
    int lastSpecialScore = -1;
    sf::Clock flashTimer;

    sf::Font font;
    if (!font.loadFromFile("arial.ttf"))
    { return -1; }

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::Black);
    scoreText.setPosition(10.f, 10.f);

    sf::Text livesText;
    livesText.setFont(font);
    livesText.setCharacterSize(24);
    livesText.setFillColor(sf::Color::Black);
    livesText.setPosition(10.f, 40.f);

    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(64);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setString("GAME OVER");
    gameOverText.setPosition(200.f, 250.f);

    sf::SoundBuffer catchBuffer;
    if (!catchBuffer.loadFromFile("catch.mp3")) {
        return -1;
    }
    sf::Sound catchSound;
    catchSound.setBuffer(catchBuffer);

    // Loop for game
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            // Close window when X is pressed
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // Flashing game over text
        if (gameOver && flashTimer.getElapsedTime().asSeconds() > 0.5f) {
            flashVisible = !flashVisible;
            flashTimer.restart();
        }
        
        if (!gameOver) {
            // Input Handling: moving left & right
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                basket.move(-(moveSpeed), 0.f); // Move to the left
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                basket.move(moveSpeed, 0.f); // Move to the right
            }
        }

        // Disallow basket leaving screen
        if (basket.getPosition().x < 0) 
        {basket.setPosition(0.f, basket.getPosition().y);}
        if (basket.getPosition().x + basket.getSize().x > 800)
        {basket.setPosition(800 - basket.getSize().x, basket.getPosition().y);}

        if (!gameOver) {
            // Move object down
            fallingObject.move(0.f, fallSpeed);

            // Collision updating
            if (fallingObject.getGlobalBounds().intersects(basket.getGlobalBounds())) {
                score++;
                fallingObject.setPosition(static_cast<float>(std::rand() % 750), 0.f);
                fallingObject.setFillColor(randomColorNoGray());
                
                if (score % 10 == 0) {
                    fallSpeed += 1.f;
                }
                if (score % 25 == 0) {
                    fallSpeed += 2.f;
                }
                    

                catchSound.play();
            }
            else if(fallingObject.getPosition().y > 600) {
                lives--;
                fallingObject.setPosition(static_cast<float>(std::rand() % 750), 0.f);
                fallingObject.setFillColor(randomColorNoGray());
                if (lives <= 0) {
                    gameOver = true;
                }
        }
        
            // Special Object which increases movement speed and fall speed every 10 score, if caught
            if (!specialObjectActive && score % 10 == 0 && score != lastSpecialScore && score != 0) {
                specialObject.setPosition(static_cast<float>(std::rand() % 750), 0.f);
                specialObjectActive = true;
                lastSpecialScore = score;
            }
                if (specialObjectActive) {
                    specialObject.move(0.f, fallSpeed);

                    if (specialObject.getGlobalBounds().intersects(basket.getGlobalBounds())) { // When special object is caught
                        fallSpeed -= 2.f;
                        moveSpeed += 1.f;
                        lives += 1;
                        specialObjectActive = false;
                        specialObject.setPosition(-50.f, -50.f);
                    } else if (specialObject.getPosition().y > 600) { // When special object is not caught
                        specialObjectActive = false; 
                        specialObject.setPosition(-50.f, -50.f);
                    }
                }
            
        }    

        // Update score text
        scoreText.setString("Score: " + std::to_string(score));
        livesText.setString("Lives: " + std::to_string(lives));
        
        // Clear screen and draw objects
        window.clear(sf::Color(255, 255, 255, 255));
        window.draw(basket); 
        window.draw(fallingObject);
        if (specialObjectActive){
        window.draw(specialObject); }
        window.draw(scoreText);
        window.draw(livesText);

        if (gameOver && flashVisible) {
            window.draw(gameOverText);
        }

        window.display(); 

    }
    
    return 0;
}