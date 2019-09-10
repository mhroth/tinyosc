
#include <TinyOscSlip.h>
TinyOscSlip osc(&Serial);



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


  osc.receiveMessages(receivedOscMessage);

  if ( messageOutputChrono.hasPassed(1000) ){
    
      messageOutputChrono.restart();
  

    
    // STEP 2 : STREAM THE OSC MESSAGE THROUGH SLIP
    osc.sendMessage( "/ms",  "i",   millis() );
    

    
  }

}
