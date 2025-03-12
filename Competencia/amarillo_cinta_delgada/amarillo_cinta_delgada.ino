///---Define's---///
#define MAX_SPEED 180                                             //Velocidad maxima en curvas, valor maximo 255
#define MED_SPEED 90                                              //Velocidad en rectas, valor maximo 255
#define OUT_SPEED 30                                              //Velocidad de reaccion para cuando pierde totalmente la linea
#define OUT_LINE  16                                              //valor dado para fuera de linea, no modificar
#define KP         1                                              //Valor de constante Kp del PID
#define KD        10                                              //Valor de constante Kd del PID
#define KI         0                                              //Valor de constante Ki del PID

                  
enum State {HOME,STARTING,RUN};
State state = HOME;
enum Out_state {CENTER,RIGHT,LEFT};
Out_state out_state = CENTER;

int pwm1=3;
int dir_pwm1=9;
int pwm2=10;
int dir_pwm2=11;

int pin2=0,pin3=0,pin4=0,pin5=0,pin6=0,pin7=0,pin8=0,pin9=0,pin10=0;
int pin11=0,pin12=0,pin14=0,pin15=0,pin16=0,pin17=0,pin18=0,pin19=0;

signed int error_actual = 0;
signed int error_anterior = 0;
int error_sum = 0;
int errores = 0;
int errorx = 0;
int cambio = 0;

signed int speed_1 = 0; //Motor 2 - left side
signed int speed_2 = 0; //Motor 1 - right side

signed int proporcional = 0;
signed int derivativo = 0;
signed int integrativo = 0;

    
void setup() {
  // Configuramos pines
  pinMode(2, INPUT); //sensor
  pinMode(4, INPUT);  //Sensor
  pinMode(5, INPUT);  //sensor
  pinMode(6, INPUT);  //sensor
  pinMode(7, INPUT);  //sensor
  pinMode(8, INPUT);  //sensor
  pinMode(12, INPUT); //sensor
  pinMode(14, INPUT); //sensor
  pinMode(15, INPUT); //sensor
  pinMode(16, INPUT); //sensor
  pinMode(17, INPUT); //sensor

  pinMode(pwm1, OUTPUT); //PWM1
  pinMode(dir_pwm1, OUTPUT); //Direccion PWM1
  pinMode(pwm2, OUTPUT);  //PWM2
  pinMode(dir_pwm2, OUTPUT);  //Direccion PWM2


  pinMode(13, OUTPUT); //led

  pinMode(18, INPUT); //S2
  pinMode(19, INPUT); //S1

  //Serial.begin(9600);

  analogWrite(pwm1,0); //PWM1 487Hz  DC. de 0 a 255
  analogWrite(pwm2,0); //487Hz PWM2
  digitalWrite(dir_pwm1, HIGH);//direccion PWM1
  digitalWrite(dir_pwm2, HIGH); //Direccion PWM2
}

///---Functions---///
///Function to read error///
int Read_error (void){
  int b1=0,b2=0,b3=0,b4=0,b5=0,b6=0,b7=0,b8=0,b9=0,b10=0;
  signed int error = 0;
  b1=!(digitalRead(2));
  b2=!(digitalRead(4));
  b3=!(digitalRead(5));
  b4=!(digitalRead(6));
  b5=!(digitalRead(7));
  b6=!(digitalRead(8)); // sensor central
  b7=!(digitalRead(12));
  b8=!(digitalRead(14));
  b9=!(digitalRead(15));
  b10=!(digitalRead(16));
  //b11=!(digitalRead(17));

  if (b1||b2||b3||b4||b5||b6||b7||b8||b9||b10) {  //Out line - Black line (si alguno lee linea entra)

    if (b1 == 1) {
      error = -9;
    } else if (b2 == 1) {
      error = -7;
    } else if (b3 == 1) {
      error = -5;
    } else if (b4 == 1) {
      error = -3;
    } else if (b5 == 1) {
      error = -1;
    } else if (b6 == 1) {
      error = 0; //Centro
    } else if (b7 == 1) {
      error = 1;
    } else if (b8 == 1) {
      error = 3;
    } else if (b9 == 1){
      error = 5;
    } else if (b10 == 1) {
      error = 7;
    } /* else if (b11 == 1) { El sensor 11 esta desactivado
      error = 9;
    } */

    if (b1 == 1 && b2 == 1) {
      error = -8;
    } else if (b2 == 1 && b3 == 1) {
      error = -6;
    } else if (b3 == 1 && b4 == 1) {
      error = -4;
    } else if (b4 == 1 && b5 == 1) {
      error = -2; 
    } else if (b5 == 1 && b6 == 1) {
      error = 0; //Centro
    } else if (b6 == 1 && b7 == 1) {
      error = 0; //Centro
    } else if (b7 == 1 && b8 == 1) {
      error = 2;
    } else if (b8 == 1 && b9 == 1) {
      error = 4;
    } else if (b9 == 1 && b10 == 1) {
      error = 6;
    } /* else if (b10 == 1 && b11 == 1) { El sensor 11 esta desactivado
      error = 8;
    } */
    
    if (error <= 3 && error >= -3) {
      out_state = CENTER;
    } else if (error >= 4 && error <= 12) {
      out_state = LEFT;
    } else if ( error <= -4 && error >= -12){
      out_state = RIGHT;
    }

    return error;
  } else {
    return OUT_LINE;
  }
}

