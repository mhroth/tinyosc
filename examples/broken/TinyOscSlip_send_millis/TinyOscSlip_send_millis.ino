// OSC PARSER AND PACKER
#include <TinyOsc.h>
TinyOsc osc;

// RECEIVE SERIAL DATA BUFFER
#define BUFFER_MAX_SIZE 256
unsigned char inputBuffer[BUFFER_MAX_SIZE];

// SLIP PARSER AND PACKER
#include <TinySlip.h>
TinySlip slip(&Serial);



#include <Chrono.h>
Chrono messageOutputChrono;

//===========
//== SETUP ==
//===========
void setup() {

  // INITIATE SERIAL COMMUNICATION
  Serial.begin(57600);

  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH);


}

// FUNCTION THAT IS CALLED FOR EVERY RECEIVED MESSAGE
void receivedOscMessage() {


}


//==========
//== LOOP ==
//==========
void loop() {


  size_t packetLength = slip.parsePacket(inputBuffer, BUFFER_MAX_SIZE);

 // IF WE RECEIVED A PACKET
  if ( packetLength > 0 ) {
    
    // PARSE THE OSC MESSAGES FROM THE BUFFER
    // FOR EVERY PARSED OSC MESSAGE IN  inputBuffer THE receivedOscMessage callback WILL BE CALLED
    // osc.parseMessages( callback function, buffer name, size of the data, )
    osc.parseMessages( receivedOscMessage, inputBuffer, packetLength );

    // FLASH THE LED TO INDICATE WE RECEIVED A PACKET
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

  }

  if ( messageOutputChrono.hasPassed(1000) ){
    
      messageOutputChrono.restart();
  
    slip.beginPacket();
    
    // STEP 2 : STREAM THE OSC MESSAGE THROUGH SLIP
    osc.writeMessage( &slip, "/ms",  "i",   millis() );
    
    slip.endPacket();

    
  }

}
