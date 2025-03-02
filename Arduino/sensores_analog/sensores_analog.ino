
/*SENSOR TEST
PROGRAMA PARA MONITOREAR EL ESTADO DE LOS 11 SENSORES Y LOS DOS BOTONES DEL KIT LINE FOLLOWER FOR ARDUINO UNO V1.0
CREADA POR MARIANO CARRILLO
FEBRERO 2016
*/

///////////////////////////VARIABLES DECLARADAS//////////////////////////////////////////////////////////////////////////////////////////////////
int s1=0;


void setup() {
/*  CONFIGURACION DE PINES UTILIZADOS EN EL KIT
PINES DE ENTRADA (SENSORES): EL KIT POSEE 11 SENSORES CUYA SEÑAL PASA A TRAVES DE UN COMPARADOR LM339 EL CUAL MEDIANTE UNA SEÑAL DE REFERENCIA MEDIANTE UN POTENCIOMETRO SE AJUSTA EL UMBRAL
PARA QUE SU SALIDA NOS ENTREGUE UN VALOR EN ALTO 5V O BIEN UN VALOR EN BAJO 0V, ESTE AJUSTE SE RECOMIENDA HACERLO PARA CALIBRAR LOS SENSORES DE ACUERDO A LAS CONDICIONES DE ILUMINACION Y DE
LOS DIFERENTES TONOS DE LA PISTA Y ASI LOS SENSORES SENSEN CORRECTAMENTE LA LINEA A SEGUIR.

*/
pinMode(3, OUTPUT); //PWM1
pinMode(9, OUTPUT); //Dir1 PWM1
pinMode(10, OUTPUT);  //dir1 PWM2
pinMode(11, OUTPUT);  //PWM2
pinMode(13, OUTPUT); //led
pinMode(4, INPUT); //Push de entrada S1


/////////////COMUNICACION SERIAL///////////////////////////////////////////////////////////////////////////////////////////////
Serial.begin(9600);
///////////////////CONFIGURACION INICIAL DE MOTORES, VELOCIDADES A 0 ///////////////////
analogWrite(3,0); //PWM1 487Hz  DC. de 0 a 255
analogWrite(11,0); //487Hz PWM2
digitalWrite(9, HIGH);//direccion PWM1
digitalWrite(10, HIGH); //Direccion PWM2
}



void loop() {
  
/////////////////LECTURA CONTINUA DE SENSORES Y BOTONES/////////////////////////////////////////
  int sensor[5];
  sensor[0]= analogRead(A0);
  sensor[1]= analogRead(A1);
  sensor[2]= analogRead(A2);
  sensor[3]= analogRead(A3);
  sensor[4]= analogRead(A4);
  s1 = digitalRead(4);


///////////////////DEPLEGAMOS EN PANTALLA SU VALOR//////////////////////////////////////////
Serial.print("A0:  ");
Serial.println(sensor[0]);

Serial.print("A1:  ");
Serial.println(sensor[1]);

Serial.print("A2:  ");
Serial.println(sensor[2]);

Serial.print("A3:  ");
Serial.println(sensor[3]);

Serial.print("A4:  ");
Serial.println(sensor[4]);



Serial.print("S1:  ");
Serial.println(s1);




Serial.println();
Serial.println();
delay(800);
}
