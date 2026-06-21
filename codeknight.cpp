#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

// structure to store match results
struct MatchResult {
    int matchNumber;
    std::string winner;
    std::string timestamp;
    int p1FinalHealth;
    int p2FinalHealth;
    int bulletsFired;

    MatchResult() : matchNumber(0), winner(""), timestamp(""), p1FinalHealth(0), p2FinalHealth(0), bulletsFired(0) {}
};

struct Projectile {
    sf::RectangleShape shape;
    float direction;
    sf::Vector2f velocity;

    Projectile() : direction(1.0f), velocity(12.0f, 0) {}
};

struct PowerUp {
    sf::CircleShape shape;
    bool active;
    sf::Clock spawnTimer;
    sf::Clock flashTimer;

    PowerUp() : active(false) {
        shape.setRadius(15.0f);
        shape.setOrigin(15.0f, 15.0f);
        shape.setFillColor(sf::Color::Green);
        shape.setOutlineThickness(3.0f);
        shape.setOutlineColor(sf::Color::White);
    }

    void spawn() {
        float x = 300.0f + static_cast<float>(rand() % 400);
        float y = 200.0f + static_cast<float>(rand() % 200);
        shape.setPosition(x, y);
        active = true;
        spawnTimer.restart();
        flashTimer.restart();
    }

    void update() {
        if (!active) return;

        // Flash effect
        if (flashTimer.getElapsedTime().asMilliseconds() > 200) {
            if (shape.getOutlineColor() == sf::Color::White)
                shape.setOutlineColor(sf::Color::Yellow);
            else
                shape.setOutlineColor(sf::Color::White);
            flashTimer.restart();
        }

        // Despawn after 10 seconds
        if (spawnTimer.getElapsedTime().asSeconds() > 10.0f) {
            active = false;
        }
    }

    void draw(sf::RenderWindow& window) const {
        if (active) {
            window.draw(shape);
        }
    }
};

class SoundManager {
private:
    sf::SoundBuffer shootBuffer;
    sf::SoundBuffer hitBuffer;
    sf::SoundBuffer jumpBuffer;
    sf::SoundBuffer powerupBuffer;
    sf::SoundBuffer deathBuffer;
    sf::SoundBuffer healBuffer;
    sf::SoundBuffer walkBuffer;

    sf::Sound shootSound;
    sf::Sound hitSound;
    sf::Sound jumpSound;
    sf::Sound powerupSound;
    sf::Sound deathSound;
    sf::Sound healSound;
    sf::Sound walkSound;

    sf::Music backgroundMusic;

public:
    SoundManager() {
        loadSounds();
    }

    void loadSounds() {
        // Create simple sounds programmatically
        createShootSound();
        createHitSound();
        createJumpSound();
        createPowerupSound();
        createDeathSound();
        createHealSound();
        createWalkSound();

        // Setup sounds
        shootSound.setBuffer(shootBuffer);
        hitSound.setBuffer(hitBuffer);
        jumpSound.setBuffer(jumpBuffer);
        powerupSound.setBuffer(powerupBuffer);
        deathSound.setBuffer(deathBuffer);
        healSound.setBuffer(healBuffer);
        walkSound.setBuffer(walkBuffer);

        // Configure sounds
        shootSound.setVolume(50.0f);
        hitSound.setVolume(60.0f);
        jumpSound.setVolume(50.0f);
        powerupSound.setVolume(70.0f);
        deathSound.setVolume(80.0f);
        healSound.setVolume(70.0f);
        walkSound.setVolume(30.0f);
        walkSound.setLoop(true);

        // Try to load background music
        if (!backgroundMusic.openFromFile("background.ogg")) {
            std::cout << "Note: Could not load background music.\n";
        }
        backgroundMusic.setVolume(30.0f);
        backgroundMusic.setLoop(true);
    }

    void createShootSound() {
        const unsigned SAMPLE_RATE = 44100;
        const unsigned DURATION = 100;
        const unsigned SAMPLES = SAMPLE_RATE * DURATION / 1000;

        sf::Int16 samples[SAMPLES];
        for (unsigned i = 0; i < SAMPLES; i++) {
            float time = static_cast<float>(i) / SAMPLE_RATE;
            samples[i] = static_cast<sf::Int16>(10000 * sin(2 * 3.14159f * 880 * time) * exp(-30 * time));
        }

        shootBuffer.loadFromSamples(samples, SAMPLES, 1, SAMPLE_RATE);
    }

    void createHitSound() {
        const unsigned SAMPLE_RATE = 44100;
        const unsigned DURATION = 200;
        const unsigned SAMPLES = SAMPLE_RATE * DURATION / 1000;

        sf::Int16 samples[SAMPLES];
        for (unsigned i = 0; i < SAMPLES; i++) {
            float time = static_cast<float>(i) / SAMPLE_RATE;
            samples[i] = static_cast<sf::Int16>(8000 * sin(2 * 3.14159f * 220 * time) * exp(-10 * time));
        }

        hitBuffer.loadFromSamples(samples, SAMPLES, 1, SAMPLE_RATE);
    }

