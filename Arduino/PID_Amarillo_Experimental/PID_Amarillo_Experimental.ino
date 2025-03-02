 #define MAX_SPEED 150
#define MED_SPEED 130
#define OUT_SPEED 120
#define OUT_LINE 16
#define KP 15
#define KD 80
#define KI 1

#define PWM1 3
#define PWM_DIR1 9
#define PWM2 11
#define PWM_DIR2 10
#define LED_PIN 13
#define START_PIN 18

const uint8_t SENSOR_PINS[11] = {A2, A4, A5, A6, A7, A8, A12, A14, A15, A16, A17};
int S[11];

enum { HOME, STARTING, RUN } state = HOME;
enum { CENTER, RIGHT, LEFT } out_state = CENTER;

signed int error_actual = 0;
signed int speed_1 = 0, speed_2 = 0;
int errorx = 0;

void setup() {
  pinMode(PWM1, OUTPUT);
  pinMode(PWM_DIR1, OUTPUT);
  pinMode(PWM2, OUTPUT);
  pinMode(PWM_DIR2, OUTPUT);

  pinMode(LED_PIN, OUTPUT);
  pinMode(START_PIN, INPUT);
  for (uint8_t i = 0; i < 11; i++) pinMode(SENSOR_PINS[i], INPUT);
  
  analogWrite(PWM1, 0);
  analogWrite(PWM2, 0);
  digitalWrite(PWM_DIR1, HIGH);
  digitalWrite(PWM_DIR2, HIGH);
}

signed int Read_error(void) {
  int active_sensors = 0, sum_position = 0;
  for (int i = 0; i < 11; i++) {
    S[i] = !digitalRead(SENSOR_PINS[i]);
    if (S[i]) {
      sum_position += (i - 5) * 2; //Convierte un rango de (0,10) a (-10, +10)
      active_sensors++;
    }
  }
  if (active_sensors == 0) return OUT_LINE;
  int error = sum_position / active_sensors;
  if (error <= 2 && error >= -2) out_state = CENTER;
  else if (error >= 5) out_state = LEFT;
  else if (error <= -5) out_state = RIGHT;
  return error;
}

void setMotor(uint8_t pwmPin, uint8_t dirPin, int speed) {
  if (speed == 0) {
    analogWrite(pwmPin, 0);
    return;
  }
  bool forward = (speed > 0);
  speed = abs(speed);
  if (abs(errorx) > 6) speed = constrain(speed * 1.5, 0, MAX_SPEED);
  else speed = constrain(speed, 0, MAX_SPEED);
  analogWrite(pwmPin, speed);
  digitalWrite(dirPin, forward ? HIGH : LOW);
}

void loop() {
  static unsigned long lastTime = 0;
  static int startStep = 0;
  static signed int error_anterior = 0;
  static signed int integrativo = 0;
  unsigned long currentTime = millis();

  switch (state) {
    case HOME:
      setMotor(PWM1, PWM_DIR1, 0);
      setMotor(PWM2, PWM_DIR2, 0);
      if (digitalRead(START_PIN)) state = STARTING;
      break;

    case STARTING:
      if (startStep == 0 && currentTime - lastTime >= 500) {
        digitalWrite(LED_PIN, HIGH);
        lastTime = currentTime;
        startStep++;
      } else if (startStep == 1 && currentTime - lastTime >= 500) {
        digitalWrite(LED_PIN, LOW);
        lastTime = currentTime;
        startStep++;
      } else if (startStep == 2 && currentTime - lastTime >= 300) {
        digitalWrite(LED_PIN, HIGH);
        lastTime = currentTime;
        startStep = 0;
        state = RUN;
        setMotor(PWM1, PWM_DIR1, 50);
        setMotor(PWM2, PWM_DIR2, 50);
        delay(150);
      }
      break;

    case RUN:
      error_actual = Read_error();
      errorx = error_actual;

      if (error_actual == OUT_LINE) {
        switch (out_state) {
          case CENTER: speed_1 = speed_2 = MED_SPEED; break;
          case LEFT: speed_1 = MAX_SPEED; speed_2 = -OUT_SPEED; break;
          case RIGHT: speed_1 = -OUT_SPEED; speed_2 = MAX_SPEED; break;
        }
      } else {
        int proporcional = KP * error_actual;
        int derivativo = KD * (error_actual - error_anterior);
        integrativo = constrain(integrativo + (KI * error_actual), -50, 50);
        if (error_actual == 0) integrativo = 0;

        int cambio = proporcional + derivativo + integrativo;
        speed_1 = constrain(MED_SPEED + cambio, -MAX_SPEED, MAX_SPEED);
        speed_2 = constrain(MED_SPEED - cambio, -MAX_SPEED, MAX_SPEED);
        error_anterior = error_actual;
      }

      setMotor(PWM1, PWM_DIR1, speed_2);
      setMotor(PWM2, PWM_DIR2, speed_1);
      break;
  }
}
