/*Integrantes:

  Julian Rojas    julianrojas@unicauca.edu.co
  David Cardenas  jcardenass@unicauca.edu.co
  Andrés Sandino  asandino@unicauca.edu.co

*/ 
// -- no sabo--
#include "pinOut.h"
#include <Servo.h>
#include "ConstumChar.h"
uint64_t value = 0;
//------------
/********************************************//**
 *  Temperature sensor control functions
 ***********************************************/
#include "DHTStable.h"
DHTStable DHT;

/********************************************//**
 *  LCD control functions
 ***********************************************/
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);
#define DEBUG(a) Serial.print(millis()); Serial.print(": "); Serial.println(a);

/********************************************//**
 *  keypad
 ***********************************************/
#include <Keypad.h>
/* Keypad setup */
char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'.', '0', '=', 'D'}
};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);

/********************************************//**
 *  clave ,mensaje bienvenida, cursor, timeout
 ***********************************************/
void sistemaClave();
void mensajeBienvenida();
void actualizarCursor();
void tiempoSalida1();
void tiempoSalida2();
void tiempoSalida3();
/********************************************//**
 *  Asynchronous Task control functions
 ***********************************************/
#include "AsyncTaskLib.h"
void mostrarTemp();
void mostrarLuz();

AsyncTask asyncTaskTimeOut2Seg(2000, true, tiempoSalida1);
AsyncTask asyncTaskTimeOut10Seg(10000, true,  tiempoSalida2);
AsyncTask asyncTaskTimeOut6Seg(6000, true,  tiempoSalida3);
AsyncTask asyncTask1(2000, true,  mostrarTemp );
AsyncTask asyncTask2(1000, true, mostrarLuz );
AsyncTask asyncTaskSeguridad(500, true, sistemaClave);
/********************************************//**
 *  State Machine control functions
 ***********************************************/
#include "StateMachineLib.h"
/** An enum type. 
 *  define states in State Machine 
 */
enum State
{
  ingresoSeguridad = 0,
  eventoPuertaVentana = 1,
  monitorAmbiental = 2,
  alarmaAmbiental = 3,
  alertaSeguridad = 4
};

/** An enum type. 
 *  define Inputs in State Machine 
 */
enum Input
{
  senialUno = 0,
  senialDos = 1,
  senialTres = 2,
  senialCuatro = 3,
  Unknown = 4,
};

/*! Stores last user input */
Input currentInput;

/*! Create new StateMachine 4 states and 6 transitions */
StateMachine stateMachine(5, 8);

/********************************************//**

/********************************************//**
 *  sonidito melo
 ***********************************************/
 #include "sound.h"
void sonidoEntrar();
void sonidoErrado();
void sonidoBloqueado();
void inicializarComponentes();

/********************************************//**
 *  Define global variables
 ***********************************************/
int tempValue;
String inString = "";    // string to hold input


void setup() {
  lcd.createChar(0, Alien);
  lcd.createChar(1, Block);
  lcd.createChar(2, Unblock);
  lcd.createChar(3, Wrong);
  inicilizarComponentes();
  //asyncTask1.Start();
  //asyncTask2.Start();
  //asyncTaskSeguridad.Start();
  //mensajeBienvenida();
  //lcd.clear();
  setupStateMachine();  
   //asyncTaskSeguridad.Start();
  //Serial.println("Start Machine Started");
  stateMachine.SetState(ingresoSeguridad, false, true);
  //sistemaClave();
 
}

void loop() {
  actualizarCursor();
  if(!activarBloqueo){
    char key = keypad.getKey();
       if (key) {
            password[i++] = key;
            lcd.print('*');
          };
       if(i == 5){
          sistemaClave();
          i = 0;
        };
  };
  //asyncTaskSeguridad.Update();
}

void inicilizarComponentes(){
  Serial.begin(115200);
  lcd.begin(16, 2);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(BUZZER, OUTPUT);  // pin 8 como salida
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, LOW);
  lcd.cursor();
 
}