    void createJumpSound() {
        const unsigned SAMPLE_RATE = 44100;
        const unsigned DURATION = 150;
        const unsigned SAMPLES = SAMPLE_RATE * DURATION / 1000;

        sf::Int16 samples[SAMPLES];
        for (unsigned i = 0; i < SAMPLES; i++) {
            float time = static_cast<float>(i) / SAMPLE_RATE;
            samples[i] = static_cast<sf::Int16>(5000 * sin(2 * 3.14159f * 330 * time) * exp(-20 * time));
        }

        jumpBuffer.loadFromSamples(samples, SAMPLES, 1, SAMPLE_RATE);
    }

    void createPowerupSound() {
        const unsigned SAMPLE_RATE = 44100;
        const unsigned DURATION = 800;
        const unsigned SAMPLES = SAMPLE_RATE * DURATION / 1000;

        sf::Int16 samples[SAMPLES];
        for (unsigned i = 0; i < SAMPLES; i++) {
            float time = static_cast<float>(i) / SAMPLE_RATE;
            samples[i] = static_cast<sf::Int16>(7000 * sin(2 * 3.14159f * 523.25f * time) *
                sin(2 * 3.14159f * 659.25f * time) *
                exp(-3 * time));
        }

        powerupBuffer.loadFromSamples(samples, SAMPLES, 1, SAMPLE_RATE);
    }

    void createDeathSound() {
        const unsigned SAMPLE_RATE = 44100;
        const unsigned DURATION = 1000;
        const unsigned SAMPLES = SAMPLE_RATE * DURATION / 1000;

        sf::Int16 samples[SAMPLES];
        for (unsigned i = 0; i < SAMPLES; i++) {
            float time = static_cast<float>(i) / SAMPLE_RATE;
            float freq = 220 * exp(-2 * time);
            samples[i] = static_cast<sf::Int16>(10000 * sin(2 * 3.14159f * freq * time) * exp(-5 * time));
        }

        deathBuffer.loadFromSamples(samples, SAMPLES, 1, SAMPLE_RATE);
    }

    void createHealSound() {
        const unsigned SAMPLE_RATE = 44100;
        const unsigned DURATION = 600;
        const unsigned SAMPLES = SAMPLE_RATE * DURATION / 1000;

        sf::Int16 samples[SAMPLES];
        for (unsigned i = 0; i < SAMPLES; i++) {
            float time = static_cast<float>(i) / SAMPLE_RATE;
            samples[i] = static_cast<sf::Int16>(6000 * sin(2 * 3.14159f * 392 * time) *
                sin(2 * 3.14159f * 523.25f * time) *
                exp(-4 * time));
        }

        healBuffer.loadFromSamples(samples, SAMPLES, 1, SAMPLE_RATE);
    }

    void createWalkSound() {
        const unsigned SAMPLE_RATE = 44100;
        const unsigned DURATION = 300;
        const unsigned SAMPLES = SAMPLE_RATE * DURATION / 1000;

        sf::Int16 samples[SAMPLES];
        for (unsigned i = 0; i < SAMPLES; i++) {
            float time = static_cast<float>(i) / SAMPLE_RATE;
            samples[i] = static_cast<sf::Int16>(3000 * (0.5 * sin(2 * 3.14159f * 100 * time) +
                0.3 * sin(2 * 3.14159f * 200 * time)) *
                exp(-10 * time));
        }

        walkBuffer.loadFromSamples(samples, SAMPLES, 1, SAMPLE_RATE);
    }

    // Public sound control methods
    void playShoot() { shootSound.play(); }
    void playHit() { hitSound.play(); }
    void playJump() { jumpSound.play(); }
    void playPowerup() { powerupSound.play(); }
    void playDeath() { deathSound.play(); }
    void playHeal() { healSound.play(); }
    void playWalk() { if (walkSound.getStatus() != sf::Sound::Playing) walkSound.play(); }
    void stopWalk() { walkSound.stop(); }

    void playBackgroundMusic() { backgroundMusic.play(); }
    void stopBackgroundMusic() { backgroundMusic.stop(); }
    void setMusicVolume(float volume) { backgroundMusic.setVolume(volume); }
};

class Player {
private:
    sf::Clock walkAnimationClock;
    float legAngle = 0.0f;
    bool walking = false;

public:
    sf::RectangleShape torso;
    sf::CircleShape head;
    sf::RectangleShape armR, armL, legR, legL;
    float velocityY = 0;
    bool onGround = true;
    int health = 100;
    bool facingRight = true;
    sf::Color color;
    std::string name;
    bool isAlive = true;
    sf::Clock damageFlashTimer;
    sf::Clock jumpAnimationTimer;
    bool isJumping = false;
    float jumpProgress = 0.0f;
    sf::Clock shootCooldownTimer;
    bool canShoot = true;
    const float SHOOT_COOLDOWN = 0.5f; // 0.5 seconds cooldown
    int shotsFired = 0; // Track number of shots fired

