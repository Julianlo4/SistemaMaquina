/*Integrantes:

  Julian Rojas    julianrojas@unicauca.edu.co
  David Cardenas  jcardenass@unicauca.edu.co
  Andrés Sandino  asandino@unicauca.edu.co

*/ 

#include "pinOut.h"
#include <Servo.h>
#include "ConstumChar.h"

/**********************************************
 *  Temperature sensor control functions
 ***********************************************/
#include "DHTStable.h"
DHTStable DHT;

/**********************************************
 *  LCD control functions
 ***********************************************/
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);
#define DEBUG(a) Serial.print(millis()); Serial.print(": "); Serial.println(a);

/*********************************************
 *  keypad control functions
 ***********************************************/
#include <Keypad.h>
char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'.', '0', '=', 'D'}
};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);

/**********************************************
 Update cursor control Task control functions
 ***********************************************/
void actualizarCursor();

/**********************************************
  Segurity control Task control functions
 ***********************************************/
void sistemaClave();

/**********************************************
 Message welcome control Task control functions
 ***********************************************/
void mensajeBienvenida();

/**********************************************
 Asynchronous Task control functions
 ***********************************************/
#include "AsyncTaskLib.h"
void mostrarTemp();
void mostrarLuz();
void tiempoSalida1();
void tiempoSalida2();
void tiempoSalida3();
void contarTiempo();
void sensores();

AsyncTask asyncTaskTimeOut2Seg(2000, tiempoSalida1);
AsyncTask asyncTaskTimeOut10Seg(10000,tiempoSalida2);
AsyncTask asyncTaskTimeOut6Seg(6000, tiempoSalida3);
AsyncTask asyncTaskContarSegundos(1000, true, contarTiempo );
AsyncTask asyncTask1(2000, true,  mostrarTemp );
AsyncTask asyncTask2(1000, true, mostrarLuz );
AsyncTask asyncTaskSeguridad(500,sistemaClave);
AsyncTask asyncTaskSensores(500, true ,sensores);
/**********************************************
 State Machine control functions
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
  senialZero = 0,
  senialUno = 1,
  senialDos = 2,
  senialTres = 3,
  senialCuatro = 4,
  Unknown = 5,
};

/*! Stores last user input */
Input currentInput;

/*! Create new StateMachine 5 states and 8 transitions */
StateMachine stateMachine(5, 8);

/**********************************************
 Sound control functions
 ***********************************************/
 #include "sound.h"
void sonidoEntrar();
void sonidoErrado();
void sonidoBloqueado();
void inicializarComponentes();

/**********************************************
 Define global variables
 ***********************************************/
int tempValue = 0;
String inString = "";    
uint64_t value = 0;
const byte temperaturaNormal = 30;
const byte temperaturaAlta = 32;
unsigned long tiempoActual = 0;
int segundos = 0;
bool activarBloqueo = false;
char password[6];
char passwordCorrecta[] = "12345";
unsigned char i = 0;
byte intentos = 3;

int SensorState=0;
int analogVal = 0;

void setup() {
  lcd.createChar(0, Alien);
  lcd.createChar(1, Block);
  lcd.createChar(2, Unblock);
  lcd.createChar(3, Wrong);
  inicilizarComponentes();
  setupStateMachine();  
  stateMachine.SetState(State::ingresoSeguridad, false, true);
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
        }
  };
  asyncTaskTimeOut2Seg.Update();
  asyncTaskTimeOut6Seg.Update();
  asyncTaskTimeOut10Seg.Update();
  asyncTask1.Update();
  asyncTask2.Update();
   asyncTaskSensores.Update();
  asyncTaskContarSegundos.Update();
  stateMachine.Update();
}

/**
  * @brief initialize components like pines,leds etc.
  * @param none
  * @return none
*/
void inicilizarComponentes(){
  Serial.begin(115200);
  lcd.begin(16, 2);
  pinMode(D0Pin, INPUT);
  pinMode(tracingPin, INPUT);
  pinMode(SensorPin,INPUT);

  pinMode(ledPin, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(BUZZER, OUTPUT); 
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, LOW);
  lcd.cursor();
 
}

/**
  * @brief Toggles the cursor on an LCD display based on a time-based condition
  * @param none
  * @return none
*/
void actualizarCursor() {
  if (millis() / 250 % 2 == 0 ) {
    lcd.cursor();
  } else {
    lcd.noCursor();
  }
}
/**
  * @brief Handles the completion of Time T1
  * @param none
  * @return none
*/
void tiempoSalida1(){
  Serial.print("tiempo 1");
  DEBUG("T1_END");
   lcd.clear();
  currentInput = Input::senialDos;
  updateInputStateMachine(currentInput);
}
/**
  * @brief Handles the completion of Time T2
  * @param none
  * @return none
*/
void tiempoSalida2(){
  DEBUG("T2_END");
  currentInput = Input::senialUno;
   lcd.clear();
  updateInputStateMachine(currentInput);
}
/**
  * @brief Handles the completion of Time T3
  * @param none
  * @return none
*/

