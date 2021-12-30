float RULERHEIGHT = 21;

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