    void init(const std::string& playerName, sf::Vector2f pos, sf::Color col, bool startsRight) {
        name = playerName;
        color = col;
        facingRight = startsRight;
        shotsFired = 0;

        torso.setSize(sf::Vector2f(20.0f, 30.0f));
        torso.setOrigin(10.0f, 15.0f);
        torso.setFillColor(col);
        torso.setOutlineThickness(2.0f);
        torso.setOutlineColor(sf::Color::Black);

        head.setRadius(10.0f);
        head.setOrigin(10.0f, 10.0f);
        head.setFillColor(sf::Color(255, 220, 180));
        head.setOutlineThickness(1.0f);
        head.setOutlineColor(sf::Color::Black);

        armR.setSize(sf::Vector2f(15.0f, 5.0f));
        armR.setFillColor(col);
        armR.setOrigin(0.0f, 2.5f);

        armL.setSize(sf::Vector2f(15.0f, 5.0f));
        armL.setFillColor(col);
        armL.setOrigin(15.0f, 2.5f);

        legR.setSize(sf::Vector2f(6.0f, 12.0f));
        legR.setFillColor(sf::Color(40, 40, 40));
        legR.setOrigin(3.0f, 0.0f);

        legL.setSize(sf::Vector2f(6.0f, 12.0f));
        legL.setFillColor(sf::Color(40, 40, 40));
        legL.setOrigin(3.0f, 0.0f);

        torso.setPosition(pos);
        shootCooldownTimer.restart();
        canShoot = true;
        isAlive = true;
        health = 100;
        updateLayout();
    }

    void updateLayout() {
        if (!isAlive) return;

        sf::Vector2f p = torso.getPosition();
        head.setPosition(p.x, p.y - 25.0f);

        // Leg positions with walking animation
        float legOffset = 0.0f;
        if (walking) {
            legOffset = sin(walkAnimationClock.getElapsedTime().asSeconds() * 10.0f) * 15.0f;
        }

        legR.setPosition(p.x + 4.0f, p.y + 15.0f);
        legL.setPosition(p.x - 10.0f, p.y + 15.0f);

        // Leg animation
        if (walking) {
            legR.setRotation(legOffset);
            legL.setRotation(-legOffset);
        }
        else {
            legR.setRotation(0.0f);
            legL.setRotation(0.0f);
        }

        // Arm positions
        if (facingRight) {
            armR.setPosition(p.x + 10.0f, p.y - 2.0f);
            armL.setPosition(p.x, p.y + 4.0f);

            // Arm animation when walking
            if (walking) {
                float armOffset = sin(walkAnimationClock.getElapsedTime().asSeconds() * 10.0f) * 30.0f;
                armR.setRotation(armOffset);
                armL.setRotation(-armOffset);
            }
            else {
                armR.setRotation(30.0f);
                armL.setRotation(-30.0f);
            }
        }
        else {
            armR.setPosition(p.x - 10.0f, p.y - 2.0f);
            armL.setPosition(p.x, p.y + 4.0f);

            // Flip arms when facing left
            if (walking) {
                float armOffset = sin(walkAnimationClock.getElapsedTime().asSeconds() * 10.0f) * 30.0f;
                armR.setRotation(180 - armOffset);
                armL.setRotation(180 + armOffset);
            }
            else {
                armR.setRotation(150.0f);
                armL.setRotation(210.0f);
            }
        }

        // Jump animation
        if (isJumping) {
            float jumpHeight = sin(jumpProgress * 3.14159f) * 5.0f;
            head.move(0.0f, -jumpHeight);
            torso.move(0.0f, -jumpHeight);
            jumpProgress += 0.1f;
            if (jumpProgress >= 1.0f) {
                isJumping = false;
                jumpProgress = 0.0f;
            }
        }
    }

    void applyDamage(int damage, SoundManager& soundManager) {
        health -= damage;
        if (health <= 0) {
            health = 0;
            isAlive = false;
            soundManager.playDeath();
        }
        else {
            soundManager.playHit();
        }
        torso.setFillColor(sf::Color::White);
        damageFlashTimer.restart();
    }

    void heal(int amount, SoundManager& soundManager) {
        health += amount;
        if (health > 100) health = 100;
        soundManager.playHeal();

        // Healing effect - green flash
        torso.setFillColor(sf::Color::Green);
        damageFlashTimer.restart();
    }

    void update(float deltaTime = 1.0f) {
        if (!isAlive) return;

        // Update shoot cooldown
        if (!canShoot && shootCooldownTimer.getElapsedTime().asSeconds() >= SHOOT_COOLDOWN) {
            canShoot = true;
        }

        // Restore color after damage/healing flash
        if (damageFlashTimer.getElapsedTime().asMilliseconds() > 100) {
            torso.setFillColor(color);
        }

        velocityY += 0.8f;
        torso.move(0.0f, velocityY * deltaTime);

        // Boundary checking
        if (torso.getPosition().x < 20.0f) torso.setPosition(20.0f, torso.getPosition().y);
        if (torso.getPosition().x > 980.0f)
            torso.setPosition(980.0f, torso.getPosition().y);

        updateLayout();
    }

