/*
  AnalogReadSerial
  Reads an analog input on pin 0, prints the result to the serial monitor.
  Graphical representation is available using serial plotter (Tools > Serial Plotter menu)
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.
*/

// starting @ a tick position and going to another tick position,
// returns how many degrees we've moved.
float degreeChange(int fromTick, int toTick);


// set on init so we know what the orientation "0" is
int initialPotentiometerReading = -1;

void writeCsvLabelLine();
void writeCsvDataLine(int rotaryTicks, int angleTicks, );

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  initialPotentiometerReading = analogRead(A0);
}

int lastSensorValue = -1; // cant be this

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);
  // look until change
  if (sensorValue == lastSensorValue) {
    return; // reloop
  }
  // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
 
  // print out the value you read:
  Serial.print(sensorValue);
  Serial.print(" est. angle delta = ");
  Serial.print(degreeChange(lastSensorValue, sensorValue));

  Serial.print(" total <) ");
  Serial.print(degreeChange(initialPotentiometerReading, sensorValue));
  
  Serial.println("");

  lastSensorValue = sensorValue;
  delay(500);        // delay in between reads for stability
}


float degreeChange(int fromTick, int toTick) {
  // this is the real world mapping of the degrees we've turned it to the internal
  // tick count. We suspect this is actually non-linear, which we'll fix later.
  const float realDegreesPerTicks = 30.0/700.00; // 30 deg turn = 0 to 700 tick

  // right now we're using a linear mapping of ticks to degrees,
  // but we'll change this later here if we need to
  int tickChange = toTick-fromTick; 
  float degChange = tickChange * realDegreesPerTicks;

  // we always want our degree output to be positive, so abs it
  return abs(degChange);
}

