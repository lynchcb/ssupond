// ############################################
// CONFIG SECTION
bool veryVerboseDebug = true;

int pinA = 3;  // Connected to CLK on KY-040
int pinB = 4;  // Connected to DT on KY-040

int rotaryTicks = 0;
int lastRotaryTicks = 0;

const String usbFileName = "PONDS.PD";

// every n writes we manually close the file and reopen to prevent data
// loss. Lower n = slower perf, but safer
const int USB_WRITE_MANUAL_FLUSH_RATE = 2;

// END CONFIG SECT
// ############################################

int lastRotaryValue; // 1/0 read from rotary

boolean rotatingClockwise;

// set on init so we know what the orientation "0" is
int initialPotentiometerReading = -1;

int lastPotentiometerValue = -1; // cant be -1

// smooths out potentiometer readingts b/c jitter sucks
void addNewPoteniometerPointSample(int newPotentValue);
int rollingAveragePotent();


bool isUsbStickPluggedIn();

// open a filename
void usbOpenFile4Write(const String& filename);
// write some data to an open file
void write2file(const String& data);

// util functions for writing in CSV format
void writeCsvLabel();
void writeCsvRow(int rotaryTickCount, int potentTickCount, double x, double y);

// read all the input coming back from the usb controller
String dumpUsbOutput();

// this is just to add some dealy so that the usb device can start up
// before we do operations on it
long int started;
bool usbDeviceWasSetup;
// Write data to the usb device. If command must have the \r on it!
void usbDeviceWrite(const String& data);
// same as usbDeviceWrite but doesn't read all the response input after writing
// to the device
void usbDeviceWriteUnbuffered(const String& data);

// setup the connection to the usb device
void setupUsbDevice();
void setupUsbStickForWrite(); // open files and add new section delimeter

// alias to make clear that Serial1 is the usb device
HardwareSerial& usbDev = Serial1;

// whether the current usb stick has been 'prepd' for writing data
// basically whether the file is open for writing and some other stuff
bool usbStickIsPrepped = false;
 
void setup() { 
    pinMode(pinA, INPUT);
    pinMode(pinB,  INPUT);
    /* Read Pin A
    Whatever state it's in will reflect the last position   
    */
    lastRotaryValue = digitalRead(pinA);
    initialPotentiometerReading = analogRead(A0);
    Serial.begin (9600);
    Serial.println("Now running!");

    started = millis();
    usbDeviceWasSetup = false;
}

void loop() {
  dumpUsbOutput();
  
  if (millis() - started < 1000) {
    Serial.println("skipped");
    return;
  }

  if (!usbDeviceWasSetup) {
    usbDeviceWasSetup = true;
    setupUsbDevice();
    setupUsbStickForWrite();
    delay(150); // 150ms for the client to prepare for display cause clients were slow & dropping label packets
    writeCsvLabel();
    dumpUsbOutput();
  }
  
   int newRotaryValue = digitalRead(pinA);
   addNewPoteniometerPointSample(analogRead(A0));
   int newPotentiometerValue = rollingAveragePotent();


   // we only care when the nob rotates. If not, re-loop
   if (newRotaryValue == lastRotaryValue && abs(newPotentiometerValue-lastPotentiometerValue) < 5) // jitter
   {
      return; // reloop
   }
  
   if (newRotaryValue != lastRotaryValue) {
    // if the knob is rotating, we need to determine direction
    // We do that by reading pin B.
    lastRotaryTicks = rotaryTicks;
    if (digitalRead(pinB) != newRotaryValue) {  // Means pin A Changed first - We're Rotating Clockwise
     rotaryTicks++;
     rotatingClockwise = true;
    } else {// Otherwise B changed first and we're moving CCW
     rotatingClockwise = false;
     rotaryTicks--;
    }
   }



    if (Serial) {
      Serial.print("rotary="); Serial.print(rotaryTicks);
      Serial.print(", potent = "); Serial.println(newPotentiometerValue);
    }

    writeCsvRow(rotaryTicks, newPotentiometerValue);

    // ================================================
    // end of loop
    lastRotaryValue = newRotaryValue;
    lastPotentiometerValue = newPotentiometerValue;
}

// sometimes we get stuck between two discrete values
const int JITTER_SAMPLES_COUNT = 4;
int potentData[JITTER_SAMPLES_COUNT];
int jitter_pointer = 0;