/*F**************************************************************************
* NAME: actualizarCursor
*----------------------------------------------------------------------------
* PARAMS:
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Define timeout_T1 function for to leaf the state ALERT_D
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void actualizarCursor() {
  if (millis() / 250 % 2 == 0 ) {
    lcd.cursor();
  } else {
    lcd.noCursor();
  }
}
/*F**************************************************************************
* NAME: timeout_T1
*----------------------------------------------------------------------------
* PARAMS:
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Define timeout_T1 function for to leaf the state ALERT_D
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void tiempoSalida1(){
  DEBUG("T1_END");
  currentInput = Input::senialDos;
  updateInputStateMachine();
}

void tiempoSalida2(){
  DEBUG("T2_END");
  currentInput = Input::senialUno;
  updateInputStateMachine();
}

void tiempoSalida3(){
  DEBUG("T3_END");
  currentInput = Input::senialUno;
  updateInputStateMachine();
}
/*F**************************************************************************
* NAME: sonidoEntrar
*----------------------------------------------------------------------------
* PARAMS:
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Define timeout_T1 function for to leaf the state ALERT_D
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void sonidoEntrar(){
for (int i = 0; i < 25; i++) {			// bucle repite 25 veces
      int duracion = 1000 / duraciones[i];		// duracion de la nota en milisegundos
      tone(BUZZER, melodia[i], duracion);	// ejecuta el tono con la duracion
      int pausa = duracion * 1.30;			// calcula pausa
      delay(pausa);					// demora con valor de pausa
      noTone(BUZZER);	
}
}
/*F**************************************************************************
* NAME: sonidoErrado
*----------------------------------------------------------------------------
* PARAMS:
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Define timeout_T1 function for to leaf the state ALERT_D
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void sonidoErrado(){
      tone(BUZZER, 1000, 100);
      delay(100);
      noTone(BUZZER);	
}
/*F**************************************************************************
* NAME: sonidoBloqueado
*----------------------------------------------------------------------------
* PARAMS:
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Define timeout_T1 function for to leaf the state ALERT_D
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void sonidoBloqueado(){
for (int i = 0; i < 5; i++) {			// bucle repite 25 veces
      int duracion = 1000 / duraciones[i];		// duracion de la nota en milisegundos
      tone(BUZZER, 400, duracion);	// ejecuta el tono con la duracion
      int pausa = duracion * 1.30;			// calcula pausa
      delay(pausa);					// demora con valor de pausa
      noTone(BUZZER);	
}
}
/*F**************************************************************************
* NAME: mostrarTemp
*----------------------------------------------------------------------------
* PARAMS:
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Define timeout_T1 function for to leaf the state ALERT_D
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void mostrarTemp(){
  Serial.println("nivel de humedad - temperatura");
  Serial.print("DHT11, \t");
  int chk = DHT.read11(DHT11_PIN);
  switch (chk)
  {
    case DHTLIB_OK:  
      Serial.print("OK,\t"); 
      break;
    case DHTLIB_ERROR_CHECKSUM: 
      Serial.print("Checksum error,\t"); 
      break;
    case DHTLIB_ERROR_TIMEOUT: 
      Serial.print("Time out error,\t"); 
      break;
    default: 
      Serial.print("Unknown error,\t"); 
      break;
  }
  Serial.print(DHT.getHumidity(), 1);
  lcd.setCursor(0, 0);
  lcd.print("H");
  lcd.setCursor(2, 0);
  lcd.print(DHT.getHumidity(), 1);
  lcd.setCursor(0, 1);
  lcd.print("T");
  lcd.setCursor(2, 1);
  Serial.print(",\t");
  tempValue = DHT.getTemperature();
  Serial.println(tempValue, 1);
  lcd.print(tempValue, 1);
}
/*F**************************************************************************
* NAME: mostrarLuz
*----------------------------------------------------------------------------
* PARAMS:
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Define timeout_T1 function for to leaf the state ALERT_D
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
  void mostrarLuz(void){
  int outputValue = 0;
   Serial.println("nivel de luz");
  outputValue = analogRead(photocellPin);
  lcd.setCursor(5, 0);
  lcd.print("L");
  lcd.setCursor(7, 0);
  lcd.print(outputValue);//print the temperature on lcd1602
  Serial.println(outputValue);
  lcd.setCursor(11, 0);
  lcd.print("");
}

