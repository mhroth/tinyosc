#include <TinyOscSlip.h>
TinyOscSlip osc(&Serial);


#include <Chrono.h>
Chrono messageOutputChrono;

unsigned char data[20];


//===========
//== SETUP ==
//===========
void setup() {

  // INITIATE SERIAL COMMUNICATION
  Serial.begin(57600);

  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH);

  for ( int i =0; i < 20 ; i++ ) {
    data[i] = i;
  }

}

// FUNCTION THAT IS CALLED FOR EVERY RECEIVED MESSAGE
void receivedOscMessage() {


}


//==========
//== LOOP ==
//==========
void loop() {

  osc.receiveMessages(receivedOscMessage);

  if ( messageOutputChrono.hasPassed(1000) ){
    
      messageOutputChrono.restart();
  
    // STEP 2 : STREAM THE OSC BLOB THROUGH SLIP
    osc.sendMessage( "/blob",  "b",   20, data );

    
  }

}
