#include <LiquidCrystal.h>
#include <Keypad.h>

// LCD SETUP
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// KEYPAD SETUP 
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {13, 10, A0, A1};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// BUZZER
const int buzzer = A4;

// GAME VARIABLES
char target;
unsigned long startTime;

int score = 0;
int rounds = 0;
int difficultyLevel = 0;
int lives = 3;
bool gameActive = false;

// SETUP
void setup() {
  lcd.begin(16, 2);
  randomSeed(analogRead(A5));

  pinMode(buzzer, OUTPUT);

  showStartScreen();
}

//  LOOP 
void loop() {
  char key = keypad.getKey();

  if (!gameActive && key == '#') {
    startGame();
    return;
  }

  if (gameActive) {
    if (key == '#') {
      showStartScreen();
      return;
    }

    unsigned long reaction = millis() - startTime;
    unsigned long timeLimit = 99999;
    if (rounds >= 5) {
      timeLimit = 2000 - (difficultyLevel * 200);
      if (timeLimit < 500) timeLimit = 500;
    }

    // Timeout
    if (reaction > timeLimit) {
      lives--;
      playWrongTone();

      lcd.clear();
      lcd.print("Too Slow!");
      lcd.setCursor(0,1);
      lcd.print("L:");
      lcd.print(lives);
      lcd.print(" D:");
      lcd.print(difficultyLevel);
      lcd.print(" S:");
      lcd.print(score);

      delay(1200);
      checkGameOver();
      newRound();
      return;
    }

    if (key) {
      lcd.clear();
      if (key == target) {
        lcd.print("Correct!");
        lcd.setCursor(10,0);
        lcd.print(reaction);
        lcd.print("ms");

        playCorrectTone();
        score++;
      } else {
        lives--;
        lcd.print("Wrong!");
        lcd.setCursor(10,0);
        lcd.print(reaction);
        lcd.print("ms");

        playWrongTone();
      }

      lcd.setCursor(0,1);
      lcd.print("L:");
      lcd.print(lives);
      lcd.print(" D:");
      lcd.print(difficultyLevel);
      lcd.print(" S:");
      lcd.print(score);

      delay(1200);

      rounds++;
      if (rounds % 5 == 0) difficultyLevel++;

      checkGameOver();
      newRound();
    }
  }
}

// BUZZER FUNCTIONS 
void playStartTone() {
  tone(buzzer, 800, 150);
  delay(200);
  tone(buzzer, 1000, 150);
}

void playCorrectTone() {
  tone(buzzer, 1200, 100);
}

void playWrongTone() {
  tone(buzzer, 300, 200);
}

void playGameOverTone() {
  tone(buzzer, 400, 200);
  delay(250);
  tone(buzzer, 200, 400);
}

// START SCREEN 
void showStartScreen() {
  gameActive = false;
  rounds = 0;
  difficultyLevel = 0;
  score = 0;
  lives = 3;

  lcd.clear();
  lcd.print("Reaction Game");
  lcd.setCursor(0,1);
  lcd.print("Press # to start");
}

// START GAME 
void startGame() {
  gameActive = true;
  playStartTone();  // start sound
  countdownStart();
  newRound();
}

// COUNTDOWN 
void countdownStart() {
  lcd.clear();
  lcd.print("Get Ready...");
  delay(1000);

  for (int i = 3; i > 0; i--) {
    lcd.clear();
    lcd.print(i);
    delay(700);
  }

  lcd.clear();
}

// NEW ROUND
void newRound() {
  lcd.clear();
  char options[] = {'1','2','3','4','5','6','7','8','9','A','B','C','D'};
  target = options[random(0,13)];

  lcd.setCursor(0,0);
  lcd.print("Press: ");
  lcd.print(target);

  lcd.setCursor(0,1);
  lcd.print("L:");
  lcd.print(lives);
  lcd.print(" D:");
  lcd.print(difficultyLevel);
  lcd.print(" S:");
  lcd.print(score);

  startTime = millis();
}

// CHECK GAME OVER 
void checkGameOver() {
  if (lives <= 0) {
    showFinalScreen();
  }
}

// FINAL SCREEN
void showFinalScreen() {
  gameActive = false;

  playGameOverTone(); // game over sound

  lcd.clear();
  lcd.print("Game Over");

  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);

  delay(2000);

  lcd.clear();
  lcd.print("Press # to");
  lcd.setCursor(0, 1);
  lcd.print("restart");

  while (true) {
    char key = keypad.getKey();
    if (key == '#') {
      showStartScreen();
      break;
    }
  }
}