enum operationState
{
  OFF,
  ON
}; // operating state


class Motor
{
private:
  int pin;

public:
  Motor(int pin);
  operationState state;
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
