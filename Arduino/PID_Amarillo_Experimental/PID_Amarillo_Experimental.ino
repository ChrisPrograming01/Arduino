///---Define's---///
#define MAX_SPEED 150  //Velocidad maxima en curvas, valor maximo 255
#define MED_SPEED 130  //Velocidad en rectas, valor maximo 255
#define OUT_SPEED 120  //Velocidad de reaccion para cuando pierde totalmente la linea
#define OUT_LINE 16    //valor dado para fuera de linea, no modificar
#define KP 15          //Valor de constante Kp del PID
#define KD 80          //Valor de constante Kd del PID
#define KI 1           //Valor de constante Ki del PID.nhbgvx 
#define PWM1 3
#define PWM_DIR1 9
#define PWM2 11
#define PWM_DIR2 10
#define LED 13
#define S2 18

int PWM1 = 3;
int PWM_DIR1 = 9;
int PWM2 = 11;
int PWM_DIR2 = 10;
int LED = 13;
int S2 = 18;


enum { HOME, STARTING, RUN } state = HOME;  //se enumeran home, starting y run con 0,1 y2. state
enum { CENTER, RIGHT, LEFT } out_state = CENTER;
int Sensors[11];
int S[11];

//Local variables//
signed int error_actual = 0;
signed int error_anterior = 0;
int error_sum = 0;
int errores = 0;
int errorx = 0;

signed int speed_1 = 0;  //Motor 2 - left side
signed int speed_2 = 0;  //Motor 1 - right side
int cambio;

signed int proporcional = 0;
signed int derivativo = 0;
signed int integrativo = 0;

/*//Variables de los sensores de color
int S0 = 2;  //Sensor de hasta la izquierda
int S1 = 4;
int S2 = 5;
int S3 = 6;
int S4 = 7;
int S5 = 8;  //Sensor del medio
int S6 = 12;
int S7 = 14;
int S8 = 15;
int S9 = 16;
int S10 = 17;  //Sensor de hasta la derecha*/


void setup() {
  pinMode(PWM1, OUTPUT);  //PWM1
  pinMode(PWM_DIR1, OUTPUT);   //Direccion PWM1
  pinMode(PWM2, OUTPUT);  //PWM2
  pinMode(PWM_DIR2, OUTPUT);  //Direccion PWM2

  // Configuramos pines
  pinMode(A2, INPUT);   //sensor 2
  pinMode(A4, INPUT);   //Sensor
  pinMode(A5, INPUT);   //sensor
  pinMode(A6, INPUT);   //sensor
  pinMode(A7, INPUT);   //sensor
  pinMode(A8, INPUT);   //sensor
  pinMode(A12, INPUT);  //sensor
  pinMode(A14, INPUT);  //sensor
  pinMode(A15, INPUT);  //sensor
  pinMode(A16, INPUT);  //sensor
  pinMode(A17, INPUT);  //sensor 17
  
  
  pinMode(LED, OUTPUT);  //led
  pinMode(S2, INPUT);  //S2
  pinMode(19, INPUT);  //S1

  //Serial.begin(9600);

  analogWrite(PWM1, 0);       //PWM1 487Hz  DC. de 0 a 255
  analogWrite(PWM2, 0);      //487Hz PWM2
  digitalWrite(PWM_DIR1, HIGH);  //Direccion PWM1
  digitalWrite(PWM_DIR2, HIGH);  //Direccion PWM2
}