void addNewPoteniometerPointSample(int newPotentValue)
{
  potentData[jitter_pointer++ % JITTER_SAMPLES_COUNT] = newPotentValue;
  jitter_pointer = jitter_pointer%JITTER_SAMPLES_COUNT;
}

int rollingAveragePotent()
{
  int sum = 0;
  for(int i = 0; i < JITTER_SAMPLES_COUNT; i++) sum += potentData[i];
  return sum/JITTER_SAMPLES_COUNT;
}

// And now for CSV utility functions
void writeCsvLabel()
{
  // we write the read sensor values first to preserve the raw data and then our interpreted/calculated x,y points
  // in the cartesian plane
  // so it's (r, theta, x, y)
  String header = "RotaryEncoder,Potentiometer,MillisSinceStart\n";
  if (Serial) {
    Serial.print("LABEL,");
    Serial.println(header);
  }

  write2file(header);
}

void writeCsvRow(int rotaryTickCount, int potentTickCount)
{
  // these need to be in the same order as writeCsvLabel()'s implementatio
  if (Serial) {
    Serial.print("DATA");
    Serial.print(","); Serial.print(rotaryTickCount);
    Serial.print(","); Serial.print(potentTickCount);
  }

  String dataLine = "";
  dataLine += rotaryTickCount;
  dataLine += ',';
  dataLine += potentTickCount;
  dataLine += ',';
  dataLine += millis();
  dataLine += '\n';

  if (veryVerboseDebug)
  {
    Serial.print("-----------Would have written: (");
    Serial.print(dataLine.length());
    Serial.println(" characters)");
    Serial.println(dataLine);
    Serial.println("----------------------");
  }

  write2file(dataLine);
}

void usbDeviceWriteUnbuffered(const String& data)
{
  if (veryVerboseDebug) {
    Serial.println("DEBUG: WRITING 2 USB:");
    Serial.println(data);
  }
  usbDev.write(data.c_str(), data.length());
  delay(data.length());
  usbDev.flush();
  delay(data.length());
}

void usbDeviceWrite(const String& data)
{
  usbDeviceWriteUnbuffered(data);
  dumpUsbOutput();
}

void setupUsbDevice()
{
  usbDev.begin(9600);
  dumpUsbOutput();
  usbDeviceWrite("failingspacercommand\r");
  usbDeviceWrite("IPA\r");
  usbDeviceWrite("ECS\r");
  delay(100);
}

void setupUsbStickForWrite()
{
  // open a file for writing
  usbOpenFile4Write(usbFileName);
  // special delimeter for mathematica to know that
  // this is a new section of the file
  write2file("########## NEW SECTION ##########\n");
  usbDeviceWrite("plzfail\r");
}


int write2fileCount = 0;

void write2file(const String& data) {
  String writeCommand = "WRF ";
  writeCommand += data.length();
  writeCommand += '\r';
  writeCommand += data;
  usbDeviceWrite(writeCommand);
  delay(150);

  if (++write2fileCount >= USB_WRITE_MANUAL_FLUSH_RATE) {
    Serial.println("Manually closing & reopening datafile.");
    write2fileCount = 0;
    usbDeviceWrite("CLF\r");
    usbOpenFile4Write(usbFileName);
  }
}

String dumpUsbOutput()
{
    if (usbDev.available()) {
    String fromUsb = "";
    while (usbDev.available()) {
      char readC = usbDev.read();
      fromUsb += readC;
    }

    // wait 10ms to see if anything else comes
    bool delayWorked = true;
    while (delayWorked) {
      delayWorked = false;
      delay(20);
      while (usbDev.available()) {
        delayWorked = true;
        char readC = usbDev.read();
        fromUsb += readC;
      }
    }
    

    Serial.println("###########################");
    Serial.println("From USB STICK:");
    Serial.println(fromUsb);
    Serial.println("###########################");

    return fromUsb;
  }

  return "";
}

void usbOpenFile4Write(const String& filename)
{
  usbDeviceWrite("OPW " + filename + "\r");
  delay(200);
  dumpUsbOutput();
}

bool isUsbStickPluggedIn()
{
  usbDeviceWriteUnbuffered("DVL\r");
  delay(100);
  String response = dumpUsbOutput();
  return response != "No Device blalbla";
}


