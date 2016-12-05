int pinA = 3;  // Connected to CLK on KY-040
int pinB = 4;  // Connected to DT on KY-040

int rotaryTicks = 0;
int lastRotaryTicks = 0;

int lastRotaryValue; // 1/0 read from rotary

boolean rotatingClockwise;

// set on init so we know what the orientation "0" is
int initialPotentiometerReading = -1;

int lastPotentiometerValue = -1; // cant be -1

// smooths out potentiometer readingts b/c jitter sucks
void addNewPoteniometerPointSample(int newPotentValue);
int rollingAveragePotent();


// util functions for writing in CSV format
void writeCsvLabel();
void writeCsvRow(int rotaryTickCount, int potentTickCount, double x, double y);


// hacky because for some reason we can print in the setup() fxn
bool wroteCsvHeader;
 
void setup() { 
    pinMode(pinA,INPUT);
    pinMode(pinB,INPUT);
    /* Read Pin A
    Whatever state it's in will reflect the last position   
    */
    lastRotaryValue = digitalRead(pinA);
    initialPotentiometerReading = analogRead(A0);
    wroteCsvHeader = false;
    Serial.begin (9600);
    Serial.println("Now running!");
}

 void loop() { 
  // if they disconnect from reading the output stream, resend the header data on reconnec
  if (!Serial) {
    wroteCsvHeader = false;
  }

  if (!wroteCsvHeader && Serial) // if(Serial) only available on Leonardo: detects if tty connected
  {
    delay(150); // 150ms for the client to prepare for display cause clients were slow & dropping label packets
    writeCsvLabel();
    wroteCsvHeader = true;
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

   
    Serial.print("rotary="); Serial.print(rotaryTicks);
    Serial.print(", potent = "); Serial.println(newPotentiometerValue);

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
  Serial.println("LABEL,RotaryEncoder,Potentiometer");
}

void writeCsvRow(int rotaryTickCount, int potentTickCount)
{
  // these need to be in the same order as writeCsvLabel()'s implementation
  Serial.print("DATA");
  Serial.print(","); Serial.print(rotaryTickCount);
  Serial.print(","); Serial.print(potentTickCount);
  Serial.println();
}

