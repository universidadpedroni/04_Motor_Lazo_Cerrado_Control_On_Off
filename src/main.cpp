#include <Arduino.h>
#include <blink.h>
#include <motor.h>
#include <encoder.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // RX, TX

const int PIN_ENCODER_A=2;
const int PIN_ENCODER_B=3;
const int PIN_ENABLE=5;
const int PIN_IN1=6;
const int PIN_IN2=7;

struct Signals {
  float r;    // Referencia
  float y;    // Salida del sistema (Posición)
  float v;    // Salida del sistema (Velocidad)
  float e;    // Error
  float u;    // Acción de control
};

Signals signals={0,0,0,0,0};



//static float ppr=200;

blink parpadeo(LED_BUILTIN);
motor motordc(PIN_ENABLE,PIN_IN1,PIN_IN2);
Encoder miEncoder(PIN_ENCODER_A,PIN_ENCODER_B);

void Telemetria(unsigned long interval);


void setup() {
  delay(5000);
  //Serial.begin(115200);
  mySerial.begin(115200);
  parpadeo.init();
  motordc.init();
  mySerial.flush();
  

  
}

void loop() {
  signals.r = 1000.0;    // Señal de referencia
  signals.y = (float)miEncoder.read(); // Señal de salida del sistema.

  // CONTROL BANG BANG
  signals.e = signals.r - signals.y; // señal de error
  float egap = 50;
  if (abs(signals.e)>egap) signals.e >= egap? signals.u=9 : signals.u=-9;
  
  
  int potencia = constrain((int) 255 / 9 * signals.u,-255,255);
  motordc.run(potencia);
  
  Telemetria(10);

  parpadeo.update(500);
    
}

void Telemetria(unsigned long interval)
{
  static bool primeraVez=true;

	static unsigned long previousMillis = 0;        // will store last time LED was updated
	unsigned long currentMillis = millis();
	static float yk_1=10;
  
	if (primeraVez)
  {
    mySerial.print("y e u");
    primeraVez=false;
  }

	if(currentMillis - previousMillis > interval) 
	{
		previousMillis = currentMillis;
    // cálculo de la velocidad.
    signals.v=(signals.y-yk_1)/(float)interval;
    yk_1=signals.y;

		mySerial.print(signals.y);
    mySerial.print("\t");
    mySerial.print(signals.e);
    mySerial.print("\t");
    //mySerial.print(signals.v);
    //mySerial.print("\t");
    mySerial.print(100*signals.u);
    mySerial.print("\t");
    mySerial.println();
	}
    
}