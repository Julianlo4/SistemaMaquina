
/*F**************************************************************************
* NAME: setupStateMachine
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Setup the State Machine
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void setupStateMachine()
{
  // Add transitions
  stateMachine.AddTransition(ingresoSeguridad, eventoPuertaVentana, []() { return currentInput == senialUno; });
  
  stateMachine.AddTransition(eventoPuertaVentana, monitorAmbiental, []() { return currentInput == senialDos; });
  stateMachine.AddTransition(eventoPuertaVentana, alertaSeguridad, []() { return currentInput == senialTres; });

  stateMachine.AddTransition(monitorAmbiental, eventoPuertaVentana, []() { return currentInput == senialUno; });
  stateMachine.AddTransition(monitorAmbiental,alarmaAmbiental, []() { return currentInput == senialCuatro; });

  stateMachine.AddTransition(alarmaAmbiental, monitorAmbiental, []() { return currentInput == senialDos; });
  stateMachine.AddTransition(alarmaAmbiental, alertaSeguridad, []() { return currentInput == senialTres; });
  
  stateMachine.AddTransition(alertaSeguridad, eventoPuertaVentana, []() { return currentInput == senialDos; });

  // Add actions
  stateMachine.SetOnEntering(ingresoSeguridad, mensajeBienvenida);
  stateMachine.SetOnEntering(eventoPuertaVentana, mensajeEvento);
  stateMachine.SetOnEntering(monitorAmbiental, mensajeMonitor);
  stateMachine.SetOnEntering(alarmaAmbiental, mensajeAlarma);
  stateMachine.SetOnEntering(alertaSeguridad, mensajeAlerta);

  stateMachine.SetOnLeaving(ingresoSeguridad, salidaSeguridad);
  stateMachine.SetOnLeaving(eventoPuertaVentana, salidaEvento);
  stateMachine.SetOnEntering(monitorAmbiental, salidaMonitor);
  stateMachine.SetOnLeaving(alarmaAmbiental, salidaAlarma);
  stateMachine.SetOnLeaving(alertaSeguridad, salidaAlerta);
}

/*F**************************************************************************
* NAME: readData
*----------------------------------------------------------------------------
* PARAMS:   nonce
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Read data from serial port
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void readData(){
  
  bool flagData = false;
  
  while (Serial.available() > 0) {
    int inChar = Serial.read();
    if (isDigit(inChar)) {
      // convert the incoming byte to a char and add it to the string:
      inString += (char)inChar;
    }
    // if you get a newline, print the string, then the string's value:
    if (inChar == '\n') {
      Serial.print("Value:");
      tempValue = inString.toInt();
      flagData = true;
      Serial.println(tempValue);
      Serial.print("String: ");
      Serial.println(inString);
      // clear the string for new input:
      inString = "";
    }
  }

  if (flagData){
    updateInputStateMachine();
  }

}

/*F**************************************************************************
* NAME: sistemaClave
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Implements a password-based system to control access and performs actions based on the entered password
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void sistemaClave(){
 lcd.clear();
 lcd.print("Ingrese clave");
 lcd.setCursor(0, 1);
  if(i==5){
      if(strcmp(password, passwordCorrecta) ==0){
            lcd.clear();
            lcd.print("Clave Correcta");
            lcd.setCursor(7, 1);
            lcd.write(byte(2));
            digitalWrite(LED_RED, LOW);
            digitalWrite(LED_BLUE, LOW);
            digitalWrite(LED_GREEN, HIGH);
            sonidoEntrar();
            lcd.clear();
            currentInput = Input::senialUno;
            updateInputStateMachine();
         }  else {
              lcd.clear();
              digitalWrite(LED_GREEN, LOW);
              digitalWrite(LED_BLUE, LOW);
              digitalWrite(LED_RED, HIGH);
              sonidoErrado();
              lcd.print("Clave Incorrecta"); 
              lcd.setCursor(7, 1);
              lcd.write(byte(3));
              lcd.clear();
              lcd.print("Ingrese clave");
              lcd.setCursor(0, 1);
              intentos--;
        }
      
        if(intentos == 0) {
          activarBloqueo = true;
          lcd.clear();
          lcd.print("SistemaBloqueado"); 
          lcd.setCursor(7, 1);
           lcd.write(byte(1));
          digitalWrite(LED_GREEN, LOW);
          digitalWrite(LED_RED, LOW);
          digitalWrite(LED_BLUE, HIGH);
          lcd.noCursor();
          sonidoBloqueado();
        }
      digitalWrite(LED_RED, LOW);
      //digitalWrite(LED_BLUE, LOW);
      digitalWrite(LED_GREEN, LOW);
      }
}

/*F**************************************************************************
* NAME: medirTemperaturaHumedadLuz
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Initiates the measurement of temperature, humidity, and light levels and performs actions based on the measured values
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void medirTemperaturaHumedadLuz(){
    asyncTask1.Start();
    asyncTask2.Start();
  if(tempValue > temperaturaAlta){
      currentInput = Input::senialCuatro;
      updateInputStateMachine();
  }
  else{
    currentInput = Input::Unknown;
  }
  asyncTaskTimeOut10Seg.Start();
}

/*F**************************************************************************
* NAME: activarAlarmaAmbiental
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Activates the environmental alarm based on temperature readings and performs actions accordingly
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void activarAlarmaAmbiental(){
  sonidoBloqueado();
  tiempo = 0;
  tiempo = millis();
    if(tempValue < temperaturaNormal){
    currentInput = Input::senialDos;
    updateInputStateMachine();
  } else if ( (tempValue > temperaturaAlta) &&  (tiempo > 5000)){
    tiempo = 0;
    currentInput = Input::senialTres;
    updateInputStateMachine();
  }
}

/*F**************************************************************************
* NAME: ventasPuertas
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Handles events related to window and door operations
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/

void ventasPuertas(){
  Serial.println("Eventos ventanas ");
  lcd.print("VentanaPuerta");
  asyncTaskTimeOut2Seg.Start();
}
/*F**************************************************************************
* NAME: updateInputStateMachine
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* update State Machine inputs from temperature sensor
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void updateInputStateMachine()
{
  int currentState = stateMachine.GetState();
  switch (currentState)
  {
    case senialUno:   ventasPuertas();  
                    break;
    case senialDos:  medirTemperaturaHumedadLuz(); 
                    break;
    case senialTres:  mensajeAlerta(); 
                    break;
    case senialCuatro:  activarAlarmaAmbiental();
                    break;
    default:        Serial.println("state Unknown"); 
                    break;
  }
}

/*F**************************************************************************
* NAME: mensajeBienvenida
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Displays a welcome message and initializes the system
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void mensajeBienvenida() {
  currentInput = Input::Unknown;
  actualizarCursor();
 // DEBUG("Bienvenido");
  lcd.setCursor(1, 0);
  lcd.print("Bienvenido");
  lcd.setCursor(11, 0);
  lcd.write(byte(0));
  Serial.println("Bienvenido");
  lcd.setCursor(2, 0);
  String message = "sistema Pro";
  lcd.clear();
  for (byte i = 0; i < message.length(); i++) {
    lcd.print(message[i]);
    delay(50);
  }
  delay(500);
  sistemaClave();
}


/*F**************************************************************************
* NAME: salidaSeguridad
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Handles the transition from the security system to events
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void salidaSeguridad()
{
  DEBUG("Dejando sistema de seguridad");
  Serial.println("Pasando de seguridad a eventos");
}

/*F**************************************************************************
* NAME: mensajeEvento
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Displays a message related to an event
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void mensajeEvento()
{
  currentInput = Input::Unknown;
  DEBUG("State_NORMAL_B");
  //lcd.clear();
  //lcd.setCursor(0, 0);
  lcd.print("VentanaPuerta");
  Serial.println("VentanaPuerta");
 
}

/*F**************************************************************************
* NAME: salidaEvento
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Handles the transition from events to the environmental monitoring system
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void salidaEvento()
{
   DEBUG("Dejando eventos");
  Serial.println("Pasando de eventos a monitor ambiental");
}
/*F**************************************************************************
* NAME: mensajeMonitor
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Displays a message related to the environmental monitoring system
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void mensajeMonitor()
{
  currentInput = Input::Unknown;
  DEBUG("State_NORMAL_B");
  lcd.clear();
  lcd.print("MonitorAmb");
  Serial.println("MonitorAmb");
  lcd.setCursor(3, 1);
  lcd.setCursor(0, 0);
}
/*F**************************************************************************
* NAME: salidaMonitor
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Handles the transition from the environmental monitoring system to the environmental alarm system
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void salidaMonitor()
{
   DEBUG("Dejando monitor ambiental");
  Serial.println("Pasando de monitor ambiental a alarma ambiental");
}


/*F**************************************************************************
* NAME: mensajeAlarma
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Displays a message related to the environmental alarm system
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void mensajeAlarma()
{
  currentInput = Input::Unknown;
  DEBUG("State_NORMAL_B");
  lcd.clear();
  lcd.print("Alarma ambi");
  Serial.println("Alarma ambiental");
  lcd.setCursor(3, 1);
  lcd.setCursor(0, 0);
}

/*F**************************************************************************
* NAME: salidaAlarma
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Handles the transition from the environmental alarm system to the security alert system
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void salidaAlarma()
{
  //DEBUG("Leaving State_HIGH_C");
  DEBUG("Dejando alarma ambiental");
  Serial.println("Pasando de alarma ambiental a alerta seguridad");
}

/*F**************************************************************************
* NAME: mensajeAlerta
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Displays a message related to the security alert system and starts a timeout task
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void mensajeAlerta()
{
  currentInput = Input::Unknown;
  DEBUG("State_NORMAL_B");
  lcd.clear();
  lcd.print("Alerta Seguridad");
  Serial.println("Alerta Seguridad");
  lcd.setCursor(3, 1);
  lcd.setCursor(0, 0);
  asyncTaskTimeOut6Seg.Start();
}

/*F**************************************************************************
* NAME: salidaAlerta(
*----------------------------------------------------------------------------
* PARAMS:   none
* return:   none
*----------------------------------------------------------------------------
* PURPOSE:
* Handles the transition from the security alert system to the events system
*----------------------------------------------------------------------------
* NOTE:
* 
*****************************************************************************/
void salidaAlerta()
{
  DEBUG("Dejando alerta seguridad");
  Serial.println("Pasando de alerta seguridad a eventos");
}
