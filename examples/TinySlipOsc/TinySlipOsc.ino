
// UDP BUFFERS
#define BUFFER_MAX_SIZE 256
char inputBuffer[BUFFER_MAX_SIZE];

#define BUFFER_MAX_SIZE 256
char outputBuffer[BUFFER_MAX_SIZE];

// OSC PARSER AND PACKER
#include <TinyOsc.h>
TinyOsc osc;

// SLIP PARSER AND PACKER
#include <TinySlip.h>
TinySlip slip;


int previousAnalogRead0;

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

  

  if ( osc.fullMatch("/led","i") ) {
    // GET THE OSC int ARGUMENT AS AN ARDUINO int
    int state = osc.getNextInt32(); 
    // !NOTE THAT TO GET THE FULL RANGE OF AN OSC int, USE long LIKE THIS:
    // long state = osc.getNextInt32();
    //digitalWrite(LED_BUILTIN, state);
  }

/*
  // IF THE ADDRESS IS /led AND THERE IS AN OSC int ('i') ARGUMENT
  if ( osc.fullMatch("/led","i") ) {
    // GET THE OSC int ARGUMENT AS AN ARDUINO int
    int state = osc.getNextInt32(); 
    // !NOTE THAT TO GET THE FULL RANGE OF AN OSC int, USE long LIKE THIS:
    // long state = osc.getNextInt32();
    digitalWrite(LED_BUILTIN, state);
    digitalWrite(4, state);
    
  // ELSE IF THE ADDRESS IS /touch AND THERE IS A float ('f') ARGUMENT  
  // (SOMETIMES APPLICATIONS SEND floats INSTEAD OF ints)
  } else if ( osc.fullMatch("/touch","f") ) {
    float f = osc.getNextFloat(); // GET THE float ARGUMENT
    // DO SOMETHING WITH f HERE (UP TO YOU)...
  }
*/
}


//==========
//== LOOP ==
//==========
void loop() {


  while ( slip.parseStream(&Serial,inputBuffer,BUFFER_MAX_SIZE) ) {
    
    int bufferSize = slip.available();

    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

    // PARSE THE OSC MESSAGES FROM THE BUFFER
    // FOR EVERY PARSED OSC MESSAGE IN  inputBuffer THE receivedOscMessage callback WILL BE CALLED
    // osc.parse( buffer name, size of the data, callback function)
    osc.parse( inputBuffer, bufferSize, receivedOscMessage);

  }

  int newAnalogRead0 = analogRead(0);
  if ( previousAnalogRead0 != newAnalogRead0) {
    previousAnalogRead0 = newAnalogRead0;

    // STEP 1 : WRITE THE OSC MESSAGE INTO THE OUTPUT BUFFER outputBuffer
    int oscMessageSize = osc.writeMessage( outputBuffer , BUFFER_MAX_SIZE,  "/a0",  "i",   newAnalogRead0 );
    
    // STEP 2 : STREAM THE OSC MESSAGE THROUGH SLIP
    slip.streamPacket(&Serial, outputBuffer, oscMessageSize);
    
  }


}
