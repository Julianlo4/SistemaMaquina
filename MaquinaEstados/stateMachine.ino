
/**
  * @brief Setup the State Machine
  * @param none
  * @return none
*/
void setupStateMachine()
{
  // Add transitions
  stateMachine.AddTransition(ingresoSeguridad, eventoPuertaVentana, []() { return currentInput == senialUno; });
  
  stateMachine.AddTransition(eventoPuertaVentana, monitorAmbiental, []() { return currentInput == senialDos; });
  stateMachine.AddTransition(eventoPuertaVentana, alertaSeguridad, []() { return currentInput == senialCuatro; });

  stateMachine.AddTransition(monitorAmbiental, eventoPuertaVentana, []() { return currentInput == senialUno; });
  stateMachine.AddTransition(monitorAmbiental,alarmaAmbiental, []() { return currentInput ==  senialTres; });

  stateMachine.AddTransition(alarmaAmbiental, monitorAmbiental, []() { return currentInput == senialDos; });
  stateMachine.AddTransition(alarmaAmbiental, alertaSeguridad, []() { return currentInput == senialCuatro; });
  
  stateMachine.AddTransition(alertaSeguridad, eventoPuertaVentana, []() { return currentInput == senialUno; });

  // Add actions
  stateMachine.SetOnEntering(ingresoSeguridad, mensajeBienvenida);
  stateMachine.SetOnEntering(eventoPuertaVentana, mensajeEvento);
  stateMachine.SetOnEntering(monitorAmbiental, medirTemperaturaHumedadLuz);
  stateMachine.SetOnEntering(alarmaAmbiental, mensajeAlarma);
  stateMachine.SetOnEntering(alertaSeguridad, mensajeAlerta);

 stateMachine.SetOnLeaving(ingresoSeguridad, salidaSeguridad);
  stateMachine.SetOnLeaving(eventoPuertaVentana, salidaEvento);
  stateMachine.SetOnLeaving(monitorAmbiental, salidaMonitor);
  stateMachine.SetOnLeaving(alarmaAmbiental, salidaAlarma);
  stateMachine.SetOnLeaving(alertaSeguridad, salidaAlerta);
}
/**
  * @brief Read data from serial port
  * @param none
  * @return none
*/
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
    //updateInputStateMachine();
  }

}
/**
  * @brief Implements a password-based system to control access and performs actions based on the entered password
  * @param none
  * @return none
*/
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
            updateInputStateMachine(currentInput);
         }  else {
              lcd.clear();
              digitalWrite(LED_GREEN, LOW);
              digitalWrite(LED_BLUE, LOW);
              digitalWrite(LED_RED, HIGH);
              sonidoErrado();
              lcd.print("Clave Incorrecta"); 
              lcd.setCursor(7, 1);
              lcd.write(byte(3));
              delay(3000);
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
      i = 0;
      }
}
/**
  * @brief Initiates the measurement of temperature, humidity, and light levels and performs actions based on the measured values
  * @param none
  * @return none
*/
void medirTemperaturaHumedadLuz(){ 
    lcd.clear();
    mensajeMonitor();
    asyncTask1.Start();
    asyncTask2.Start();
    asyncTaskTimeOut10Seg.Start();
}
/**
  * @brief Activates the environmental alarm based on temperature readings and performs actions accordingly
  * @param none
  * @return none
*/
void activarAlarmaAmbiental(){
  asyncTaskTimeOut2Seg.Stop();
  asyncTaskTimeOut10Seg.Stop();
  mensajeAlarma();
  lcd.clear();
  lcd.print("Alarma");
  Serial.println("alarma");
  sonidoBloqueado();
  asyncTaskContarSegundos.Start();
  digitalWrite(LED_RED, LOW);
}
/**
  * @brief Handles events related to window and door operations
  * @param none
  * @return none
*/
void ventasPuertas(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("VentanaPuerta");
  asyncTaskTimeOut2Seg.Start();
  Serial.println("Eventos ventanas ");
  asyncTaskSensores.Start();
   digitalWrite(LED_RED, LOW);
   digitalWrite(LED_BLUE, LOW);
   digitalWrite(LED_GREEN, LOW);
}
/**
  * @brief update State Machine inputs from temperature sensor
  * @param none
  * @return none
*/
void updateInputStateMachine(int current)
{
  switch (current)
  {
    case ingresoSeguridad: asyncTaskSeguridad.Start(); // sistemaClave();  break;
    case eventoPuertaVentana:  ventasPuertas();    break;
    case monitorAmbiental:  medirTemperaturaHumedadLuz(); break;
    case alarmaAmbiental: activarAlarmaAmbiental(); break;
    case alertaSeguridad:   break;
    default: Serial.println("state Unknown"); break;
  }
}
/**
  * @brief Displays a welcome message and initializes the system
  * @param none
  * @return none
*/
void mensajeBienvenida() {
  Serial.println("Bienvenido");
  Serial.println("1   2   3   4   5");
  Serial.println("X           ");
  Serial.println();
  actualizarCursor();
  lcd.setCursor(1, 0);
  lcd.print("Bienvenido");
  lcd.setCursor(10, 0);
  lcd.write(byte(0));
  lcd.setCursor(2, 0);
  String message = "sistema Pro";
  lcd.clear();
  for (byte i = 0; i < message.length(); i++) {
    lcd.print(message[i]);
    delay(50);
  }
  sistemaClave();
}

