import processing.serial.*;
import ddf.minim.*;

Serial myPort;
Minim minim;
AudioPlayer redLightplayer;
AudioPlayer greenLightplayer;
byte greenLightCode = 97;
byte redLightCode = 98;

void setup()
{
  printArray(Serial.list());
  // In the next line, you'll need to change this based on your USB port name
  myPort = new Serial(this, "/dev/tty.usbserial-140", 115200);
  myPort.setDTR(true);
  myPort.bufferUntil(13);
  
  minim = new Minim(this);

  // Put in the name of your sound file below, and make sure it is in the same directory
  redLightplayer = minim.loadFile("red-light-sound.mp3");
  greenLightplayer = minim.loadFile("green-light-sound.mp3");
}

void draw() {
  while (myPort.available() > 0) {
    int inByte = myPort.read();
    
    if (inByte == redLightCode) {
      redLightplayer.rewind();
      redLightplayer.play();
    }
    if (inByte == greenLightCode) {
      greenLightplayer.rewind();
      greenLightplayer.play();
    }
  }
}
