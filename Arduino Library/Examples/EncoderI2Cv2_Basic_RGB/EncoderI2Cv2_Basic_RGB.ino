#include <Wire.h>
#include "i2cEncoderLibV2.h"

/*This is a basic example for using the I2C RGBEncoder V2
  The counter is set to work between +10 to -10, at every encoder click the counter value is printed on the terminal.
  It's also printet when the push button is pressed, or released or double pushed.


  Connections with Arduino UNO:
  - -> GND
  + -> 5V
  SDA -> A4
  SCL -> A5
  INT -> A3

  Connections with WeMos D1 mini:
  - -> GND
  + -> 5V
  SDA -> GPIO  4 (19) / D2
  SCL -> GPIO  5 (20) / D1
  INT -> GPIO 15 (16) / D8

*/
const int sclPin = D1;
const int sdaPin = D2;
const int intPin = D7;

//Class initialization with the I2C addresses
i2cEncoderLibV2 RGBEncoder(0x01); /* In this case i'm using the I2C address 1, change according to your device*/

volatile byte knobsHaveChanged = false;
static void handleIrq() {
  knobsHaveChanged = true;
}

void setup(void)
{
  Wire.begin(sdaPin, sclPin);
  pinMode(intPin, INPUT_PULLUP);

  Serial.begin(115200);
  Serial.println("**** I2C RGBEncoder V2 basic example ****");

  scani2c();
  /*
        INT_DATA= The register are considered integer.
        WRAP_DISABLE= The WRAP option is disabled
        DIRE_LEFT= RGBEncoder left direction increase the value
        IPUP_ENABLE= INT pin have the pull-up enabled.
        RMOD_X1= RGBEncoder configured as X1.
        RGB_ENCODER= type of encoder is RGB, change to STD_ENCODER in case you are using a normal rotary encoder.
  */

  RGBEncoder.begin(INT_DATA | WRAP_DISABLE | DIRE_LEFT | IPUP_ENABLE | RMOD_X1 | RGB_ENCODER);
  //  RGBEncoder.begin(INT_DATA | WRAP_DISABLE | DIRE_LEFT | IPUP_ENABLE | RMOD_X1 | STD_ENCODER); // try also this!
  //  RGBEncoder.begin(INT_DATA | WRAP_ENABLE | DIRE_LEFT | IPUP_ENABLE | RMOD_X1 | RGB_ENCODER);  // try also this!

  RGBEncoder.writeCounter((int32_t)0); /* Reset the counter value */
  RGBEncoder.writeMax((int32_t)10); /* Set the maximum threshold*/
  RGBEncoder.writeMin((int32_t) - 10); /* Set the minimum threshold */
  RGBEncoder.writeStep((int32_t)1); /* Set the step to 1*/
  RGBEncoder.writeInterruptConfig(0xff); /* Enable all the interrupt -- seems to be the bits in encoder status (ENCODER_STATUS_CASE)*/
  RGBEncoder.writeAntibouncingPeriod(20);  /* Set an anti-bouncing of 200ms */
  RGBEncoder.writeDoublePushPeriod(50);  /*Set a period for the double push of 500ms */

  RGBEncoder.writeFadeRGB(1); //Set the fade with 1ms step

  // IRQ will clear on next slave read.
  attachInterrupt(digitalPinToInterrupt(intPin), handleIrq, FALLING);
}

void scani2c()
{
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++ )
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
}

void checkKnobs() {
  //  uint8_t enc_cnt;
  if (!RGBEncoder.updateStatus()) {
    Serial.println("NO change");
    return;
  };
if (0) {
  if ( RGBEncoder.readStatus(RINC)) {
    Serial.print("Increment: ");
    Serial.println( RGBEncoder.readCounterByte());

    /* Write here your code */

  }
  if ( RGBEncoder.readStatus(RDEC)) {
    Serial.print("Decrement: ");
    Serial.println( RGBEncoder.readCounterByte());

    /* Write here your code */

  }

  if ( RGBEncoder.readStatus(RMAX)) {
    Serial.print("Maximum threshold: ");
    Serial.println( RGBEncoder.readCounterByte());

    /* Write here your code */

  }

  if ( RGBEncoder.readStatus(RMIN)) {
    Serial.print("Minimum threshold: ");
    Serial.println( RGBEncoder.readCounterByte());

    /* Write here your code */

  }

  if ( RGBEncoder.readStatus(PUSHR)) {
    Serial.println("Push button Released");

    /* Write here your code */

  }

  if ( RGBEncoder.readStatus(PUSHP)) {
    Serial.println("Push button Pressed");

    /* Write here your code */

  }

  if ( RGBEncoder.readStatus(PUSHD)) {
    Serial.println("Double push!");

    /* Write here your code */

  }
}
  if (RGBEncoder.readStatus(RMAX) || RGBEncoder.readStatus(RMIN) ) {
    RGBEncoder.writeLEDR(0xff); //If the limits are reached turn on the RED LED
    RGBEncoder2.writeLEDR(0xff);
  } else {
    if (RGBEncoder.readStatus(RINC) || RGBEncoder.readStatus(RDEC)) {
      RGBEncoder.writeLEDG(0xff); //If the encoder is moved turn on the Green LED
    }
  }

  if (RGBEncoder.readStatus(PUSHR)) {
    RGBEncoder.writeLEDB(0xff);  //If the encoder is pushed turn on the BLUE LED
  }

  if (RGBEncoder.readInt2(FADE_INT)) { //Check if the fade proccess finished, if yes turn off the LEDs
    RGBEncoder.writeLEDR(0x00);
    RGBEncoder.writeLEDG(0x00);
    RGBEncoder.writeLEDB(0x00);
  }

  Serial.println(RGBEncoder.readCounterInt());
}

void loop() {
  if (knobsHaveChanged) {
    knobsHaveChanged = false;
    checkKnobs();
  }
}

