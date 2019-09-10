
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

    osc.sendMessage("/sizeoffloat", "i", sizeof(float));
    osc.sendMessage("/sizeofdouble", "i", sizeof(double));
    
    osc.sendMessage( "/ms",  "i",   millis() );

    osc.sendMessage( "/msf",  "f",   millis() * 1.0);

    osc.sendMessage( "/msf10",  "f",   float(millis())*0.1 );

    osc.sendMessage( "/ten",  "f",   10.0);
    
    osc.sendMessage( "/zero",  "f",   0.0);

    osc.sendMessage( "/i",  "i",   10);
    
  }

}
