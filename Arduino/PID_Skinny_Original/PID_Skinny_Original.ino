///---Define's---///
#define MAX_SPEED 110   //Velocidad maxima de correccion fuera de una recta, valor maximo 255
#define MED_SPEED  90   //Velocidad en rectas, valor maximo 255
#define OUT_LINE   16   //valor dado para fuera de linea, no modificar
#define KP         20   //Valor de constante Kp del PID
#define KD         40   //Valor de constante Kd del PID
#define KI          1   //Valor de constante Ki del PID

enum {HOME,STARTING,RUN}state = HOME;//se enumeran home, starting y run con 0,1 y2. state 
enum {CENTER,RIGHT,LEFT} out_state = CENTER;
int sensor[5];

   //Local variables//
   signed int error_actual = 0;
    signed int error_anterior = 0;
    int error_sum = 0;
    int errores=0;
   
   signed int speed_1  =0; //Motor 2 - left side
    signed int speed_2  =0; //Motor 1 - right side
   
   signed int proporcional = 0;
    signed int derivativo = 0;
    signed int integrativo = 0;

    
void setup() {


  // Configuramos pines
pinMode(3, OUTPUT); //PWM1
pinMode(9, OUTPUT); //Dir1 PWM1
pinMode(10, OUTPUT);  //dir1 PWM2
pinMode(11, OUTPUT);  //PWM2

//pinMode(12, INPUT); //sensor
pinMode(13, OUTPUT); //led
pinMode(4, INPUT); //S2


//Serial.begin(9600);

analogWrite(3,0); //PWM1 487Hz  DC. de 0 a 255
analogWrite(11,0); //487Hz PWM2
digitalWrite(9, HIGH);//direccion PWM1
digitalWrite(10, HIGH); //Direccion PWM2


}

///---Functions---///
void Read_analog(void)
{
 
  sensor[0]= analogRead(A0);
  sensor[1]= analogRead(A1);
  sensor[2]= analogRead(A2);
  sensor[3]= analogRead(A3);
  sensor[4]= analogRead(A4);
  
}
///Function to read error///
signed int Read_error (void)
{
    int i,b0,b1,b2,b3,b4,umbral=600;
    signed int error = 0;
    Read_analog();
   for(i=0;i<5;i++)
   {
    if(sensor[i]>umbral)
    {
      sensor[i]=0;
    }
    else
    {
      sensor[i]=1;
    }
   }
   
   b3=!(sensor[0]);
   b1=!(sensor[1]);
   b0=!(sensor[2]);
   b2=!(sensor[3]);
   b4=!(sensor[4]);
  
   if (b0||b1||b2||b3||b4) {  //Out line - Black line (si alguno lee linea entra)
   
      /*Negative left sensor*/
        error = (b1)         ? (0-2) : error; //(0-2) operacion para que el comilador vea un -2
        error = (b3)         ? (0-4) : error;
        
        error = (b1 && b3)   ? (0-3) : error; //se leen sensores dobles al final
        
        
        
        
        /*Positive right sensor*/
        error = (b2)         ? 2 : error;
        error = (b4)         ? 4 : error;
        
        error = (b2 && b4)   ? 3 : error;
        
        
        
        /*Neutral middle sensor*/
        error = (b0)         ? 0 : error;
        error = (b0 && b1)   ? (0-1) : error;
        error = (b0 && b2)   ? 1 : error;
        
        out_state = ((error <= 2)&&(error >=(0-2)))  ? CENTER : out_state; //fuera de linea
        out_state = ((error >= 3)&&(error <=4))        ? LEFT : out_state;
        out_state = ((error <=(0-3))&&(error >=(0-4))) ? RIGHT: out_state;

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
   analogWrite(11,0);
   }
   else {
      speed = (speed >= MAX_SPEED) ? MAX_SPEED : speed;
      
      if (speed >=1){
      analogWrite(11,speed);  
      digitalWrite(10, LOW); //high adelante M1
      }
      else {
      speed *= (0-1);
      ////////////////////////
      speed = (speed >= MAX_SPEED) ? MAX_SPEED : speed;
      ///////////////////////////////////
      analogWrite(11,speed);
      digitalWrite(10, HIGH); //high adelante M1
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
            if(digitalRead(4) == 1){
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
                            speed_2 = (0-MED_SPEED);
                            break;
                        case RIGHT:
                            speed_1 = (0-MED_SPEED);
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
                    if(error_sum > 25) {// valor que se puede modificar para la acomulaicon de errores para el kd
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
