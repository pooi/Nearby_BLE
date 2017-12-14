#include <SoftwareSerial.h>

#include <Wire.h>
#include <Adafruit_MLX90614.h>

/*============================= Health Temperature =============================*/
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
/*==============================================================================*/

/*================================= Heart Rate =================================*/
//  Variables
int pulsePin = 0;                 // Pulse Sensor purple wire connected to analog pin 0
int blinkPin = 13;                // pin to blink led at each beat
int fadePin = 5;                  // pin to do fancy classy fading blink at each beat
int fadeRate = 0;                 // used to fade LED on with PWM on fadePin

// Volatile Variables, used in the interrupt service routine!
volatile int BPM;                   // int that holds raw Analog in 0. updated every 2mS
volatile int Signal;                // holds the incoming raw data
volatile int IBI = 600;             // int that holds the time interval between beats! Must be seeded! 
volatile boolean Pulse = false;     // "True" when User's live heartbeat is detected. "False" when not a "live beat". 
volatile boolean QS = false;        // becomes true when Arduoino finds a beat.

// Regards Serial OutPut  -- Set This Up to your needs
static boolean serialVisual = true;   // Set to 'false' by Default.  Re-set to 'true' to see Arduino Serial Monitor ASCII Visual Pulse 
/*==============================================================================*/

SoftwareSerial BTSerial(2, 3);

void nearby_init(){

  mlx = Adafruit_MLX90614();
  
  pulsePin = 0;
  blinkPin = 13;
  fadePin = 5;
  fadeRate = 0;
  BPM = 0;
  Signal = 0;
  IBI = 600;
  Pulse = false;
  QS = false;
  serialVisual = true;
  
}

void setup() {
  nearby_init();
  pinMode(blinkPin,OUTPUT);         // pin that will blink to your heartbeat!
  pinMode(fadePin,OUTPUT);          // pin that will fade to your heartbeat!
  
  Serial.begin(9600);
  BTSerial.begin(9600);
  Serial.print("Init");
  Serial.println();

  interruptSetup();
  
  mlx.begin();
}

void loop() {
  //if (BTSerial.available()) {
  //  Serial.write(BTSerial.read());
  //}
  // Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempC()); 
  // Serial.print("*C\tObject = "); Serial.print(mlx.readObjectTempC()); Serial.println("*C");
  //Serial.print("Temperature = "); Serial.print(mlx.readObjectTempC()); Serial.println("*C");
  BTSerial.print("T"); BTSerial.println(mlx.readObjectTempC());// BTSerial.println("*C");
  // Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempF()); 
  // Serial.print("*F\tObject = "); Serial.print(mlx.readObjectTempF()); Serial.println("*F");
  //Serial.print("BPM = "); Serial.print(BPM);

  //Serial.println();

  serialOutput() ;
  if (QS == true){     // A Heartbeat Was Found
                       // BPM and IBI have been Determined
                       // Quantified Self "QS" true when arduino finds a heartbeat
        fadeRate = 255;         // Makes the LED Fade Effect Happen
                                // Set 'fadeRate' Variable to 255 to fade LED with pulse
        serialOutputWhenBeatHappens();   // A Beat Happened, Output that to serial.     
        QS = false;                      // reset the Quantified Self flag for next time    
  }
     
  ledFadeToBeat();                      // Makes the LED Fade Effect Happen 
  
  delay(500);//500
}

void ledFadeToBeat(){
  fadeRate -= 15;                         //  set LED fade value
  fadeRate = constrain(fadeRate,0,255);   //  keep LED fade value from going into negative numbers!
  analogWrite(fadePin,fadeRate);          //  fade LED
}
