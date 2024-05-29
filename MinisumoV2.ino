#include <Wire.h>
#include <QTRSensors.h>
#include <StateMachineLib.h>

// Pines motores
#define in1 19
#define in2 18
#define in3 17
#define in4 16
#define stby 15

// Pines sensores QTR
const uint8_t SensorCount = 6;
uint16_t sensorValues[SensorCount];
const uint8_t sensorPins[SensorCount] = {34, 35, 32};
const uint8_t emitterPin = 27;

// Pines sensores Sharp
#define sensorPin1 25 // frontal izquierdo
#define sensorPin2 26 // frontal derecho
#define sensorPin3 14 // lateral izquierdo
#define sensorPin4 27 // lateral derecho

QTRSensors qtr;

int motorSpeedA = 255;
int motorSpeedB = 255;
int motorSpeedAA = 50;
int motorSpeedBB = 50;
const int umbralDistancia = 20; // Umbral de distancia en centímetros
const int QTR_THRESHOLD = 900;  // Umbral para los sensores QTR

enum State {
  Inicio = 0,
  Busqueda = 1,
  Desviacion = 2,
  Empuja = 3
};

enum Input {
  Delanteros = 0,
  Laterales = 1,
  Linea = 2,
  Nada = 3,
  Unknown = 4
};

StateMachine stateMachine(4, 5);
Input currentInput;

void setup() {
  Serial.begin(115200);

  pinMode(stby, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  digitalWrite(stby, HIGH);
  delay(500);

  qtr.setTypeAnalog();
  qtr.setSensorPins(sensorPins, SensorCount);
  qtr.setEmitterPin(emitterPin);

  delay(200);
  pinMode(emitterPin, OUTPUT);
  digitalWrite(emitterPin, HIGH); // Encender el LED del sensor para indicar que estamos en modo de calibración

  for (uint16_t i = 0; i < 400; i++) {
    qtr.calibrate();
  }
  digitalWrite(emitterPin, LOW); // Apagar el LED del sensor para indicar que hemos terminado la calibración

  Serial.println("Starting State Machine...");
  setupStateMachine();
  Serial.println("State Machine Started");
  stateMachine.SetState(Inicio, false, true);
}

void loop() {
  currentInput = static_cast<Input>(readInput());
  stateMachine.Update();
}

void setupStateMachine() {
  stateMachine.AddTransition(Inicio, Busqueda, []() { return true; }); // Siempre comienza en estado de búsqueda
  stateMachine.AddTransition(Busqueda, Desviacion, []() { return currentInput == Linea; });
  stateMachine.AddTransition(Busqueda, Empuja, []() { return currentInput == Delanteros; });
  stateMachine.AddTransition(Desviacion, Busqueda, []() { return currentInput == Nada; });
  stateMachine.AddTransition(Empuja, Busqueda, []() { return currentInput == Nada; });

  stateMachine.SetOnEntering(Inicio, funct_inicio);
  stateMachine.SetOnEntering(Busqueda, funct_Busqueda);
  stateMachine.SetOnEntering(Desviacion, funct_Desviacion);
  stateMachine.SetOnEntering(Empuja, funct_Empuja);
}

void funct_inicio(void) {
  // No se necesita ninguna acción especial al entrar en el estado de inicio
}

void funct_Busqueda(void) {
  if (leerDistancia(sensorPin1) < umbralDistancia || leerDistancia(sensorPin2) < umbralDistancia) {
    currentInput = Input::Delanteros;
  } else if (sensorValues[0] > QTR_THRESHOLD || sensorValues[5] > QTR_THRESHOLD) {
    currentInput = Input::Linea;
  } else {
    currentInput = Input::Nada;
  }
}

void funct_Desviacion(void) {
  motorDerecha();
  delay(500); // Girar durante medio segundo
  apagarMotores();
}

void funct_Empuja(void) {
  motorDelante(motorSpeedA);
  delay(1000); // Avanzar durante un segundo
}

float leerDistancia(int pin) {
  int lectura = analogRead(pin);
  float distancia = 4800 / (lectura - 20);
  return distancia;
}

void motorDelante(int velocidad) {
  analogWrite(in1, velocidad);
  analogWrite(in3, velocidad);
}

void motorDerecha() {
  analogWrite(in2, motorSpeedAA);
  analogWrite(in3, motorSpeedBB);
}

void apagarMotores() {
  analogWrite(in1, LOW);
  analogWrite(in2, LOW);
  analogWrite(in3, LOW);
  analogWrite(in4, LOW);
}

int readInput() {
  // Leer entrada según el estado actual
  int currentState = stateMachine.GetState();
  switch (currentState) {
    case Inicio:
      // No hay entrada en el estado de inicio
      return Input::Nada;
    case Busqueda:
      // Leer sensores y determinar entrada
      funct_Busqueda();
      break;
    case Desviacion:
      // No hay entrada en el estado de desviación
      return Input::Nada;
    case Empuja:
      // No hay entrada en el estado de empuje
      return Input::Nada;
    default:
      Serial.println("Estado desconocido");
      return Input::Unknown;
  }
  return currentInput;
}

