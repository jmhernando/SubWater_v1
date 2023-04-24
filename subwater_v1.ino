/**
____________________
MULTIPLEXOR
Vin -> 5V
GND -> GND
SDA -> A4
SCL -> A5
SC0 -> D2
SC1 -> D3
SC2 -> D4
SC3 -> D5
____________________
ACELERÓMETRO
VCC -> 5V
GND -> GND
SDA -> SDA (Multiplexor)
SCL -> SCL (Multiplexor)

____________________
OLED
VCC -> 5V
GND -> GND
SDA -> SDA (Multiplexor)
SCL -> SCL (Multiplexor)
*/

#include <Servo.h>    //Esta librería se utiliza para crear objetos Servo y manejar con ellos servomotores.
#include "I2Cdev.h"   //Esta librería permite la comunicación con el sensor MPU6050. Que es un sensor de 6 ejes (Acelerómetro y giroscopio)
#include "MPU6050.h"  //Sirve para lo mismo que la anterior
#include <Wire.h>     //Sirve para comunicarse con dispositivos que usan el protocolo de comunicación 12C (con reloj)
#include <Adafruit_SSD1306.h> //Para el control de pantallas OLED.

#define PCA9548A_ADDRESS 0x70
#define CHANNEL_0 0
#define CHANNEL_1 1

#define OLED_WIDTH 128
#define OLED_HEIGHT 32
#define OLED_RESET -1
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);
 
// Pantalla OLED con dirección TWI
#define OLED_ADDR 0x3C
const int mpuAddress = 0x68;  // Puede ser 0x68 o 0x69
MPU6050 mpu(mpuAddress);

int ax, ay, az;
int gx, gy, gz;
int i=0;

Servo servoMotor;

/**
__________________________
SISTEMAS DE SEGURIDAD
--------------------------
__________________________
Sensor de humedad
*/
int sensorPin = A0;
unsigned long lastMillis = 0;

void setup() {
   // Iniciamos el servo para que empiece a trabajar con el pin 9
  servoMotor.attach(9);
  // inicializa y borra la pantalla
  display.begin (SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay ();
  display.display ();
 
/**
__________________________
CONFIGURACIÓN DE LA PANTALLA
--------------------------
En esta sección se configurarán las 
siguiente características: 

-> Tamaño del texto.
-> Color del texto.
-> Seleccionar dónde empieza el texto.
->
__________________________
Relacionado con el texto
*/
  display.setTextSize(1); 
  display.setTextColor (WHITE);
  display.setCursor (1,0);
 
// actualiza la pantalla con todos los gráficos anteriores
  display.display();
  Serial.begin(9600);
  Wire.begin();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Inicia la pantalla OLED
  display.clearDisplay(); // Limpia la pantalla

  Wire.beginTransmission(PCA9548A_ADDRESS);
  Wire.write(1 << CHANNEL_0); // Selecciona el canal 0 del multiplexor
  Wire.endTransmission();

  mpu.initialize();
  Serial.println(mpu.testConnection() ? F("IMU iniciado correctamente") : F("Error al iniciar IMU"));
}

void loop() {
  // Selecciona el canal 0 del multiplexor
  Wire.beginTransmission(PCA9548A_ADDRESS);
  Wire.write(1 << CHANNEL_0);
  Wire.endTransmission();

  // Leer las aceleraciones
  mpu.getAcceleration(&ax, &ay, &az);

  //Calcular los angulos de inclinacion
  float accel_ang_x = atan(ax / sqrt(pow(ay, 2) + pow(az, 2))) * (180.0 / 3.14);
  float accel_ang_y = atan(ay / sqrt(pow(ax, 2) + pow(az, 2))) * (180.0 / 3.14);

  // Selecciona el canal 1 del multiplexor
  Wire.beginTransmission(PCA9548A_ADDRESS);
  Wire.write(1 << CHANNEL_1);
  Wire.endTransmission();

  // Mostrar resultados
  display.clearDisplay(); // Limpia la pantalla antes de escribir nuevos datos
  display.setCursor(0, 0); // Posiciona el cursor en la esquina superior izquierda
  display.println(F("Inclinacion en X: "));
  display.print(accel_ang_x);
  display.setCursor(0, 16); // Posiciona el cursor en la segunda línea de la pantalla
  display.println(F("Inclinacion en Y: "));
  display.println(accel_ang_y);
  display.display(); // Muestra los datos en la pantalla OLED
 

  
  if (millis() - lastMillis >= 1000) {
    moverServo1();
  }

  if (millis() - lastMillis >= 1000) {
    int humedad = analogRead(sensorPin);
    if(humedad<500) exit(0);
    else{
      Serial.println(humedad);
      lastMillis = millis(); // Reiniciamos el tiempo de referencia
    }
  }
}

void moverServo1(bool tiempo){
  switch(i){
    case 0: 
      // Desplazamos a la posición 0º
      servoMotor.write(0);
      Serial.println("0");
      i++;
      break;
     
    case 1:
      // Desplazamos a la posición 90º
      servoMotor.write(90);
      Serial.println("90");
      i++;
      break;

    case 2:  
    // Desplazamos a la posición 180º
    servoMotor.write(180);
    Serial.println("180");
    i=0;
    break;
  }
}
