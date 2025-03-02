///---Define's---///
#define MAX_SPEED 150                                              //Velocidad maxima en curvas, valor maximo 255
#define MED_SPEED 130                                              //Velocidad en rectas, valor maximo 255
#define OUT_SPEED 120                                              //Velocidad de reaccion para cuando pierde totalmente la linea
#define OUT_LINE   16                                              //valor dado para fuera de linea, no modificar
#define KP         15                                              //Valor de constante Kp del PID
#define KD         80                                             //Valor de constante Kd del PID
#define KI          1                                              //Valor de constante Ki del PID


                                          
enum {HOME,STARTING,RUN}state = HOME;//se enumeran home, starting y run con 0,1 y2. state 
enum {CENTER,RIGHT,LEFT} out_state = CENTER;

int pin2 = 0,pin3=0,pin4=0,pin5=0,pin6=0,pin7=0,pin8=0,pin9=0,pin10=0,pin11=0,pin12=0,pin14=0,pin15=0,pin16=0,pin17=0,pin18=0,pin19=0;
   //Local variables//
   signed int error_actual = 0;
    signed int error_anterior = 0;
    int error_sum = 0;
    int errores=0;
    int errorx=0;
   
   signed int speed_1  =0; //Motor 2 - left side
    signed int speed_2  =0; //Motor 1 - right side
   
   signed int proporcional = 0;
    signed int derivativo = 0;
    signed int integrativo = 0;

    
void setup() {
  // Configuramos pines
pinMode(2, INPUT); //sensor
pinMode(3, OUTPUT); //PWM1
pinMode(4, INPUT);  //Sensor
pinMode(5, INPUT);  //sensor
pinMode(6, INPUT);  //sensor
pinMode(7, INPUT);  //sensor
pinMode(8, INPUT);  //sensor

pinMode(9, OUTPUT); //Direccion PWM1
pinMode(10, OUTPUT);  //PWM2
pinMode(11, OUTPUT);  //Direccion PWM2

pinMode(12, INPUT); //sensor
pinMode(13, OUTPUT); //led

pinMode(14, INPUT); //sensor
pinMode(15, INPUT); //sensor
pinMode(16, INPUT); //sensor
pinMode(17, INPUT); //sensor
pinMode(18, INPUT); //S2
pinMode(19, INPUT); //S1

//Serial.begin(9600);

analogWrite(3,0); //PWM1 487Hz  DC. de 0 a 255
analogWrite(10,0); //487Hz PWM2
digitalWrite(9, HIGH);//direccion PWM1
digitalWrite(11, HIGH); //Direccion PWM2


}

///---Functions---///
///Function to read error///
signed int Read_error (void)
{
    int b0,b1,b2,b3,b4,b5,b6,b7,b8,b9,b10;
    signed int error = 0;
   b0=!(digitalRead(8));
   b1=!(digitalRead(7));
   b3=!(digitalRead(6));
   b2=!(digitalRead(12));
   b4=!(digitalRead(14));
   b5=!(digitalRead(5));
   b6=!(digitalRead(15));
   b7=!(digitalRead(4));
   b8=!(digitalRead(16));
   b9=!(digitalRead(2));
   b10=!(digitalRead(17));

  
   if (b0||b1||b2||b3||b4||b5||b6||b7||b8||b9||b10) {  //Out line - Black line (si alguno lee linea entra)
   
      /*Negative left sensor*/
     // error = (b0 && b9)   ? (0-9) : error;
        error = (b1)         ? (0-2) : error; //(0-2) operacion para que el comilador vea un -2
        error = (b3)         ? (0-4) : error;
        error = (b5)         ? (0-6) : error;
        error = (b7)         ? (0-8) : error;
        error = (b9)         ? (0-10) : error;
        error = (b1 && b3)   ? (0-3) : error; //se leen sensores dobles al final
        error = (b3 && b5)   ? (0-5) : error;
        error = (b5 && b7)   ? (0-7) : error;
        
        error = (b7 && b9)   ? (0-9) : error;
        
        
        /*Positive right sensor*/
         //error = (b0 && b10)  ? 9 : error;
        error = (b2)         ? 2 : error;
        error = (b4)         ? 4 : error;
        error = (b6)         ? 6 : error;
        error = (b8)         ? 8 : error;
        error = (b10)        ? 10 : error;
        error = (b2 && b4)   ? 3 : error;
        error = (b4 && b6)   ? 5 : error;
        error = (b6 && b8)   ? 7 : error;
       
        error = (b8 && b10)  ? 9 : error;
        
        
        
        /*Neutral middle sensor*/
        error = (b0)         ? 0 : error;
        error = (b0 && b1)   ? (0-1): error;
        error = (b0 && b2)   ? 1 : error;
        
        out_state = ((error <= 2)&&(error >=(0-2)))  ? CENTER : out_state; //fuera de linea
        out_state = ((error >= 5)&&(error <=10))        ? LEFT : out_state;
        out_state = ((error <=(0-5))&&(error >=(0-10))) ? RIGHT: out_state;

        return error;
    }
      else {
    return OUT_LINE;
    }
}

///Function to set speed of right motor///
void Motor_R (signed int speed)
{
   if(!speed) {
   
   analogWrite(3,0); // M1 Duty 0% 
   
   }
   else {
     
     if((errorx<-6)||(errorx>6))
     {
       speed=speed*1.5;
     }
      speed = (speed >= MAX_SPEED) ? MAX_SPEED : speed;
      
      if (speed >=1){
      analogWrite(3,speed);  
      digitalWrite(9, HIGH);
            
      
      }
      else {
      speed *= (0-1);
      ////////////////////////
      speed = (speed >= MAX_SPEED) ? MAX_SPEED : speed;
      ///////////////////////////////////
      analogWrite(3,speed); 
      digitalWrite(9, LOW); //Cambio de giro
      
      }
   } 
   return;
}

///Function to set speed of left motor///
void Motor_L (signed int speed)
{  
   if(!speed) {
   analogWrite(10,0);
   }
   else {
     
     if((errorx<-6)||(errorx>6))
     {
       speed=speed*1.5;
     }
      speed = (speed >= MAX_SPEED) ? MAX_SPEED : speed;
      
      if (speed >=1){
      analogWrite(10,speed);  
      digitalWrite(11, LOW); //high adelante M1
      }
      else {
      speed *= (0-1);
      ////////////////////////
      speed = (speed >= MAX_SPEED) ? MAX_SPEED : speed;
      ///////////////////////////////////
      analogWrite(10,speed);
      digitalWrite(11, HIGH); //high adelante M1
      }
   }
   return;
}

void loop() {
  
switch (state)
      {
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
               switch (out_state)
                    {
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
            }
            //On line
            else {
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
                  
                    speed_1  = MED_SPEED + (proporcional + derivativo + integrativo);
                    speed_2  = MED_SPEED - (proporcional + derivativo + integrativo);
            }
            
            Motor_R(speed_2);
            Motor_L(speed_1);
            
            break;
      }
   

}
