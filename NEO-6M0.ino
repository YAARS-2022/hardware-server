
#include <SoftwareSerial.h>
#include <TinyGPS.h>

#include <SPI.h>
#include <LoRa.h>

String myVal;

// RX = 11  TX = 10
SoftwareSerial mySerial(6, 7);
TinyGPS gps;

void gpsdump(TinyGPS &gps);
void printFloat(double f, int digits = 2);

void setup()  
{
  // Oploen serial communications and wait for port to open:
  Serial.begin(9600);
  // set the data rate for the SoftwareSerial port
  
  delay(1000);
}

void loop() // run over and over
{
  myVal = "";
  mySerial.begin(9600);
  bool newdata = false;
  unsigned long start = millis();
  // Every 5 seconds we print an update
  while (millis() - start < 5000) 
  {
    if (mySerial.available()) 
    
    {
      char c = mySerial.read();
     
      //myVal += c;
     Serial.print(c);  // uncomment to see raw GPS data
      if (gps.encode(c)) 
      {
        newdata = true;
        break;  // uncomment to print new data immediately!
      }
    }
  }
  
  if (newdata) 
  {
//    Serial.println("Acquired Data");
//    Serial.println("-------------");
    gpsdump(gps);
//    Serial.println("-------------");
//    Serial.println();
  }
//  Serial.println();
  mySerial.end();

  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setSyncWord(0xF3);
  LoRa.setTxPower(20);

  LoRa.beginPacket();
  LoRa.print(myVal);
  LoRa.print("\n");
  LoRa.endPacket();

  LoRa.end();
  
  
  Serial.println(myVal);

  
}

void gpsdump(TinyGPS &gps)
{
  long lat, lon;
  float flat, flon;
  unsigned long age, date, time, chars;
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned short sentences, failed;

  gps.get_position(&lat, &lon, &age);
//  Serial.print("Lat/Long(10^-5 deg): "); Serial.print(lat); Serial.print(", "); Serial.print(lon); 
//  Serial.print(" Fix age: "); Serial.print(age); Serial.println("ms.");
//  
  // On Arduino, GPS characters may be lost during lengthy Serial.print()
  // On Teensy, Serial prints to USB, which has large output buffering and
  //   runs very fast, so it's not necessary to worry about missing 4800
  //   baud GPS characters.

  gps.f_get_position(&flat, &flon, &age);
  printFloat(flat, 5); myVal += ", "; Serial.print(", "); printFloat(flon, 5);//--------------------------------------->
 

  gps.get_datetime(&date, &time, &age);

  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
}

void printFloat(double number, int digits)
{
  // Handle negative numbers
  if (number < 0.0) 
  {
     myVal += "-";
     Serial.print('-');
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;
  
  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  myVal += int_part;
  Serial.print(int_part);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0) {
   myVal += "."; 
    Serial.print(".");
  } 

  // Extract digits from the remainder one at a time
  while (digits-- > 0) 
  {
    remainder *= 10.0;
    int toPrint = int(remainder);
    myVal += toPrint;
    Serial.print(toPrint);
    remainder -= toPrint;
  }
  myVal += " ";
  Serial.print(" ");
} 
