#include <SoftwareSerial.h>
#include <ETPP.h>   //EasyTranser++

// INPUT
const int switchPin = 11;
const int buttonPin = 12;

// OUTPUT
const int LEDpin = 4;
const int buzzerPin = 6;

// Constants

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
    //use the atmega328 internal 20k pullup resistors
    //this means that HIGH is the normal state (+5v), and LOW is "closed" (0v, gnd)
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(switchPin, INPUT_PULLUP);

    pinMode(LEDpin, OUTPUT);
    pinMode(buzzerPin, OUTPUT);
    
    Serial.begin(9600);
    
    APC220.begin(2400); //lowest RX/TX baudrate APC220 supports so we get best signal propagation

    playStartupSong();
}
void loop() 
{
    //process buttons and switches
    txPacket_t p = processInputs();

    //output stuff based on our inputs
    processOutputs(p);
    
    //send the data
    tx.send(p);
    //wait 500msec between packet transfers
    delay(500);
}

txPacket_t processInputs()
{
    int buttonState = digitalRead(buttonPin);
    int switchState = digitalRead(switchPin);

    txPacket_t packet;    //all defaults to false
    if (switchState == LOW)
    {
        packet.armed = true;
        
        if (buttonState == LOW) //only check button if we're already armed!
        {
            packet.fire = true;
        }
    }
    return packet;
}
void processOutputs(const txPacket_t& packet)
{
    if (packet.armed)
    {
        digitalWrite(buzzerPin, HIGH);
        
        if (packet.fire)
        {
            digitalWrite(LEDpin, HIGH);     //turn on LED
        }
        else
        {
            digitalWrite(LEDpin, LOW);
        }
    }
    else
    {
        digitalWrite(buzzerPin, LOW);
    }
}
void buzz(int msecs)
{
    digitalWrite(buzzerPin, HIGH);
    delay(msecs);
    digitalWrite(buzzerPin, LOW);
}

/* A quick 'song' that the buzzer plays when we turn on the unit */
void playStartupSong()
{
    for(int i = 0; i < 4; ++i)
    {
        buzz(100);
        delay(50);
    }
}
