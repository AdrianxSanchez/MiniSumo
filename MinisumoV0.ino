

#include <Wire.h>

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
const int umbralDistancia = 50;  // 20 cm en milímetros

#define in1 19
#define in2 18
#define in3 17
#define in4 16
#define stby 15


int motorSpeedA = 255;
int motorSpeedB = 255;
int motorSpeedAA = 50;
int motorSpeedBB = 50;

int objetivoDistancia = 100;
int objetivoPosicion = 220;

bool girado = false; 


void setup() {
  Serial.begin(115200);
  pinMode(stby, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  digitalWrite(stby, HIGH);
  delay(500);

  if (!lox.begin()) {
    Serial.println(F("Error al iniciar el VL53L0X"));
    while (1);
  }
}

void loop() {
 
 // Girar a la derecha solo si aún no ha girado
  if (!girado) {
    motorDerecha();
    delay(5000);  // Esperar un segundo
    girado = true;  // Marcar como girado
  }

  // Continuar con la lógica original
  manejarSensorVL53L0X();

}

void manejarSensorVL53L0X() {

  
   VL53L0X_RangingMeasurementData_t measure;

  lox.rangingTest(&measure, false);
  if (measure.RangeStatus != 4) {  // Si la medición es válida
    Serial.print(F("Distancia: "));
    Serial.print(measure.RangeMilliMeter);
    Serial.println(F(" mm"));

    // Control proporcional
    int error = umbralDistancia - measure.RangeMilliMeter;
    int correccion = 2 * error;  // Ajusta este valor según la sensibilidad deseada

    if (measure.RangeMilliMeter <= umbralDistancia) {
      // Acelerar al máximo
      motorDelante(motorSpeedA);
    } else {
      // Ajustar velocidad según la distancia
      int velocidadDeseada = map(measure.RangeMilliMeter, umbralDistancia, 400, motorSpeedA, 0);
      motorDelante(velocidadDeseada);
    }
  } else {
    // Si no se detecta ningún objeto, apagar los motores
    apagarmotores();
  }
}
void motorDelante(int velocidad) {
  analogWrite(in1, velocidad);
  analogWrite(in3, velocidad);
}


void motorAtras() {
  analogWrite(in2, motorSpeedA);
  analogWrite(in4, motorSpeedB);
  delay(5000);
}

void motorDerecha() {
  analogWrite(in2, motorSpeedAA);
  analogWrite(in3, motorSpeedBB);
  delay(100);
}

void motorIzquierda() {
  analogWrite(in1, motorSpeedAA);
  analogWrite(in4, motorSpeedBB);
  delay(5000);
}

void apagarmotores() {
  analogWrite(in1, LOW);
  analogWrite(in4, LOW);
  analogWrite(in2, LOW);
  analogWrite(in3, LOW);
}