    void jump(SoundManager& soundManager) {
        if (onGround && isAlive) {
            velocityY = -15.0f;
            onGround = false;
            isJumping = true;
            jumpProgress = 0.0f;
            jumpAnimationTimer.restart();
            soundManager.playJump();
        }
    }

    bool shoot(SoundManager& soundManager) {
        if (canShoot && isAlive) {
            canShoot = false;
            shootCooldownTimer.restart();
            shotsFired++;
            soundManager.playShoot();
            return true;
        }
        return false;
    }

    float getCooldownProgress() const {
        if (canShoot) return 1.0f;
        float elapsed = shootCooldownTimer.getElapsedTime().asSeconds();
        return std::min(elapsed / SHOOT_COOLDOWN, 1.0f);
    }

    void startWalking(SoundManager& soundManager) {
        if (!walking) {
            walking = true;
            walkAnimationClock.restart();
            soundManager.playWalk();
        }
    }

    void stopWalking(SoundManager& soundManager) {
        walking = false;
        legR.setRotation(0.0f);
        legL.setRotation(0.0f);
        armR.setRotation(facingRight ? 30.0f : 150.0f);
        armL.setRotation(facingRight ? -30.0f : 210.0f);
        soundManager.stopWalk();
    }

    void draw(sf::RenderWindow& window) const {
        if (!isAlive) return;

        // Draw shadow
        sf::CircleShape shadow(15.0f);
        shadow.setFillColor(sf::Color(0, 0, 0, 100));
        shadow.setPosition(torso.getPosition().x - 15.0f, 540.0f);
        window.draw(shadow);

        window.draw(legR);
        window.draw(legL);
        window.draw(armL);
        window.draw(torso);
        window.draw(head);
        window.draw(armR);
    }

    sf::FloatRect getGlobalBounds() const {
        return torso.getGlobalBounds();
    }
};

// Function to get current timestamp - Fixed version
std::string getCurrentTimestamp() {
    time_t now = time(0);
    tm ltm;

    // Use localtime_s for safety (Windows)
#ifdef _WIN32
    localtime_s(&ltm, &now);
#else
// For non-Windows platforms, use localtime_r
    tm* ltm_ptr = localtime(&now);
    if (ltm_ptr) {
        ltm = *ltm_ptr;
    }
    else {
        // If localtime fails, return empty string
        return "Unknown Time";
    }
#endif

    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << ltm.tm_hour << ":"
        << std::setfill('0') << std::setw(2) << ltm.tm_min << ":"
        << std::setfill('0') << std::setw(2) << ltm.tm_sec << " "
        << std::setfill('0') << std::setw(2) << ltm.tm_mday << "/"
        << std::setfill('0') << std::setw(2) << (1 + ltm.tm_mon) << "/"
        << (1900 + ltm.tm_year);
    return ss.str();
}

// Function to save match result to file
void saveMatchResult(const MatchResult& result) {
    std::ofstream file("match_history.txt", std::ios::app);

    if (file.is_open()) {
        file << "Match #" << result.matchNumber << "\n";
        file << "Winner: " << result.winner << "\n";
        file << "Time: " << result.timestamp << "\n";
        file << "Player 1 Final Health: " << result.p1FinalHealth << "\n";
        file << "Player 2 Final Health: " << result.p2FinalHealth << "\n";
        file << "Total Bullets Fired: " << result.bulletsFired << "\n";
        file << "----------------------------------------\n";
        file.close();
        std::cout << "Match result saved to match_history.txt\n";
    }
    else {
        std::cerr << "Error: Could not open file for writing!\n";
    }
}

// Function to read match history from file
void displayMatchHistory(sf::RenderWindow& window, const sf::Font& font) {
    std::ifstream file("match_history.txt");

    if (!file.is_open()) {
        return;
    }

    std::string line;
    std::vector<std::string> history;

    while (getline(file, line)) {
        history.push_back(line);
    }
    file.close();

    // Display last 5 matches (5 matches * 6 lines each = 30 lines)
    int start = std::max(0, static_cast<int>(history.size()) - 30);
    sf::Text historyText;
    historyText.setFont(font);
    historyText.setCharacterSize(14);
    historyText.setFillColor(sf::Color::White);
    historyText.setPosition(20.0f, 150.0f);

    std::string displayText = "Last Matches:\n";
    for (int i = start; i < history.size(); i++) {
        displayText += history[i] + "\n";
    }

    historyText.setString(displayText);

    // Create a semi-transparent background for the history
    sf::RectangleShape historyBg(sf::Vector2f(300.0f, 400.0f));
    historyBg.setFillColor(sf::Color(0, 0, 0, 180));
    historyBg.setPosition(10.0f, 140.0f);

    window.draw(historyBg);
    window.draw(historyText);
}

