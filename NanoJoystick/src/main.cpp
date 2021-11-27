#include <Arduino.h>
#include <SPI.h>
#include "printf.h"
#include "RF24.h"

// instantiate an object for the nRF24L01 transceiver
RF24 radio(9, 10); // using pin 7 for the CE pin, and pin 8 for the CSN pin

// Let these addresses be used for the pair
uint8_t address[][6] = {"1Node", "2Node"};
// It is very helpful to think of an address as a path instead of as
// an identifying device destination

// to use different addresses on a pair of radios, we need a variable to
// uniquely identify which address this radio will use to transmit

// For this example, we'll be using a payload containing
// a single float number that will be incremented
// on every successful transmission
// float payload = 0;

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

void
setup()
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
  radio.setAutoAck(true);
  // radio.enableAckPayload();
  // radio.setRetries(0, 15);
  // radio.setPayloadSize(32);
  radio.openWritingPipe(address[0]); // always uses pipe 0

  // save on transmission time by setting the radio to only transmit the
  // number of bytes we need to transmit a float
  // radio.setPayloadSize(sizeof(payload)); // float datatype occupies 4 bytes

  // set the TX address of the RX node into the TX pipe

  // set the RX address of the TX node into a RX pipe
  // radio.openReadingPipe(1, address[!radioNumber]); // using pipe 1
  radio.powerUp();

  // additional setup specific to the node's role
  radio.stopListening(); // put radio in TX mode
} // setup

void loop()
{
    // This device is a TX node
    unsigned long start_timer = micros();               // start the timer
    int x, y;
    x = analogRead(A0);
    y = analogRead(A1);
    Serial.print("X: ");
    Serial.print(x);
    Serial.print(" Y: ");
    Serial.println(y);
    xy[0] = x;
    xy[1] = y;
    digitalWrite(LED_BUILTIN, HIGH);
    // bool report = radio.write(&payload, sizeof(payload)); // transmit & save the report
    bool report = radio.write(&xy, sizeof(xy)); // transmit & save the report
    delay(200);
    Serial.print("Payload size = ");
    Serial.println(sizeof(xy));
    digitalWrite(LED_BUILTIN, LOW);

    unsigned long end_timer = micros();                 // end the timer

    // Serial.println(payload);

    if (report)
    {
      Serial.print(F("Transmission successful! ")); // payload was delivered
      Serial.print(F("Time to transmit = "));
      Serial.print(end_timer - start_timer); // print the timer result
      Serial.print(F(" us. Sent: x: "));
      Serial.print(xy[0]); // print payload sent
      Serial.print(F(" y: "));
      Serial.println(xy[1]); // print payload sent
      // payload += 1;         // increment float payload
    }
    else
    {
      Serial.println(F("Transmission failed or timed out")); // payload was not delivered
    }

    // to make this example readable in the serial monitor
    delay(100); // slow transmissions down by 1 second

} // loop
