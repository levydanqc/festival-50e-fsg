/*

  This is an replication of the game Green Light, Red Light.

  4 contestants must press a button at the sound of the Green Light alert and
  hold it until the Red Light alert is herd.

  Time is accumulated while they hold the button and the first to obtain a
  certain number is the winner (or multiple winners if they finish at the same
  time).

  The head of the doll will turn face to contestants while Red Light and face
  they other way while Green Light.

  Contestant must stop pressing the button as soon as the Red Light alert is
  herd or they will be disqualified from the game.

*/

#include <Adafruit_NeoPixel.h>
#include <Servo.h>

#define SERVO_PIN 6
#define RELAY1 9
#define RELAY2 10
#define RELAY3 11
#define RELAY4 12
#define BTN1_PIN 2
#define BTN2_PIN 3
#define BTN3_PIN 4
#define BTN4_PIN 5

#define LED_PIN 8
#define START_BTN_PIN 7

// #define BUZZER_PIN 8

#define REACTION_DELAY_MS 300
#define TOTAL_TIME_END 60000

Adafruit_NeoPixel pixels(1, LED_PIN, NEO_GRB + NEO_KHZ800);
Servo servo;

bool gameStarted = false;
bool losers[] = {0, 0, 0, 0};
unsigned long previousMillis = 0;
unsigned long interval = 0;
unsigned long startTime = 0;
unsigned long times[] = {0, 0, 0, 0};

byte greenLightCode = 97;
byte redLightCode = 98;

void setup() {
  Serial.begin(115200);

  servo.attach(SERVO_PIN);

  pinMode(BTN1_PIN, INPUT_PULLUP);
  pinMode(BTN2_PIN, INPUT_PULLUP);
  pinMode(BTN3_PIN, INPUT_PULLUP);
  pinMode(BTN4_PIN, INPUT_PULLUP);
  pinMode(START_BTN_PIN, INPUT_PULLUP);

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);

  pixels.begin();
  pixels.setPixelColor(0, pixels.Color(255, 255, 255));
  pixels.show();
}

void loop() {
  if (!gameStarted && !digitalRead(START_BTN_PIN)) {
    gameStarted = true;

    digitalWrite(RELAY1, 1);
    digitalWrite(RELAY2, 1);
    digitalWrite(RELAY3, 1);
    digitalWrite(RELAY4, 1);
  }

  if (!gameStarted) {
    return;
  }

  /*
  GAME SEQUENCE
  */

  if (losers[0] == 0 || losers[0] == 0 || losers[2] == 0 || losers[3] == 0) {
    pixels.setPixelColor(0, pixels.Color(255, 0, 0));
    pixels.show();
    servo.write(5);
    Serial.write(greenLightCode);

    // Wait a random number of seconds
    interval = random(3500, 7001);
    startTime = millis();

    // Read buttons + add to accumulated counter
    while (millis() <= startTime + interval) {
      if (!digitalRead(BTN1_PIN) && losers[0] == 0)
        times[0] += 1;
      if (!digitalRead(BTN2_PIN) && losers[1] == 0)
        times[1] += 1;
      if (!digitalRead(BTN3_PIN) && losers[2] == 0)
        times[2] += 1;
      if (!digitalRead(BTN4_PIN) && losers[3] == 0)
        times[3] += 1;
    }
  }

  // Stop game when someone has accumulated enough time
  if (times[0] >= TOTAL_TIME_END || times[1] >= TOTAL_TIME_END ||
      times[2] >= TOTAL_TIME_END || times[3] >= TOTAL_TIME_END ||
      (losers[0] == 1 && losers[1] == 1 && losers[2] == 1 && losers[3] == 1)) {
    digitalWrite(RELAY1, 0);
    digitalWrite(RELAY2, 0);
    digitalWrite(RELAY3, 0);
    digitalWrite(RELAY4, 0);
    pixels.setPixelColor(0, pixels.Color(0, 0, 255));
    pixels.show();

    // Determine the winner(s) by comparing times
    unsigned long maxTime =
        max(max(times[0], times[1]), max(times[2], times[3]));

    if (times[0] == maxTime && losers[0] == 0)
      digitalWrite(RELAY1, HIGH);
    if (times[1] == maxTime && losers[1] == 0)
      digitalWrite(RELAY2, HIGH);
    if (times[2] == maxTime && losers[2] == 0)
      digitalWrite(RELAY3, HIGH);
    if (times[3] == maxTime && losers[3] == 0)
      digitalWrite(RELAY4, HIGH);

    while (!digitalRead(START_BTN_PIN))
      ;

    gameStarted = false;
    losers[0] = losers[1] = losers[2] = losers[3] = 0;
    times[0] = times[1] = times[2] = times[3] = 0;

    return;
  }

  pixels.setPixelColor(0, pixels.Color(0, 255, 0));
  pixels.show();
  servo.write(175);
  Serial.write(redLightCode);

  interval = random(4000, 8001);
  startTime = millis();

  // Wait to take in account the human reaction delay
  delay(REACTION_DELAY_MS);

  // Read buttons + disqualify losers
  while (millis() <= startTime + interval) {
    if (!digitalRead(BTN1_PIN)) {
      digitalWrite(RELAY1, 0);
      losers[0] = 1;
    }
    if (!digitalRead(BTN2_PIN)) {
      digitalWrite(RELAY2, 0);
      losers[1] = 1;
    }
    if (!digitalRead(BTN3_PIN)) {
      digitalWrite(RELAY3, 0);
      losers[2] = 1;
    }
    if (!digitalRead(BTN4_PIN)) {
      digitalWrite(RELAY4, 0);
      losers[3] = 1;
    }
  }
}