// Function to get next match number from file
int getNextMatchNumber() {
    std::ifstream file("match_history.txt");
    if (!file.is_open()) {
        return 1; // First match if file doesn't exist
    }

    std::string line;
    int maxMatchNumber = 0;

    while (getline(file, line)) {
        if (line.find("Match #") == 0) {
            try {
                std::string numStr = line.substr(7);
                int num = std::stoi(numStr);
                if (num > maxMatchNumber) {
                    maxMatchNumber = num;
                }
            }
            catch (...) {
                // If conversion fails, continue
            }
        }
    }
    file.close();

    return maxMatchNumber + 1;
}











int main() {
    srand(static_cast<unsigned int>(time(0)));

    sf::RenderWindow window(sf::VideoMode(1000, 600), "CodeKnight");
    window.setFramerateLimit(60);

    // Initialize sound manager
    SoundManager soundManager;
    soundManager.playBackgroundMusic();

    // Create background with gradient
    sf::VertexArray background(sf::Quads, 4);
    background[0].position = sf::Vector2f(0, 0);
    background[1].position = sf::Vector2f(1000, 0);
    background[2].position = sf::Vector2f(1000, 600);
    background[3].position = sf::Vector2f(0, 600);
    background[0].color = sf::Color(20, 20, 40);
    background[1].color = sf::Color(30, 30, 60);
    background[2].color = sf::Color(10, 10, 30);
    background[3].color = sf::Color(20, 20, 40);

    // Ground with pattern
    sf::RectangleShape ground(sf::Vector2f(1000.0f, 50.0f));
    ground.setFillColor(sf::Color(50, 50, 70));
    ground.setPosition(0.0f, 550.0f);

    // Main platform
    sf::RectangleShape platform(sf::Vector2f(300.0f, 20.0f));
    platform.setFillColor(sf::Color(100, 100, 120));
    platform.setOutlineThickness(2.0f);
    platform.setOutlineColor(sf::Color(150, 150, 170));
    platform.setPosition(350.0f, 380.0f);

    // Elevated platform on left
    sf::RectangleShape leftPlatform(sf::Vector2f(150.0f, 15.0f));
    leftPlatform.setFillColor(sf::Color(80, 80, 100));
    leftPlatform.setOutlineThickness(2.0f);
    leftPlatform.setOutlineColor(sf::Color(130, 130, 150));
    leftPlatform.setPosition(150.0f, 300.0f);

    // Elevated platform on right
    sf::RectangleShape rightPlatform(sf::Vector2f(150.0f, 15.0f));
    rightPlatform.setFillColor(sf::Color(80, 80, 100));
    rightPlatform.setOutlineThickness(2.0f);
    rightPlatform.setOutlineColor(sf::Color(130, 130, 150));
    rightPlatform.setPosition(700.0f, 300.0f);

    Player p1, p2;
    p1.init("Player 1", sf::Vector2f(100.0f, 500.0f), sf::Color::Blue, true);
    p2.init("Player 2", sf::Vector2f(900.0f, 500.0f), sf::Color::Red, false);

    std::vector<Projectile> bullets;
    PowerUp shieldPowerUp;
    sf::Clock powerUpSpawnTimer;

    sf::Font font;
    bool fontLoaded = font.loadFromFile("C:/Windows/Fonts/arial.ttf");
    if (!fontLoaded) fontLoaded = font.loadFromFile("arial.ttf");

    sf::Text p1HealthText, p2HealthText, gameInfoText, powerUpText, soundText, cooldownText, historyText;
    if (fontLoaded) {
        p1HealthText.setFont(font);
        p2HealthText.setFont(font);
        gameInfoText.setFont(font);
        powerUpText.setFont(font);
        soundText.setFont(font);
        cooldownText.setFont(font);
        historyText.setFont(font);
    }
    p1HealthText.setCharacterSize(20);
    p1HealthText.setFillColor(sf::Color::Cyan);
    p1HealthText.setPosition(20.0f, 20.0f);

    p2HealthText.setCharacterSize(20);
    p2HealthText.setFillColor(sf::Color::Magenta);
    p2HealthText.setPosition(780.0f, 20.0f);

    gameInfoText.setCharacterSize(16);
    gameInfoText.setFillColor(sf::Color::White);
    gameInfoText.setPosition(300.0f, 5.0f);
    gameInfoText.setString("WASD - Move | SPACE - Shoot | ARROWS - Move | ENTER - Shoot | H - History");

    powerUpText.setCharacterSize(14);
    powerUpText.setFillColor(sf::Color::Green);
    powerUpText.setPosition(400.0f, 570.0f);

    soundText.setCharacterSize(12);
    soundText.setFillColor(sf::Color(150, 150, 150));
    soundText.setPosition(900.0f, 580.0f);
    soundText.setString("M: Music");

    cooldownText.setCharacterSize(14);
    cooldownText.setFillColor(sf::Color::Yellow);

    historyText.setCharacterSize(12);
    historyText.setFillColor(sf::Color(150, 150, 150));
    historyText.setPosition(20.0f, 580.0f);
    historyText.setString("H: Match History");

    bool gameRunning = true;
    bool p1Moving = false, p2Moving = false;
    bool musicMuted = false;
    bool showHistory = false;
    int matchNumber = getNextMatchNumber();
    bool matchResultSaved = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // RESTART - Check for R key press ANYTIME
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::R) {
                    // RESTART THE GAME
                    p1.init("Player 1", sf::Vector2f(100.0f, 500.0f), sf::Color::Blue, true);
                    p2.init("Player 2", sf::Vector2f(900.0f, 500.0f), sf::Color::Red, false);
                    bullets.clear();
                    shieldPowerUp.active = false;
                    gameRunning = true;
                    powerUpSpawnTimer.restart();
                    matchResultSaved = false; // Reset for new match
                    std::cout << "Game Restarted! Match #" << matchNumber << "\n";
                }

                // Music toggle
                if (event.key.code == sf::Keyboard::M) {
                    musicMuted = !musicMuted;
                    if (musicMuted) {
                        soundManager.setMusicVolume(0.0f);
                        soundText.setString("M: Music OFF");
                    }
                    else {
                        soundManager.setMusicVolume(30.0f);
                        soundText.setString("M: Music ON");
                    }
                }

                // History toggle
                if (event.key.code == sf::Keyboard::H) {
                    showHistory = !showHistory;
                }
            }
        }

        // --- POWER UP SPAWNING ---
        if (gameRunning) {
            if (!shieldPowerUp.active && powerUpSpawnTimer.getElapsedTime().asSeconds() > 15.0f) {
                shieldPowerUp.spawn();
                soundManager.playPowerup();
                powerUpSpawnTimer.restart();
            }
            shieldPowerUp.update();

            // Power-up collision
            if (shieldPowerUp.active) {
                if (shieldPowerUp.shape.getGlobalBounds().intersects(p1.getGlobalBounds())) {
                    p1.heal(20, soundManager);
                    shieldPowerUp.active = false;
                }
                if (shieldPowerUp.shape.getGlobalBounds().intersects(p2.getGlobalBounds())) {
                    p2.heal(20, soundManager);
                    shieldPowerUp.active = false;
                }
            }
        }

        // --- P1 MOVEMENT ---
        p1Moving = false;
        if (p1.isAlive) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && p1.torso.getPosition().x > 20.0f) {
                p1.torso.move(-5.0f, 0.0f);
                p1.facingRight = false;
                p1Moving = true;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && p1.torso.getPosition().x < 980.0f) {
                p1.torso.move(5.0f, 0.0f);
                p1.facingRight = true;
                p1Moving = true;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && p1.onGround) {
                p1.jump(soundManager);
            }

            if (p1Moving) {
                p1.startWalking(soundManager);
            }
            else {
                p1.stopWalking(soundManager);
            }

            // --- P1 SHOOTING with cooldown ---
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                if (p1.shoot(soundManager)) {
                    Projectile b;
                    b.shape.setSize(sf::Vector2f(10.0f, 4.0f));
                    b.shape.setFillColor(sf::Color::Yellow);
                    b.shape.setOutlineThickness(1.0f);
                    b.shape.setOutlineColor(sf::Color(255, 165, 0));
                    b.shape.setPosition(p1.torso.getPosition().x + (p1.facingRight ? 25.0f : -25.0f), p1.torso.getPosition().y - 2.0f);
                    b.direction = p1.facingRight ? 1.0f : -1.0f;
                    b.velocity = sf::Vector2f(b.direction * 12.0f, 0.0f);
                    bullets.push_back(b);
                }
            }
        }
        else {
            p1.stopWalking(soundManager);
        }

        // --- P2 MOVEMENT ---
        p2Moving = false;
        if (p2.isAlive) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && p2.torso.getPosition().x > 20.0f) {
                p2.torso.move(-5.0f, 0.0f);
                p2.facingRight = false;
                p2Moving = true;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && p2.torso.getPosition().x < 980.0f) {
                p2.torso.move(5.0f, 0.0f);
                p2.facingRight = true;
                p2Moving = true;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && p2.onGround) {
                p2.jump(soundManager);
            }

            if (p2Moving) {
                p2.startWalking(soundManager);
            }
            else {
                p2.stopWalking(soundManager);
            }

            // --- P2 SHOOTING with cooldown ---
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
                if (p2.shoot(soundManager)) {
                    Projectile b;
                    b.shape.setSize(sf::Vector2f(10.0f, 4.0f));
                    b.shape.setFillColor(sf::Color::Yellow);
                    b.shape.setOutlineThickness(1.0f);
                    b.shape.setOutlineColor(sf::Color(255, 165, 0));
                    b.shape.setPosition(p2.torso.getPosition().x + (p2.facingRight ? 25.0f : -25.0f), p2.torso.getPosition().y - 2.0f);
                    b.direction = p2.facingRight ? 1.0f : -1.0f;
                    b.velocity = sf::Vector2f(b.direction * 12.0f, 0.0f);
                    bullets.push_back(b);
                }
            }
        }
        else {
            p2.stopWalking(soundManager);
        }

        // --- PHYSICS ---
        p1.update();
        p2.update();

        for (auto* p : { &p1, &p2 }) {
            // Ground collision
            if (p->torso.getPosition().y >= 535.0f) {
                p->torso.setPosition(p->torso.getPosition().x, 535.0f);
                p->velocityY = 0.0f;
                p->onGround = true;
            }

            // Main platform collision
            if (p->torso.getGlobalBounds().intersects(platform.getGlobalBounds()) && p->velocityY > 0.0f) {
                if (p->torso.getPosition().y < platform.getPosition().y) {
                    p->torso.setPosition(p->torso.getPosition().x, platform.getPosition().y - 15.0f);
                    p->velocityY = 0.0f;
                    p->onGround = true;
                }
            }

            // Left platform collision
            if (p->torso.getGlobalBounds().intersects(leftPlatform.getGlobalBounds()) && p->velocityY > 0.0f) {
                if (p->torso.getPosition().y < leftPlatform.getPosition().y) {
                    p->torso.setPosition(p->torso.getPosition().x, leftPlatform.getPosition().y - 15.0f);
                    p->velocityY = 0.0f;
                    p->onGround = true;
                }
            }

            // Right platform collision
            if (p->torso.getGlobalBounds().intersects(rightPlatform.getGlobalBounds()) && p->velocityY > 0.0f) {
                if (p->torso.getPosition().y < rightPlatform.getPosition().y) {
                    p->torso.setPosition(p->torso.getPosition().x, rightPlatform.getPosition().y - 15.0f);
                    p->velocityY = 0.0f;
                    p->onGround = true;
                }
            }
        }

        // --- BULLET HIT DETECTION ---
        for (int i = 0; i < bullets.size(); ) {
            bullets[i].shape.move(bullets[i].velocity);
            bool hit = false;
            if (bullets[i].shape.getGlobalBounds().intersects(p1.getGlobalBounds())) {
                p1.applyDamage(10, soundManager);
                hit = true;
            }
            if (bullets[i].shape.getGlobalBounds().intersects(p2.getGlobalBounds())) {
                p2.applyDamage(10, soundManager);
                hit = true;
            }
            if (hit || bullets[i].shape.getPosition().x < 0.0f || bullets[i].shape.getPosition().x > 1000.0f)
                bullets.erase(bullets.begin() + i);
            else i++;
        }

        // Check game over and save result
        if (gameRunning && (!p1.isAlive || !p2.isAlive)) {
            gameRunning = false;

            // Save match result only once
            if (!matchResultSaved) {
                MatchResult result;
                result.matchNumber = matchNumber;
                result.winner = p1.isAlive ? "Player 1" : "Player 2";
                result.timestamp = getCurrentTimestamp();
                result.p1FinalHealth = p1.health;
                result.p2FinalHealth = p2.health;
                result.bulletsFired = p1.shotsFired + p2.shotsFired;

                saveMatchResult(result);
                matchResultSaved = true;
                matchNumber++; // Increment for next match
            }
        }

        // Update UI text
        std::stringstream ss1, ss2, powerUpTimer, cooldownInfo;
        ss1 << "P1: " << p1.health << " (Shots: " << p1.shotsFired << ")";
        ss2 << "P2: " << p2.health << " (Shots: " << p2.shotsFired << ")";
        p1HealthText.setString(ss1.str());
        p2HealthText.setString(ss2.str());

        if (shieldPowerUp.active) {
            int timeLeft = 10 - static_cast<int>(shieldPowerUp.spawnTimer.getElapsedTime().asSeconds());
            powerUpTimer << "Shield: " << timeLeft << "s";
        }
        else {
            int timeUntil = 15 - static_cast<int>(powerUpSpawnTimer.getElapsedTime().asSeconds());
            if (timeUntil > 0)
                powerUpTimer << "Next shield: " << timeUntil << "s";
            else
                powerUpTimer << "Shield incoming!";
        }
        powerUpText.setString(powerUpTimer.str());

        // Cooldown info
        float p1Cooldown = p1.getCooldownProgress();
        float p2Cooldown = p2.getCooldownProgress();

        std::string p1Ready = p1Cooldown >= 1.0f ? "READY" : "COOLING";
        std::string p2Ready = p2Cooldown >= 1.0f ? "READY" : "COOLING";

        // --- RENDER ---
        window.clear();

        // Draw background
        window.draw(background);

        // Draw ground and platforms
        window.draw(ground);
        window.draw(platform);
        window.draw(leftPlatform);
        window.draw(rightPlatform);

        // Health bars with background
        sf::RectangleShape healthBarBack1(sf::Vector2f(200.0f, 15.0f));
        healthBarBack1.setFillColor(sf::Color(50, 50, 50, 200));
        healthBarBack1.setPosition(20.0f, 45.0f);
        window.draw(healthBarBack1);

        sf::RectangleShape healthBar1(sf::Vector2f(static_cast<float>(p1.health) * 2.0f, 10.0f));
        healthBar1.setFillColor(sf::Color::Cyan);
        healthBar1.setPosition(25.0f, 47.5f);
        window.draw(healthBar1);

        sf::RectangleShape healthBarBack2(sf::Vector2f(200.0f, 15.0f));
        healthBarBack2.setFillColor(sf::Color(50, 50, 50, 200));
        healthBarBack2.setPosition(780.0f, 45.0f);
        window.draw(healthBarBack2);

        sf::RectangleShape healthBar2(sf::Vector2f(static_cast<float>(p2.health) * 2.0f, 10.0f));
        healthBar2.setFillColor(sf::Color::Magenta);
        healthBar2.setPosition(785.0f, 47.5f);
        window.draw(healthBar2);

        // Draw cooldown indicators
        sf::RectangleShape cooldownBack1(sf::Vector2f(100.0f, 10.0f));
        cooldownBack1.setFillColor(sf::Color(50, 50, 50, 200));
        cooldownBack1.setPosition(20.0f, 70.0f);
        window.draw(cooldownBack1);

        sf::RectangleShape cooldownBar1(sf::Vector2f(100.0f * p1Cooldown, 8.0f));
        cooldownBar1.setFillColor(p1Cooldown >= 1.0f ? sf::Color::Green : sf::Color(255, 165, 0));
        cooldownBar1.setPosition(21.0f, 71.0f);
        window.draw(cooldownBar1);

        sf::RectangleShape cooldownBack2(sf::Vector2f(100.0f, 10.0f));
        cooldownBack2.setFillColor(sf::Color(50, 50, 50, 200));
        cooldownBack2.setPosition(780.0f, 70.0f);
        window.draw(cooldownBack2);

        sf::RectangleShape cooldownBar2(sf::Vector2f(100.0f * p2Cooldown, 8.0f));
        cooldownBar2.setFillColor(p2Cooldown >= 1.0f ? sf::Color::Green : sf::Color(255, 165, 0));
        cooldownBar2.setPosition(781.0f, 71.0f);
        window.draw(cooldownBar2);

        // Draw text
        if (fontLoaded) {
            window.draw(p1HealthText);
            window.draw(p2HealthText);
            window.draw(gameInfoText);
            window.draw(powerUpText);
            window.draw(soundText);
            window.draw(historyText);

            // Cooldown status text
            cooldownText.setString(p1Ready);
            cooldownText.setPosition(25.0f, 85.0f);
            cooldownText.setFillColor(p1Cooldown >= 1.0f ? sf::Color::Green : sf::Color(255, 165, 0));
            window.draw(cooldownText);

            cooldownText.setString(p2Ready);
            cooldownText.setPosition(785.0f, 85.0f);
            cooldownText.setFillColor(p2Cooldown >= 1.0f ? sf::Color::Green : sf::Color(255, 165, 0));
            window.draw(cooldownText);
        }

        // Draw power-up
        shieldPowerUp.draw(window);

        // Draw players and bullets
        p1.draw(window);
        p2.draw(window);
        for (auto& b : bullets) window.draw(b.shape);

        // Display match history if H is pressed
        if (showHistory && fontLoaded) {
            displayMatchHistory(window, font);
        }

        // Game over screen
        if (!gameRunning) {
            sf::RectangleShape overlay(sf::Vector2f(1000.0f, 600.0f));
            overlay.setFillColor(sf::Color(0, 0, 0, 150));
            window.draw(overlay);

            if (fontLoaded) {
                sf::Text gameOverText;
                gameOverText.setFont(font);
                gameOverText.setCharacterSize(50);
                gameOverText.setFillColor(sf::Color::Yellow);
                gameOverText.setString(p1.isAlive ? "Player 1 Wins!" : "Player 2 Wins!");
                gameOverText.setPosition(350.0f, 200.0f);
                window.draw(gameOverText);

                sf::Text restartText;
                restartText.setFont(font);
                restartText.setCharacterSize(25);
                restartText.setFillColor(sf::Color::White);
                restartText.setString("Press R to restart");
                restartText.setPosition(380.0f, 280.0f);
                window.draw(restartText);

                // Show match number
                sf::Text matchText;
                matchText.setFont(font);
                matchText.setCharacterSize(20);
                matchText.setFillColor(sf::Color::Green);
                matchText.setString("Match #" + std::to_string(matchNumber - 1) + " saved to file!");
                matchText.setPosition(350.0f, 320.0f);
                window.draw(matchText);
            }
        }

        window.display();
    }
    return 0;
}
