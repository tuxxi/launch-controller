#include <SoftwareSerial.h>
#include <ETPP.h>   //EasyTranser++

// OUTPUT
const int transistorPin = 6;    // connected to the base of the transistor
const int LEDpin = 4;

// Constants
const uint16_t FIRE_TIME = 200;    //msec

//SOFTARE SERIAL for APC220
SoftwareSerial APC220(2,3); // RX, TX

//The packet that we're sending and receiving
struct txPacket_t
{
    bool armed {false};
    bool continuity {false};
    bool fire {false};
};

//Init the EasyTransfer++ serialization object
EasyTransferPP<Stream, txPacket_t> tx(APC220);

void setup()
{
    pinMode(transistorPin, OUTPUT);
    pinMode(LEDpin, OUTPUT);
    
    Serial.begin(9600);
    
    APC220.begin(2400); //lowest RX/TX baudrate APC220 supports so we get best signal propagation
}

void loop()
{   
    if (tx.receive())
    {
        Serial.println("received packet");
        const txPacket_t& p = tx.receiveBuffer();
        Serial.print("armed: ");
        Serial.print(p.armed);
        Serial.print(", fire: ");
        Serial.println(p.fire);        
        processOutputs(p);
    }
}
void processOutputs(const txPacket_t& packet)
{
    if (packet.armed)
    {
        digitalWrite(LEDpin, HIGH);
        if (packet.fire)
        {
            fireIgnitor();
        }
    }
    else
    {
        digitalWrite(LEDpin, LOW);
    }
}
void fireIgnitor()
{
    digitalWrite(transistorPin, HIGH);  // turn on ematch
    Serial.println("FIRING!");
    
    delay(FIRE_TIME);
    
    digitalWrite(transistorPin, LOW);   // turn off ematch
}

