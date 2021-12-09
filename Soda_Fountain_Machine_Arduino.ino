#include <LiquidCrystal.h>
#include <Keypad.h>
float RULERHEIGHT = 21;
float CUP1 = 2;
float CUP2 = 3;
bool debugCode = false;
enum operationState
{
  OFF,
  ON
}; // operating state

enum ProgressState
{
  INPUT_PASSWORD,
  CHOOSE_MODE,
  POUR_MODE
};

class Motor
{
private:
  int pin;
  operationState state;

public:
  Motor(int pin);
  void turnOn()
  {
    this->state = operationState::ON;
    digitalWrite(this->pin, LOW);
  }
  void turnOff()
  {
    this->state = operationState::OFF;
    digitalWrite(this->pin, HIGH);
  }
  ~Motor();
};

Motor::Motor(int pin)
{
  this->state = operationState::OFF;
  this->pin = pin;
}

Motor::~Motor()
{
}

class Ultrasonic
{
private:
  int trig, echo;

public:
  Ultrasonic(int trigPin, int echoPin);
  ~Ultrasonic();
  float getDistance();
  float getHeight();
};

Ultrasonic::Ultrasonic(int trigPin, int echoPin)
{
  this->trig = trigPin;
  this->echo = echoPin;
}

Ultrasonic::~Ultrasonic()
{
}

float Ultrasonic::getDistance()
{

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  float t = pulseIn(echo, HIGH, 28500);
  float distance = t / 57;
  return distance;
}

float Ultrasonic::getHeight()
{

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  float t = pulseIn(echo, HIGH, 28500);
  float distance = t / 57;
  return RULERHEIGHT - distance;
}

// Global Variables section

// PINS Mapping
const int trig = 12, echo = 11;
const int buzzer = 13;
const int motor_pin = 10;
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
const int red_led = 0;
const int blue_led = 1;
//
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
Ultrasonic ultrasonic(trig, echo);
const int ROW_NUM = 4;
const int COLUMN_NUM = 3;

char keys[ROW_NUM][COLUMN_NUM] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};

byte pin_rows[ROW_NUM] = {8, A0, A1, A2};
byte pin_column[COLUMN_NUM] = {A3, A4, A5};

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);
Motor motor = Motor(motor_pin);

ProgressState current_state = INPUT_PASSWORD;
const String real_password = "1234";
String input_password = "";

bool first_time = true;
uint8_t cup_size;

void setup()
{
  if (debugCode)
  {

    Serial.begin(9600);
  }
  pinMode(buzzer, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(motor_pin, OUTPUT);
  pinMode(red_led, OUTPUT);
  pinMode(blue_led, OUTPUT);
  digitalWrite(motor_pin, HIGH);
  lcd.begin(16, 2);
}

void clear_char(uint8_t input_length)
{
  lcd.setCursor(input_length - 1, 1);
  lcd.print(' ');
  lcd.setCursor(input_length - 1, 1);
}

void alert()
{
  for (uint8_t i = 0; i < 3; i++)
  {
    lcd.noDisplay();
    tone(buzzer, 1000);
    delay(200);
    lcd.display();
    noTone(buzzer);
    delay(200);
  }
}
void handlePassword()
{
  if (first_time)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Password:");
    lcd.setCursor(0, 1);
    first_time = false;
  }

  char key = keypad.getKey();

  if (key)
  {
    switch (key)
    {
    case '*':
      if (input_password.length() > 0)
      {
        clear_char(input_password.length());
        input_password = input_password.substring(0, input_password.length() - 1);
      }
      break;
    case '#':
      if (input_password == real_password)
      {
        input_password = "";
        current_state = ProgressState::CHOOSE_MODE;
        first_time = true;
      }
      else
      {
        alert();
      }
      break;
    default:
      if (input_password.length() < 16)
      {
        input_password += key;
        lcd.print(key);
      }
      break;
    }
  }
}

void handleMode()
{
  if (first_time)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("1:Medium 2:Large");
    lcd.setCursor(0, 1);
    first_time = false;
  }

  char key = keypad.getKey();

  if (key)
  {
    switch (key)
    {
    case '1':
      cup_size = 1;
      current_state = ProgressState::POUR_MODE;
      first_time = true;
      break;
    case '2':
      cup_size = 2;
      current_state = ProgressState::POUR_MODE;
      first_time = true;
      break;
    default:
      alert();
      break;
    }
  }
}

long current_time = 0, motor_time = 0;
float first_height;
void pourCocaCola()
{

  // pour the coca
  // check for complete of the coca
  // store in the EEPROM
  /*
    while(within_time && difference in depth withing required filing){

    }
  
  */
  // in finished input password
  if (first_time)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Pouring drink ");
    lcd.setCursor(0, 1);

    if (cup_size == 1)
      lcd.print("Medium");
    else
      lcd.print("Large");

    first_time = false;

    motor.turnOn();
    motor_time = millis();
    first_height = ultrasonic.getHeight();
  }

  current_time = millis();
  lcd.setCursor(7, 1);
  lcd.print(((current_time - motor_time) / 1000) + 1);

  float heightDiff = first_height - ultrasonic.getHeight();
  lcd.setCursor(9, 1);
  lcd.print (heightDiff);
  if ((cup_size == 1 && heightDiff > CUP1 && current_time - motor_time >= 5000) || (cup_size == 2 && heightDiff > CUP2 && current_time - motor_time >= 10000))
  {
    motor.turnOff();
    current_state = ProgressState::INPUT_PASSWORD;
    first_time = true;
  }
}

long oldMillis = 0, currentMillis;
float current_height;
bool is_empty = false;
void loop()
{
  if (debugCode)
  {
    float distance = ultrasonic.getDistance();

    Serial.print("sonic distance is ");
    Serial.println(distance);
  }

  // lcd.clear();
  // lcd.setCursor(0, 0);
  // lcd.print("height is ");
  // lcd.setCursor(0, 1);
  // lcd.print(current_height);
  // delay(1000);
  // check if there is space
  currentMillis = millis();
  if (currentMillis - oldMillis > 100)
  {
    current_height = ultrasonic.getHeight();
    if (current_height < 3.5)
    {
      digitalWrite(red_led, HIGH);
      digitalWrite(blue_led, LOW);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("no coca available");

      input_password = "";
      current_state = ProgressState::INPUT_PASSWORD;
      motor.turnOff();

      is_empty = true;
    }
    else if (current_height > 4)
    {
      digitalWrite(red_led, LOW);
      digitalWrite(blue_led, HIGH);
      if (is_empty) {
      is_empty = false;
      first_time = true;
      }
    }
    oldMillis = currentMillis;
  }

  if (!is_empty)
    switch (current_state)
    {
    case ProgressState::INPUT_PASSWORD:
      handlePassword();
      break;

    case ProgressState::CHOOSE_MODE:
      handleMode();
      break;

    case ProgressState::POUR_MODE:
      pourCocaCola();
      break;
    default:
      break;
    }
}
