///---Define's---///
#define MAX_SPEED 135 //Velocidad maxima de correccion fuera de una recta, valor maximo 255
#define MED_SPEED 80  //Velocidad en rectas, valor maximo 255
#define OUT_LINE 10   //valor dado para fuera de linea, no modificar
#define KP 8       //Valor de constante Kp del PID , Entre mayor sea el valor mayor la oscialcion
#define KD 30    //Valor de constante Kd del PID , Entre mayor, mas micro-oscilaciones con decremento de velocidad
#define KI 0         //Valor de constante Ki del PID
#define SENSOR_TRESHOLD 800 //Valor donde cambia el estado de la lectura de los sensores
//30 velocidad

enum State { HOME, STARTING, RUN };
State state = HOME;  //se enumeran home, starting y run con 0,1 y2. state
enum OutState { CENTER, RIGHT, LEFT };
OutState out_state = CENTER;
int sensor[5];
int b[5];

//Local variables//
signed int error_actual = 0;
signed int error_anterior = 0;
int error_sum = 0;
int errores = 0;
int cambio = 0;

signed int speed_1 = 0;  //Motor 2 - left side
signed int speed_2 = 0;  //Motor 1 - right side

signed int proporcional = 0;
signed int derivativo = 0;
signed int integrativo = 0;

int PWM1 = 3;
int PWM_DIR1 = 9;
int PWM2 = 11;
int PWM_DIR2 = 10;
int LED = 13;
int S2 = 4;

void setup() {
  // Configuramos pines
  pinMode(PWM1, OUTPUT);   //PWM1
  pinMode(PWM_DIR1, OUTPUT);   //Dir1 PWM1
  pinMode(PWM2, OUTPUT);  //PWM2
  pinMode(PWM_DIR2, OUTPUT);  //dir1 PWM2

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);

  //pinMode(12, INPUT); //sensor
  pinMode(LED, OUTPUT);  //led
  pinMode(S2, INPUT);    //S2

  //Serial.begin(9600);
  analogWrite(PWM1, 0);          //PWM1 487Hz  DC. de 0 a 255
  analogWrite(PWM2, 0);           //487Hz PWM2
  digitalWrite(PWM_DIR1, HIGH);  //direccion PWM1
  digitalWrite(PWM_DIR2, HIGH);  //Direccion PWM2
}

///---Functions---///
void read_analog() {
  sensor[0] = analogRead(A0);
  sensor[1] = analogRead(A1);
  sensor[2] = analogRead(A2);
  sensor[3] = analogRead(A3);
  sensor[4] = analogRead(A4);
}

///Function to read error///
signed int read_error() {
  int i, umbral = SENSOR_TRESHOLD;
  signed int error = 0;

  read_analog();
  for (i = 0; i < 5; i++) {
    if (sensor[i] > umbral) {
      b[i] = 1;
    } else {
      b[i] = 0;
    }
  }

  if (b[0] || b[1] || b[2] || b[3] || b[4]) { //Si cualquier sensor esta en linea negra
    if (b[0] == 1) {
      error = -4;
    } else if (b[1] == 1) {
      error = -2;
    } if (b[2] == 1) {
      error = 0;
    } else if (b[3] == 1) {
      error = 2;
    } else if (b[4] == 1) {
      error = 4;
    }

    if (b[0] == 1 && b[1] == 1) {
      error = -3;
    } else if (b[1] == 1 && b[2] == 1) {
      error = -1;
    } else if (b[2] == 1 && b[3] == 1) {
      error = 1;
    } else if (b[3] == 1 && b[4] == 1) {
      error = 3;
    }

    
    if (error <= 2 && error >= -2) {
      out_state = CENTER;
    } else if (error >= 3) {
      out_state = LEFT;
    } else if (error <= -3) {
      out_state = RIGHT;
    }

    return error;
  } else { // Si ningun sensor esta tocando linea negra
    return OUT_LINE;
  }
}

///Function to set speed of right motor///
void motor_r(signed int speed) {
  if (!speed) {
    analogWrite(PWM2, 0); 
    return; // M1 Duty 0%
  }
  if (speed >= 1) {
    speed = constrain(speed, 0, MAX_SPEED);
    analogWrite(PWM1, speed);
    digitalWrite(PWM_DIR1, HIGH);
  } else {
    speed *= -1;
    speed = constrain(speed, 0, MAX_SPEED);
    analogWrite(PWM1, speed);
    digitalWrite(PWM_DIR1, LOW);  //Cambio de giro
  }
  return;
}

///Function to set speed of left motor///
void motor_l(signed int speed) {
  if (!speed) {
    analogWrite(PWM1, 0);
    return;
  }
  if (speed >= 1) {
    speed = constrain(speed, 0, MAX_SPEED);
    analogWrite(PWM2, speed);
    digitalWrite(PWM_DIR2, LOW);
  } else {
    //speed *= -1;
    speed = constrain(speed, 0, MAX_SPEED);
    analogWrite(PWM2, speed);
    digitalWrite(PWM_DIR2, HIGH);
  }
  return;
}

void loop() {

  switch (state) {
    case HOME:
      motor_r(0);
      motor_l(0);
      //output_high(PIN_G3);
      if (digitalRead(4) == 1) {
        state = STARTING;
      }
      delay(100);
      //output_low(PIN_G3);
      break;

    case STARTING:
      digitalWrite(13, HIGH);
      delay(500);
      digitalWrite(13, LOW);
      delay(500);
      digitalWrite(13, HIGH);
      delay(300);
      state = RUN;
      motor_r(50);
      motor_l(50);
      delay(150);
      break;

    case RUN:
      //Read real error -10 to 10
      error_actual = read_error();

      //Out line
      if (error_actual == OUT_LINE) {
        //Out line state machine//
        switch (out_state) {
          case CENTER:
            speed_1 = MED_SPEED;
            speed_2 = MED_SPEED;
            break;
          case LEFT:
            speed_1 = MAX_SPEED;
            speed_2 = (0 - MED_SPEED);
            break;
          case RIGHT:
            speed_1 = (0 - MED_SPEED);
            speed_2 = MAX_SPEED;
            break;
        }
      } else { //On line
        integrativo = KI * errores;
        proporcional = KP * error_actual;
        derivativo = KD * (error_actual - error_anterior);

        error_sum++;
        if (error_sum > 25) {  // valor que se puede modificar para la acomulaicon de errores para el kd
          error_anterior = error_actual;
          error_sum = 0;
          errores += error_actual;
        }
        if (error_actual == 0) {
          errores = 0;
        }
        cambio = proporcional + derivativo + integrativo;
        speed_1 = MED_SPEED + cambio;
        speed_2 = MED_SPEED - cambio;
      }

      motor_r(speed_2);
      motor_l(speed_1);

      break;
  }
}
