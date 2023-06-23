#define LED_GREEN A7
#define LED_RED A5
#define LED_BLUE A6
#define BUZZER A14
#define DHT11_PIN A15


const byte KEYPAD_ROWS = 4;
const byte KEYPAD_COLS = 4;
byte rowPins[KEYPAD_ROWS] = {5, 4, 3, 2};
byte colPins[KEYPAD_COLS] = {A3, A2, A1, A4};

bool activarBloqueo = false;
char password[6];
char passwordCorrecta[] = "12345";
unsigned char i = 0;
byte intentos = 3;

const int photocellPin = A10;
const int ledPin = 13;
const byte temperaturaNormal = 30;
const byte temperaturaAlta = 32;
unsigned long tiempoActual = 0;
 int segundos = 0;
