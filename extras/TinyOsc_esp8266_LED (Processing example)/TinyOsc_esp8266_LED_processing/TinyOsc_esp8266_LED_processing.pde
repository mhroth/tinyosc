// INTALL oscP5 FROM THE LIBRARY MANAGER

/*
  RECEIVED OSC MESSAGES:
  /button 0 : set the background to white
  /button 1 : set the background to black
 
  SENT OSC MESSAGES:
  /led 1 : sent when the mouse is pressed
  /led 0 : sent when the mouse is released

 */
 
import oscP5.*;
import netP5.*;
  
OscP5 oscP5;

   /* myRemoteLocation is a NetAddress. a NetAddress takes 2 parameters,
   * an ip address and a port number. myRemoteLocation is used as parameter in
   * oscP5.send() when sending osc packets to another computer, device, 
   * application. usage see below. for testing purposes the listening port
   * and the port of the remote location address are the same, hence you will
   * send messages back to this sketch.
   */
NetAddress remoteLocation = new NetAddress("192.168.25.10",7777);


color backgroundColor = color(0);

void setup() {
  size(400,400);

  /* start oscP5, listening for incoming messages at port 7890 */
  oscP5 = new OscP5(this,7890);
  

}


void draw() {
  background(backgroundColor);  
}

void mousePressed() {
  /* in the following different ways of creating osc messages are shown by example */
  OscMessage message = new OscMessage("/led");
  
  message.add(1); /* add an int to the osc message */

  /* send the message */
  oscP5.send(message, remoteLocation); 
}

void mouseReleased() {
  /* in the following different ways of creating osc messages are shown by example */
  OscMessage message = new OscMessage("/led");
  
  message.add(0); /* add an int to the osc message */

  /* send the message */
  oscP5.send(message, remoteLocation); 
}


/* incoming osc message are forwarded to the oscEvent method. */
void oscEvent(OscMessage theOscMessage) {
  /* print the address pattern and the typetag of the received OscMessage */
  println("### received an osc message:");
  println("addrpattern: "+theOscMessage.addrPattern());
  println("typetag: "+theOscMessage.typetag());
  
  String address = theOscMessage.addrPattern();

  if ( address.equals("/button") ) {
       
    // Get the first int value
    int intValue = theOscMessage.get(0).intValue();
    if ( intValue == 0 )  backgroundColor = color(0);
    else  backgroundColor = color(255);
    
  }

}
