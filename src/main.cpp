#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

constexpr uint8_t BUTTON_PIN = 3;
constexpr uint8_t BUZZER_PIN = 11;

// Custom graphics bitmaps
byte DINO1[8] = {
    0b00000,
    0b00111,
    0b00111,
    0b10110,
    0b11111,
    0b01010,
    0b01000,
    0b00000,
};

byte DINO2[8] = {
    0b00000,
    0b00111,
    0b00111,
    0b10110,
    0b11111,
    0b01010,
    0b00010,
    0b00000,
};

byte DINO[8] = {
    0b00000,
    0b00111,
    0b00111,
    0b10110,
    0b11111,
    0b01010,
    0b01010,
    0b00000,
};

byte CACTUS[8] = {
    0b00100,
    0b00101,
    0b10101,
    0b10101,
    0b10111,
    0b11100,
    0b00100,
    0b00000,
};

byte BIRD[8] = {
    0b00000,
    0b00100,
    0b01100,
    0b11110,
    0b00111,
    0b00110,
    0b00100,
    0b00000,
};

// Game variables
unsigned int score = 0;

byte dinoPosition = 1;
byte obstacle = 1;
byte dinoBit = 3;

int obstaclePosition[2] = {16, 16};  // int for negative boundaries
bool generate = true;

void setup() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(BUZZER_PIN, OUTPUT);

    lcd.init();
    lcd.backlight();

    lcd.createChar(0, BIRD);
    lcd.createChar(1, CACTUS);
    lcd.createChar(2, DINO);
    lcd.createChar(3, DINO1);
    lcd.createChar(4, DINO2);

    lcd.clear();

    // Splash screen
    lcd.setCursor(3, 0);
    lcd.print("Dino Game");

    lcd.setCursor(1, 0);
    lcd.write(byte(0));

    lcd.setCursor(13, 0);
    lcd.write(byte(0));

    lcd.setCursor(2, 1);
    lcd.write(byte(1));

    lcd.setCursor(7, 1);
    lcd.write(byte(2));

    lcd.setCursor(12, 1);
    lcd.write(byte(1));

    while (digitalRead(BUTTON_PIN));  // Wait for start press
}

void loop() {
    lcd.clear();

    // Spawn new obstacle
    if (generate) {
        obstacle = (random(5) > 1) ? 1 : 0;  // 1 = Cactus, 0 = Bird
        generate = false;
    }

    obstaclePosition[obstacle]--;

    // Move obstacle & check pass bounds
    if (obstaclePosition[obstacle] < 0) {
        obstaclePosition[obstacle] = 15;
        generate = true;
        score++;
        tone(BUZZER_PIN, 800, 150);
    }

    lcd.setCursor(obstaclePosition[obstacle], obstacle);
    lcd.write(byte(obstacle));

    // Handle dino input & run animation
    dinoPosition = digitalRead(BUTTON_PIN);  // 0 = Jump (Pressed), 1 = Ground

    lcd.setCursor(2, dinoPosition);
    lcd.write(byte(dinoBit));

    dinoBit ^= 7;  // Fast toggle between custom char 3 and 4

    // Collision detection
    if (dinoPosition == obstacle && obstaclePosition[obstacle] == 2) {
        lcd.clear();

        lcd.setCursor(3, 0);
        lcd.print("Game Over!");
        lcd.setCursor(1, 1);
        lcd.print("* Play Again *");

        tone(BUZZER_PIN, 500, 150);
        delay(1000);

        while (digitalRead(BUTTON_PIN));   // Wait for click
        while (!digitalRead(BUTTON_PIN));  // Wait for release (prevents instant jump)

        // Reset game state
        generate = true;
        obstaclePosition[0] = 16;
        obstaclePosition[1] = 16;
        score = 0;
    }

    // Display score
    lcd.setCursor(16 - String(score).length(), 0);
    lcd.print(score);

    // Dynamic game speed scaling
    delay(100 / (score * 0.07 + 1));
}