void tiempoSalida3(){
  DEBUG("T3_END");
  currentInput = Input::senialUno;
  updateInputStateMachine(currentInput);
}

/**
  * @brief lets count the time
  * @param none
  * @return none
*/

void contarTiempo(){
  segundos++;
    if ( (tempValue > 28) && segundos >= 5){
    currentInput = Input::senialCuatro;
    updateInputStateMachine(currentInput);
    segundos = 0;
  } else if (tempValue < 28){
    currentInput = Input::senialDos;
    updateInputStateMachine(currentInput);
  } 
}
/**
  * @brief Plays a sequence of tones on a buzzer to create a sound effect
  * @param none
  * @return none
*/
void sonidoEntrar(){
for (int i = 0; i < 25; i++) {			// bucle repite 25 veces
      int duracion = 1000 / duraciones[i];		// duracion de la nota en milisegundos
      tone(BUZZER, melodia[i], duracion);	// ejecuta el tono con la duracion
      int pausa = duracion * 1.30;			// calcula pausa
      delay(pausa);					// demora con valor de pausa
      noTone(BUZZER);	
}
}
/**
  * @brief Plays an error sound on a buzzer
  * @param none
  * @return none
*/
void sonidoErrado(){
      tone(BUZZER, 1000, 100);
      delay(100);
      noTone(BUZZER);	
}
/**
  * @brief Plays a sequence of tones on a buzzer to create a "blocked" sound effect
  * @param none
  * @return none
*/
void sonidoBloqueado(){
for (int i = 0; i < 5; i++) {			// bucle repite 25 veces
      int duracion = 1000 / duraciones[i];		// duracion de la nota en milisegundos
      tone(BUZZER, 400, duracion);	// ejecuta el tono con la duracion
      int pausa = duracion * 1.30;			// calcula pausa
      delay(pausa);					// demora con valor de pausa
      noTone(BUZZER);	
}
}
/**
  * @brief Reads humidity and temperature data from a DHT11 sensor and displays it on both the Serial monitor and an LCD display
  * @param none
  * @return none
*/
void mostrarTemp(){
  lcd.clear();
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
      if(tempValue >28 ){
        asyncTaskTimeOut10Seg.Stop();
        currentInput = Input::senialTres;
        updateInputStateMachine(currentInput);   
    } else if( tempValue < 28) {
        asyncTaskTimeOut10Seg.Start();
    }
}
/**
  * @brief Reads the light level from a photocell and displays it on both the Serial monitor and an LCD display
  * @param void
  * @return none
*/
  void mostrarLuz(void){
  int outputValue = 0;
   Serial.println("nivel de luz");
  outputValue = analogRead(photocellPin);
  lcd.setCursor(6, 0);
  lcd.print("L");
  lcd.setCursor(8, 0);
  lcd.print(outputValue);//print the temperature on lcd1602
  Serial.println(outputValue);
  lcd.setCursor(11, 0);
  lcd.print("");
}
/**
  * @brief Display and return the digital value of the electric field sensor
  * @param none
  * @return The digital value of the electric field sensor
*/
int mostrarCampoElec(){
lcd.clear();
//int analogVal = analogRead(A0Pin);
int digitalVal = digitalRead(D0Pin);
lcd.setCursor(0,1);
lcd.print("Digital Value:");
lcd.print(digitalVal);
Serial.print("Digital Value:");
Serial.print(digitalVal);
return digitalVal;
}
/**
  * @brief Check if there is presence detected by the sensor
  * @param none
  * @return True if presence is detected, False otherwise
*/
bool mostrarPresencia(){
int val = digitalRead(tracingPin);
if(val == HIGH)
{
 digitalWrite(LED_RED, HIGH);
Serial.print("Si hay presencia");
return true;
}
else
{
 digitalWrite(LED_RED, LOW);
Serial.print("No hay presencia");
return false;
}
}
/**
  * @brief Check if there is metal touch detected by the sensor
  * @param none
  * @return True if metal touch is detected, False otherwise
*/
bool mostrarMetalTouch(){
analogVal = analogRead(analogIn);
SensorState=digitalRead(SensorPin);
if(SensorState==HIGH)
{
digitalWrite(ledPin,HIGH);
Serial.print("Si hay metal");
return true;
}
else
{
digitalWrite(ledPin,LOW);
Serial.print("No hay metal");
return false;
}
}
/**
  * @brief Activate sensors and update input state if any of the conditions are met
  * @param none
  * @return none
*/
void sensores(){
  int campoElec = mostrarCampoElec();
  bool presencia = mostrarPresencia();
  bool metalTouch = mostrarMetalTouch();
  if( campoElec == 1 || presencia || metalTouch){
    Serial.print("sensores activados");
    currentInput = Input::senialCuatro;
    updateInputStateMachine(currentInput);

 }

}
