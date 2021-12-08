#include <LiquidCrystal.h>
#include <Keypad.h>

const int buzzer = 13;
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
enum OPSTATE
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
  OPSTATE state;
  int pin;

public:
  Motor(int pin);
  void turnOn()
  {
    this->state = ON;
  }
  void turnOff()
  {
    this->state = OFF;
  }
  OPSTATE getState()
  {
    return this->state;
  }
  ~Motor();
};

Motor::Motor(int pin)
{
  this->state = false;
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
  int getDistance();
};

Ultrasonic::Ultrasonic(int trigPin, int echoPin)
{
  this->trig = trigPin;
  this->echo = echoPin;
}

Ultrasonic::~Ultrasonic()
{
}

int Ultrasonic::getDistance()
{

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  float t = pulseIn(echo, HIGH, 28500);
  float distance = t / 57;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  return distance;
}

const int ROW_NUM = 4;
const int COLUMN_NUM = 3;

char keys[ROW_NUM][COLUMN_NUM] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};

byte pin_rows[ROW_NUM] = {8, A3, A4, A5};
byte pin_column[COLUMN_NUM] = {A0, A1, A2};

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

const int trig = 12, echo = 11;

ProgressState current_state = INPUT_PASSWORD;
const String real_password = "1234";
String input_password = "";

void getPassword()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Password:");
  lcd.setCursor(0, 1);
}

void setup()
{
  pinMode(buzzer, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  Serial.begin(9600);
  lcd.begin(16, 2);
  getPassword();
}

void clear_char(uint8_t input_length)
{
  lcd.setCursor(input_length - 1, 1);
  lcd.print(' ');
  lcd.setCursor(input_length - 1, 1);
}

void wrong_password_alert()
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
        Serial.println("Correct Password!");
      }
      else
      {
        Serial.println("Wrong Password!");
        wrong_password_alert();
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
    Serial.print("current input password: ");
    Serial.println(input_password);
  }
}
void handleMode()
{
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
        current_state = ProgressState::CHOOSE_MODE;
        Serial.println("Correct Password!");
      }
      else
      {
        Serial.println("Wrong Password!");
        wrong_password_alert();
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
    Serial.print("current input password: ");
    Serial.println(input_password);
  }
}

void handleMode()
{
  // handle the mood
}

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
}
void loop()
{

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
