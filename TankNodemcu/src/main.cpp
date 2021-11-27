#include <Arduino.h>
#include <SPI.h>
#include "printf.h"
#include "RF24.h"

// instantiate an object for the nRF24L01 transceiver
RF24 radio(D4, D2); // using pin 7 for the CE pin, and pin 8 for the CSN pin

// Let these addresses be used for the pair
uint8_t address[][6] = {"1Node", "2Node"};
// It is very helpful to think of an address as a path instead of as
// an identifying device destination

// to use different addresses on a pair of radios, we need a variable to
// uniquely identify which address this radio will use to transmit

// For this example, we'll be using a payload containing
// a single float number that will be incremented
// on every successful transmission
typedef struct : Printable
{
  short unsigned int x;
  short unsigned int y;

  size_t printTo(Print &p) const
  {
    size_t count = 0;
    count += p.print(F("Payload{x = "));
    count += p.print(x);
    count += p.print(F(", y = "));
    count += p.print(y);
    count += p.print(F("}"));
    return count;
  }
} __attribute__((packed)) Payload;

Payload payload;

uint16_t xy[2];

void setup()
{

  Serial.begin(115200);

  // initialize the transceiver on the SPI bus
  if (!radio.begin())
  {
    Serial.println(F("radio hardware is not responding!!"));
    while (1)
    {
    } // hold in infinite loop
  }

  radio.setChannel(0x70);

  Serial.print("Channel: ");
  Serial.println(radio.getChannel(), HEX);

  Serial.print(F("address = "));
  Serial.println((char *)address[0]);

  // role variable is hardcoded to RX behavior, inform the user of this

  // Set the PA Level low to try preventing power supply related problems
  // because these examples are likely run with nodes in close proximity to
  // each other.
  radio.setPALevel(RF24_PA_MAX); // RF24_PA_MAX is default.
  radio.setDataRate(RF24_250KBPS);
  // radio.setAutoAck(true);
  // radio.enableAckPayload();
  radio.setRetries(0, 15);
  radio.setPayloadSize(32);
  radio.openReadingPipe(1, address[0]); // using pipe 1

  // radio.printPrettyDetails();

  // radio.powerUp();
  Serial.println("Staring listening");
  radio.startListening(); // put radio in RX mode
} // setup

void loop()
{
    // This device is a RX node

    uint8_t pipe;
    if (radio.available(&pipe))
    {                                         // is there a payload? get the pipe number that recieved it
      // digitalWrite(LED_BUILTIN, HIGH);
      // delay(100);
      // digitalWrite(LED_BUILTIN, LOW);
      // uint8_t bytes = radio.getPayloadSize(); // get the size of the payload
      radio.read(&xy, sizeof(xy));            // fetch payload from FIFO
      Serial.print(F("Received x: "));
      Serial.println(xy[0]);
      Serial.print(sizeof(xy)); // print the size of the payload
      Serial.print(F(" bytes on pipe "));
      Serial.print(pipe); // print the pipe number
      Serial.print(F("y: "));
      Serial.println(xy[1]);
      // Serial.println(payload); // print the payload's value
    }
} // loop