///Function to set speed of right motor///
void Motor_R (signed int speed){
  if(!speed) {
    analogWrite(pwm1,0); // M1 Duty 0% 
  } else {
    if((errorx<-10)||(errorx>10)){
      speed=speed*1.5;
    }
    speed = (speed >= MAX_SPEED) ? MAX_SPEED : speed;
      
    if (speed >=1){
      speed = constrain(speed, 0, MAX_SPEED);
      analogWrite(pwm1,speed);  
      digitalWrite(dir_pwm1, HIGH);
    } else {
      speed*=-1;
      speed = constrain(speed, 0, MAX_SPEED);
      analogWrite(pwm1,speed); 
      digitalWrite(dir_pwm1, LOW);
    }
   } 
   return;
}

///Function to set speed of left motor///
void Motor_L (signed int speed){  
  if (!speed) {
    analogWrite(pwm2,0);
  } else {
    if((errorx<-10)||(errorx>10)){
      speed=speed*1.5;
    }
    
    if (speed >=1){
      speed = constrain(speed, 0, MAX_SPEED);
      analogWrite(pwm2,speed);  
      digitalWrite(dir_pwm2, LOW); //high adelante M1
    } else {
      speed *= -1;
      speed = constrain(speed, 0, MAX_SPEED);
      analogWrite(pwm2,speed);
      digitalWrite(dir_pwm2, HIGH); //high adelante M1
    }
  }
  return;
}

void loop() {
  switch (state){
    case HOME:
      Motor_R(0);
      Motor_L(0);
      //output_high(PIN_G3);
      if(digitalRead(18) == 1){
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
      Motor_R(50);
      Motor_L(50);
      delay(150);         
    break;
        
    case RUN:
      //Read real error -10 to 10
      error_actual = Read_error();
      errorx=error_actual;
      //Out line
      if(error_actual == OUT_LINE) {
          //Out line state machine//
        switch (out_state){
          case CENTER:
            speed_1 = MED_SPEED;
            speed_2 = MED_SPEED;
          break;
          case LEFT:
            speed_1 = MAX_SPEED;
            speed_2 = (0-OUT_SPEED);
          break;
          case RIGHT:
            speed_1 = (0-OUT_SPEED);
            speed_2 = MAX_SPEED;
          break;
        }
      } else {
        integrativo = (KI * errores);
        proporcional = (KP * error_actual);
        derivativo = (KD * (error_actual - error_anterior));
            
        error_sum ++;
        if(error_sum > 200) {//300 valor que se puede modificar para la acomulaicon de errores para el kd
            error_anterior = error_actual;
            error_sum = 0;
            errores += error_actual;
        }
        if(error_actual == 0) {
            errores = 0;
        }
      
        cambio = proporcional + derivativo + integrativo;
        speed_1  = MED_SPEED + cambio;
        speed_2  = MED_SPEED - cambio;
      }
      Motor_R(speed_2);
      Motor_L(speed_1);   
    break;
  }
}
