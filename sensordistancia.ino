//Welcome to tres sensores de distancia team fulvineitor// (Este equipo es mi equipo principal),chicas las quiero mucho,No me saquen del grupo po favo //noh vamoh pa cartagena!! ,solarte desgraciao,jhonatan devuelvenos las camisetas!!,Esteban no lleves tu moto a cartagena plisss :_(

// Definir los pines de los sensores
const int sensorPin1 = A4;   // Pin analógico para el primer sensor
//const int sensorPin2 = A1;   // Pin analógico para el segundo sensor
//const int sensorPin3 = A2;   // Pin analógico para el tercer sensor

void setup() {
  Serial.begin(9600);  // Inicializar la comunicación serial
}

void loop() {
  // Leer las distancias de los sensores
  float distancia1 = leerDistancia(sensorPin1);
  //float distancia2 = leerDistancia(sensorPin2);
  //float distancia3 = leerDistancia(sensorPin3);

  // Mostrar las distancias en el Monitor Serie
  Serial.print("Distancia 1: ");
  Serial.print(distancia1);
  Serial.println(" cm");

  /*Serial.print("Distancia 2: ");
  Serial.print(distancia2);
  Serial.println(" cm");

  Serial.print("Distancia 3: ");
  Serial.print(distancia3);
  Serial.println(" cm");*/

  delay(100);  // Esperar 1 segundo antes de la siguiente lectura
}

// Función para leer la distancia desde un sensor
float leerDistancia(int pin) {
  // Leer el valor analógico del sensor
  int lectura = analogRead(pin);

  // Calcular la distancia en centímetros utilizando una fórmula empírica
  float distancia = 4800 / (lectura - 20);

  return distancia;
}