///---Functions---///
///Function to read error///
signed int Read_error(void) {
  //int b0,b1,b2,b3,b4,b5,b6,b7,b8,b9,b10;
  int error = 0;
  //Lee de la izquierda a la derecha
  S[0] = !(digitalRead(A2));
  S[1] = !(digitalRead(A4));
  S[2] = !(digitalRead(A5));
  S[3] = !(digitalRead(A6));
  S[4] = !(digitalRead(A7));
  S[5] = !(digitalRead(A8));
  S[6] = !(digitalRead(A12));
  S[7] = !(digitalRead(A14));
  S[8] = !(digitalRead(A15));
  S[9] = !(digitalRead(A16));
  S[10] = !(digitalRead(A17));


  if (S[0] || S[1] || S[2] || S[3] || S[4] || S[5] || S[6] || S[7] || S[8] || S[9] || S[10]) {  //Out line - Black line (si alguno lee linea entra)

    //Check for single sensor
    if (S[0] == 1) {
      error = -10;
    } else if (S[1] == 1) {
      error = -8;
    } else if (S[2] == 1) {
      error = -6;
    } else if (S[3] == 1) {
      error = -4;
    } else if (S[4] == 1) {
      error = -2;
    } else if (S[5] == 1) {
      error = 0;
    } else if (S[6] == 1) {
      error = 2;
    } else if (S[7] == 1) {
      error = 4;
    } else if (S[8] == 1) {
      error = 6;
    } else if (S[9] == 1) {
      error = 8;
    } else if (S[10] == 1) {
      error = 10;
    }

    if (S[0] == 1 && S[1] == 1) {
      error = -9;
    } else if (S[1] == 1 && S[2] == 1) {
      error = -7;
    } else if (S[2] == 1 && S[3] == 1) {
      error = -5;
    } else if (S[3] == 1 && S[4] == 1) {
      error = -3;
    } else if (S[4] == 1 && S[5] == 1) {
      error = -1;
    } else if (S[5] == 1 && S[6] == 1) {
      error = 1;
    } else if (S[6] == 1 && S[7] == 1) {
      error = 3;
    } else if (S[7] == 1 && S[8] == 1) {
      error = 5;
    } else if (S[8] == 1 && S[9] == 1) {
      error = 7;
    } else if (S[9] == 1 && S[10] == 1) {
      error = 9;
    }

    if (error <= 2 && error >= -2) {
      out_state = CENTER;
    } else if (error >= 5 && error <= 10) {
      out_state = LEFT;
    } else if (error <= -5 && error >= -10) {
      out_state = RIGHT;
    }

    return error;
  } else {
    return OUT_LINE;
  }
}

///Function to set speed of right motor///
void Motor_R(int speed) {
  if (!speed) {
    analogWrite(PWM1, 0);
    return;  // M1 Duty 0%
  }
  if ((errorx < -6) || (errorx > 6)) {
    speed = speed * 1.5;
    speed = constrain(speed, 0 ,MAX_SPEED);
  }
  if (speed >= 1) {
    analogWrite(PWM1, speed);
    digitalWrite(PWM_DIR1, HIGH);
  } else {
    speed *= -1;
    speed = constrain(speed, 0 ,MAX_SPEED);
    analogWrite(PWM1, speed);
    digitalWrite(PWM_DIR1, LOW);  //Cambio de giro
  }
  return;
}

///Function to set speed of left motor//
void Motor_L(int speed) {
  if (!speed) {
    analogWrite(PWM_DIR2, 0);
    return;
  }
  if ((errorx < -6) || (errorx > 6)) {
    speed = speed * 1.5;
    speed = constrain(speed, 0 ,MAX_SPEED);
  }
  if (speed >= 1) {
    analogWrite(PWM_DIR2, speed);
    digitalWrite(PWM2, LOW);  //high adelante M1
  } else {
    speed *= -1;
    speed = constrain(speed, 0 ,MAX_SPEED);
    analogWrite(PWM_DIR2, speed);
    digitalWrite(PWM2, HIGH);  //high adelante M1
  }
  return;
}

void loop() {

  switch (state) {
    case HOME:
      Motor_R(0);
      Motor_L(0);
      //output_high(PIN_G3);
      if (digitalRead(18) == 1) {
        state = STARTING;
      }
      delay(100);
      //output_low(PIN_G3);
      break;
    case STARTING:
      digitalWrite(LED, HIGH);
      delay(500);
      digitalWrite(LED, LOW);
      delay(500);
      digitalWrite(LED, HIGH);
      delay(300);
      state = RUN;
      Motor_R(50);
      Motor_L(50);
      delay(150);
      break;
    case RUN:
      //Read real error -10 to 10
      error_actual = Read_error();
      errorx = error_actual;
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
            speed_2 = (0 - OUT_SPEED);
            break;
          case RIGHT:
            speed_1 = (0 - OUT_SPEED);
            speed_2 = MAX_SPEED;
            break;
        }
      }
      //On line
      else {
        integrativo = (KI * errores);
        proporcional = (KP * error_actual);
        derivativo = (KD * (error_actual - error_anterior));

        error_sum++;
        if (error_sum > 200) {  //300 valor que se puede modificar para la acomulaicon de errores para el kd
          error_anterior = error_actual;
          error_sum = 0;
          errores += error_actual;
        }
        if (error_actual == 0) {
          errores = 0;
        }
        cambio = (proporcional + derivativo + integrativo)
        speed_1 = MED_SPEED + cambio;
        speed_2 = MED_SPEED - cambio;
      }

      Motor_R(speed_2);
      Motor_L(speed_1);

      break;
  }
}