/**
  * @brief Handles the transition from the security system to events
  * @param none
  * @return none
*/
void salidaSeguridad()
{
  DEBUG("Dejando sistema de seguridad");

}
/**
  * @brief Displays a message related to an event
  * @param none
  * @return none
*/
void mensajeEvento()
{
  lcd.clear();
  Serial.println("VentanaPuerta");
  Serial.println("1   2   3   4   5");
  Serial.println("    X        ");
  Serial.println();
}
/**
  * @brief Handles the transition from events to the environmental monitoring system
  * @param none
  * @return none
*/
void salidaEvento()
{
  DEBUG("Dejando eventos");
}
/**
  * @brief Displays a message related to the environmental monitoring system
  * @param none
  * @return none
*/
void mensajeMonitor()
{
  lcd.clear();
  Serial.println("MonitorAmbiental");
  Serial.println("1   2   3   4   5");
  Serial.println("        X    ");
  Serial.println();
}
/**
  * @brief Handles the transition from the environmental monitoring system to the environmental alarm system
  * @return none
*/
void salidaMonitor()
{
   DEBUG("Dejando monitor ambiental");
}
/**
  * @brief Displays a message related to the environmental alarm system
  * @param none
  * @return none
*/
void mensajeAlarma()
{
  lcd.clear();
  asyncTask1.Stop();
  asyncTask2.Stop();
  Serial.println("Alarma ambiental");
  Serial.println("1   2   3   4   5");
  Serial.println("            X");
  Serial.println();
  lcd.clear();
  lcd.print("Alarma ambi");
  lcd.setCursor(3, 1);
  lcd.setCursor(0, 0);
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_BLUE, HIGH);
  digitalWrite(LED_GREEN, LOW);
}
/**
  * @brief Handles the transition from the environmental alarm system to the security alert system
  * @param none
  * @return none
*/
void salidaAlarma()
{
  DEBUG("Dejando alarma ambiental");
}
/**
  * @brief Displays a message related to the security alert system and starts a timeout task
  * @param none
  * @return none
*/
void mensajeAlerta()
{
  lcd.clear();
  Serial.println("Alerta Seguridad");
  Serial.println("1   2   3   4   5");
  Serial.println("                X");
  Serial.println();
  sistemaAlerta();
}
/**
  * @brief Handles the transition from the security alert system to the events system
  * @param none
  * @return none
*/
void salidaAlerta()
{
  DEBUG("Dejando alerta seguridad");
}
/**
  * @brief Activate the alert system by stopping and starting the necessary tasks and changing LED states.
  * @param none
  * @return none
*/
void sistemaAlerta(){
  asyncTaskTimeOut6Seg.Start();
  asyncTaskTimeOut2Seg.Stop();
  asyncTaskTimeOut10Seg.Stop();
  asyncTaskContarSegundos.Stop();
  asyncTask1.Stop();
  asyncTask2.Stop();
  asyncTaskSensores.Stop();
  lcd.print("alerta");
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_BLUE, HIGH);
  digitalWrite(LED_GREEN, LOW);
}
