
/*SENSOR TEST
PROGRAMA PARA MONITOREAR EL ESTADO DE LOS 11 SENSORES Y LOS DOS BOTONES DEL KIT LINE FOLLOWER FOR ARDUINO UNO V1.0
CREADA POR MARIANO CARRILLO
FEBRERO 2016
*/

///////////////////////////VARIABLES DECLARADAS//////////////////////////////////////////////////////////////////////////////////////////////////
int pin2 = 0,pin3=0,pin4=0,pin5=0,pin6=0,pin7=0,pin8=0,pin9=0,pin10=0,pin11=0,pin12=0,pin14=0,pin15=0,pin16=0,pin17=0,pin18=0,pin19=0;



void setup() {
/*  CONFIGURACION DE PINES UTILIZADOS EN EL KIT
PINES DE ENTRADA (SENSORES): EL KIT POSEE 11 SENSORES CUYA SEÑAL PASA A TRAVES DE UN COMPARADOR LM339 EL CUAL MEDIANTE UNA SEÑAL DE REFERENCIA MEDIANTE UN POTENCIOMETRO SE AJUSTA EL UMBRAL
PARA QUE SU SALIDA NOS ENTREGUE UN VALOR EN ALTO 5V O BIEN UN VALOR EN BAJO 0V, ESTE AJUSTE SE RECOMIENDA HACERLO PARA CALIBRAR LOS SENSORES DE ACUERDO A LAS CONDICIONES DE ILUMINACION Y DE
LOS DIFERENTES TONOS DE LA PISTA Y ASI LOS SENSORES SENSEN CORRECTAMENTE LA LINEA A SEGUIR.

*/
pinMode(2, INPUT);  //sensor
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
pinMode(18, INPUT); //Push de entrada S2 
pinMode(19, INPUT); //Push de entrada S1

/*PINES DE SALIDA: ESTOS PINES CORRESPONDEN A LAS SEÑALES DE PWM PARA LOS MOTORES ASI COMO LAS SEÑALES DE CONTROL PARA EL SENTIDO DE GIRO.
*/
pinMode(3, OUTPUT); //PWM1
pinMode(9, OUTPUT); //Direccion PWM1
pinMode(10, OUTPUT);  //PWM2
pinMode(11, OUTPUT);  //Direccion PWM2


/////////////COMUNICACION SERIAL///////////////////////////////////////////////////////////////////////////////////////////////
Serial.begin(9600);

///////////////////CONFIGURACION INICIAL DE MOTORES, VELOCIDADES A 0 //////////////////////////////////////////////////////////////////
analogWrite(3,0); //PWM1 487Hz  DC. de 0 a 255 (VELOCIDAD DEL MOTOR DE 0 A 255 DONDE 255 ES EL 100%)
analogWrite(10,0); //487Hz PWM2 DC. de 0 a 255 (VELOCIDAD DEL MOTOR DE 0 A 255 DONDE 255 ES EL 100%)
digitalWrite(9, HIGH);//direccion PWM1     ALTO --> ADELANTE           BAJO-->REVERSA
digitalWrite(11, HIGH); //Direccion PWM2   BAJO --> ADELANTE           ALTO -->REVERSA
}



void loop() {
  
/////////////////LECTURA CONTINUA DE SENSORES Y BOTONES/////////////////////////////////////////
pin2 = digitalRead(2);
pin4 = digitalRead(4);
pin5 = digitalRead(5); 
pin6 = digitalRead(6); 
pin7 = digitalRead(7);
pin8 = digitalRead(8);
pin12 = digitalRead(12);
pin14 = digitalRead(14);
pin15 = digitalRead(15);
pin16 = digitalRead(16);
pin17 = digitalRead(17);
pin18 = digitalRead(18);


///////////////////DEPLEGAMOS EN PANTALLA SU VALOR DIGITAL///////////////////////////////////////////
Serial.print("2:  ");
Serial.println(pin2);

Serial.print("4:  ");
Serial.println(pin4);

Serial.print("5:  ");
Serial.println(pin5);

Serial.print("6:  ");
Serial.println(pin6);

Serial.print("7:  ");
Serial.println(pin7);

Serial.print("8:  ");
Serial.println(pin8);

Serial.print("12:  ");
Serial.println(pin12);

Serial.print("14:  ");
Serial.println(pin14);

Serial.print("15:  ");
Serial.println(pin15);

Serial.print("16:  ");
Serial.println(pin16);

Serial.print("17:  ");
Serial.println(pin17);

Serial.print("S1:  ");
Serial.println(pin18);




Serial.println();
Serial.println();
delay(800);
}
