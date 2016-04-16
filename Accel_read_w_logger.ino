#include <SD.h>
#include <Wire.h>
#include "RTClib.h"

// how many milliseconds between grabbing data and logging it. 1000 ms is once a second
#define LOG_INTERVAL 100 // mills between entries (reduce to take more/faster data)

// how many milliseconds before writing the logged data permanently to disk
// set it to the LOG_INTERVAL to write each time (safest)
// set it to 10*LOG_INTERVAL to write all data every 10 datareads, you could lose up to 
// the last 10 reads if power is lost but it uses less power and is much faster!
#define SYNC_INTERVAL 1*LOG_INTERVAL // mills between calls to flush() - to write data to the card
uint32_t syncTime = 0; // time of last sync()

#define ECHO_TO_SERIAL   1 // echo data to serial port
#define WAIT_TO_START    0 // Wait for serial input in setup()

// the digital pins that connect to the LEDs
#define redLEDpin 2
#define greenLEDpin 3


#define aref_voltage 3.3         // we tie 3.3V to ARef and measure it with a multimeter!
#define bandgap_voltage 1.1      // this is not super guaranteed but its not -too- off

RTC_DS1307 RTC; // define the Real Time Clock object

// for the data logging shield, we use digital pin 10 for the SD cs line
const int chipSelect = 10;

const int z = A0;
const int y = A1;
const int x = A2;

const int sens = 300; // mv/g from spec sheet

const int x_off = 510;
const int y_off = 506;
const int z_off = 520;

const int button = 5;  //input pin for push button
int val = 0;  



// the logging file
File logfile;

void error(char *str)
{
  Serial.print("error: ");
  Serial.println(str);
  
  // red LED indicates error
  digitalWrite(redLEDpin, HIGH);

  while(1);
}



void setup()
{
  analogReference(EXTERNAL);
  Serial.begin(9600);      // sets the serial port to 9600
  pinMode(button, INPUT);

   // use debugging LEDs
  pinMode(redLEDpin, OUTPUT);
  pinMode(greenLEDpin, OUTPUT);
  
  #if WAIT_TO_START
    Serial.println("Type any character to start");
    while (!Serial.available());
  #endif //WAIT_TO_START

  // initialize the SD card
  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    error("Card failed, or not present");
  }
  Serial.println("card initialized.");
  
  // create a new file
  char filename[] = "LOGGER00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE); 
      break;  // leave the loop!
    }
  }
  
  if (! logfile) {
    error("couldnt create file");
  }
  
  Serial.print("Logging to: ");
  Serial.println(filename);

  // connect to RTC
  Wire.begin();  
  if (!RTC.begin()) {
    logfile.println("RTC failed");
  #if ECHO_TO_SERIAL
    Serial.println("RTC failed");
  #endif  //ECHO_TO_SERIAL
  }
  

  logfile.println("millis \t stamp \t datetime \t x (G) \t y (G) \t z (G) \n ");    
#if ECHO_TO_SERIAL
  Serial.println("millis \t stamp \t datetime \n");
#endif //ECHO_TO_SERIAL
 
  // If you want to set the aref to something other than 5v
  analogReference(EXTERNAL);
  
}

void loop()
{

  DateTime now;

  // delay for the amount of time we want between readings
  delay((LOG_INTERVAL -1) - (millis() % LOG_INTERVAL));
  
  digitalWrite(greenLEDpin, HIGH);

  // log milliseconds since starting
  uint32_t m = millis();
   


  // fetch the time
  now = RTC.now();

  #if ECHO_TO_SERIAL
    Serial.print(m);         // milliseconds since start
    Serial.print("\t "); 
    Serial.print(now.unixtime()); // seconds since 1/1/1970
    Serial.print("\t ");
    Serial.print('"');
    Serial.print(now.year(), DEC);
    Serial.print("/");
    Serial.print(now.month(), DEC);
    Serial.print("/");
    Serial.print(now.day(), DEC);
    Serial.print(" ");
    Serial.print(now.hour(), DEC);
    Serial.print(":");
    Serial.print(now.minute(), DEC);
    Serial.print(":");
    Serial.print(now.second(), DEC);
    Serial.print('"');
  #endif //ECHO_TO_SERIAL
  
  int xRaw = analogRead(x);       // read analog input pin 0
  int yRaw = analogRead(y);       // read analog input pin 1
  int zRaw = analogRead(z);       // read analog input pin 1

  float x_g = (xRaw - x_off) * (3300 / 1023.0) / sens; 
  float y_g = (yRaw - y_off) * (3300 / 1023.0) / sens; 
  float z_g = (zRaw - z_off) * (3300 / 1023.0) / sens; 
  //float x_g = map(xRaw, 0, 1023, -3.0, 3.0);
  val = digitalRead(button);
  //Serial.print(val);
  //Serial.print("\t");
  if (val == HIGH){
    // log time
    logfile.print(m);           // milliseconds since start
    logfile.print("\t "); 
    logfile.print(now.unixtime()); // seconds since 1/1/1970
    logfile.print("\t ");
    logfile.print('"');
    logfile.print(now.year(), DEC);
    logfile.print("/");
    logfile.print(now.month(), DEC);
    logfile.print("/");
    logfile.print(now.day(), DEC);
    logfile.print(" ");
    logfile.print(now.hour(), DEC);
    logfile.print(":");
    logfile.print(now.minute(), DEC);
    logfile.print(":");
    logfile.print(now.second(), DEC);
    logfile.print('"');
    // logfile.print("\t x (G), y (G), z (G): ");
    logfile.print("\t");
    logfile.print(x_g);    // print the acceleration in the X axis
    logfile.print("\t");       // prints a space between the numbers
    logfile.print(y_g);    // print the acceleration in the Y axis
    logfile.print("\t");       // prints a space between the numbers
    logfile.println(z_g);  // print the acceleration in the Z axis
    #if ECHO_TO_SERIAL  
      // Serial.print(" x (G), y (G), z (G): ");
      Serial.print("\t");
      Serial.print(x_g);    // print the acceleration in the X axis
      Serial.print("\t");       // prints a space between the numbers
      Serial.print(y_g);    // print the acceleration in the Y axis
      Serial.print("\t");       // prints a space between the numbers
      Serial.println(z_g);  // print the acceleration in the Z axis
    #endif // ECHO_TO_SERIAL
    //delay(100);              // wait 100ms for next reading
  } else {
    #if ECHO_TO_SERIAL
      Serial.println("\t Button not pushed \n");
      delay(10);
    #endif
  }
  
  // Now we write data to disk! Don't sync too often - requires 2048 bytes of I/O to SD card
  // which uses a bunch of power and takes time
  if ((millis() - syncTime) < SYNC_INTERVAL) return;
  syncTime = millis();
  
  // blink LED to show we are syncing data to the card & updating FAT!
  digitalWrite(redLEDpin, HIGH);
  logfile.flush();
  digitalWrite(redLEDpin, LOW);
  
}
