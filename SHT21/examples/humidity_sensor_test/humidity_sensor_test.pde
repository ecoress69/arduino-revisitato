

/****************************
 * GetRelativeHumidity
 *  An example sketch that reads the sensor and prints the
 *  relative humidity to the PC's serial port
 *
 *  Tested with the SHT21-Breakout
 *  Humidity sensor from Modern Device.
 *****************************/
#include <Wire.h>
#include <LibHumidity.h>

LibHumidity humidity = LibHumidity();

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
}


void loop() {
  digitalWrite(13, LOW);

  Serial.println("Use standard config **************");
  read();
 blink(5); 

  digitalWrite(3, HIGH);
      
  Serial.print("Use 12/14 config ************** Value:");
  Serial.println(humidity.setResolution(RES_12_14), BIN);
  read();
 blink(5); 


  Serial.print("Use 10/13 config ************** Value");
  Serial.println(humidity.setResolution(RES_10_13), BIN);
  read();
 blink(5); 
  digitalWrite(3, LOW);

  Serial.print("Use  8/12 config ************** Value:");
  Serial.println(humidity.setResolution(RES_08_12), BIN);
  read();
 blink(5); 
  digitalWrite(10,LOW);

  Serial.print("Set 11/11 config ************** Value: ");
  Serial.println(humidity.setResolution(RES_11_11), BIN);
  read();
 blink(5); 
  digitalWrite(3, HIGH);
  
  Serial.println("Resetting sensor");
  humidity.reset();
 blink(5); 
}

void blink(int num) {
  for(int i=0; i < num; i++) {
    digitalWrite(13, LOW);
    delay(100);
    digitalWrite(13, HIGH);
    delay(100);
  }
  digitalWrite(13, LOW);
}  

void read() {
  Serial.print(" RHumidity: ");
  Serial.print(humidity.getHumidity());
  Serial.print(" Temp in C: ");
  Serial.print(humidity.getTemperature(true));
  Serial.print(" Temp in F: ");
  Serial.println(humidity.getTemperature(false));
  Serial.print(" URegister: ");
  Serial.println(humidity.getUserRegister(), BIN);
}
