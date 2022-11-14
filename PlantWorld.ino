#include <Servo.h>

// Pin setup
int treeServoPin = 6;
int camServoPin = 5;
// int extraServoPin = 12;
Servo treeServo;
Servo camServo;
// Servo extraServo;
//int treeUpReedPin = 9;
//int treeMidReedPin = 10;
int treeSwitchPin = 11;
int camReedPin = 10;
// int extraReedPin = 3;
// int extraLDRPin = A0;  //Not working on soldered board
int frontLDRPin = A0;
int rightLDRPin = A2;
int leftLDRPin = A1;
// int extraLDRPin = A4

bool camBottom = false;
int camState = 0;
int camSpeed = 0;

bool treeBottom = false;
int treeState = 0;
int treeSpeed = 0;
int treeTimer = 0;
int treeOscillateOffset = 0; 
bool treeGoingToBottom = false;
bool treeDownLowVals = true;



int worldState = 0;
int userPresentTimer = 0;
bool userInteracted = false;
int leftLDRVal = 0;

void setup() {
  Serial.begin(9600);

  treeServo.attach(treeServoPin);
  camServo.attach(camServoPin);

  pinMode(treeSwitchPin, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(treeSwitchPin), treeDownISR, RISING);
  pinMode(camReedPin, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(camReedPin), camISR, RISING);
  pinMode(leftLDRPin, INPUT);
  
}

void loop() {

  Serial.print("World: ");
  Serial.print(worldState);
  Serial.print(" Tree: ");
  Serial.print(treeState);
  Serial.print(" Cam: ");
  Serial.print(camState);
  Serial.print(" Presence: ");
  Serial.print(userPresent());
  Serial.print(" Interacted: ");
  Serial.print(userInteracted);
  Serial.print(" FrontLDR: ");
  Serial.print(analogRead(frontLDRPin));
  Serial.print(" LeftLDR: ");
  Serial.print(analogRead(leftLDRPin));
  Serial.print(" RightLDR: ");
  Serial.print(analogRead(rightLDRPin));
  Serial.println("");
  checkUserInteraction();
  updateWorld(); //change the world state
  updateTree();
  updateCam();
}

void updateWorld(){
  switch(worldState){
    case 0:
      if(treeState == 1 && camState == 1){
        worldState = 1;
      }
      break;
    case 1:
      if(userPresent()){
        worldState = 2;
      }
      break;
    case 2:
      if(treeState == 3 && camState == 3){
        worldState = 3;
        userInteracted = false;
        userPresentTimer = millis() + 2000;
      }
      break;
    case 3:
      if(!userPresent() && millis() > userPresentTimer){
        worldState = 0;
      }else if(userPresent()){
        userPresentTimer = millis() + 2000;
      }
      if(userInteracted){
        userInteracted = false;
        worldState = 4;
      }
      break;
    case 4:
      if(!userPresent() && millis() > userPresentTimer){
        worldState = 0;
      }else{
        userPresentTimer = millis() + 2000;
      }
      if(camState == 5 && treeState == 5){
        worldState = 5;
      }
      break;
    case 5:
      if(!userPresent() && millis() > userPresentTimer){
        worldState = 0;
      }else if(userPresent()){
        userPresentTimer = millis() + 2000;
      }
      break;
  }
}

void updateCam(){
  if(!analogRead(camReedPin)){
    camBottom = false;
  }
  if(worldState == 0 && camState > 1 ){
    camState = 0;
  }
  switch(camState){
    case 0:
      camState = 1;
      break;
    case 1:
      setCamSpeed(120);
      if(worldState == 2){
        camState = 2;
      }
      break;
    case 2:
        setCamSpeed(90);
        camState = 3;
      break;
    case 3:
      setCamSpeed(80);
      if(worldState == 4){
        camState = 4;
      }
      break;
    case 4:
      setCamSpeed(130);
      camState = 5;
      break;
    case 5:
      setCamSpeed(130);
      break;
  }
}

void updateTree() {
  if(worldState == 0 && treeState > 1 ){
    treeState = 0;
  }
  int desiredSpeed;
  if(treeGoingToBottom){
    return;
  }
  switch (treeState) {
    case 0:
      treeOscillateOffset = 0 ;
      if(!treeGoingToBottom){
       treeToBottom();
      }
      if(treeBottom){
        treeState = 1;
        treeTimer = millis() + 2000;
      }
      break;
    case 1:
      if(millis() < treeTimer) {
          setTreeSpeed(-15);
          treeBottom = false;    
      }else{
        if(treeOscillateOffset == 0){
          treeOscillateOffset = millis();
        }
         int speed = oscillate(-22, 25, treeOscillateOffset, 120);
         setTreeSpeed(speed);
      }
      if(worldState == 2){
        treeState = 2;
      }
      break;
    case 2:
       if(!treeGoingToBottom){
       treeToBottom();
      }
      if(treeBottom){
        treeState = 3;
        treeTimer = millis() + 500;
      }
      break;
    case 3:
      if(millis() < treeTimer) {
          setTreeSpeed(-15);
          treeBottom = false;    
      }else{
        setTreeSpeed(0);
      }
      if(worldState == 4){
        treeState = 4;
      }
      break;
    case 4:
      treeState = 5;
      treeTimer = millis() + 3000;
      break;
    case 5:
     if(millis() < treeTimer) {
          setTreeSpeed(-15);
          treeBottom = false;
        }else{
          setTreeSpeed(0);
        }
      break;
  }

}

void setTreeSpeed(int speed){
  if(speed == 0){
    treeServo.detach();
  }
  if(speed != treeSpeed){
    treeServo.attach(treeServoPin);
    Serial.print("Tree Speed: ");
    Serial.println(speed);
    if(treeDownLowVals){
      treeSpeed = -speed;
     }else{
      treeSpeed = -speed;
    }

    treeServo.attach(treeServoPin);
    treeServo.write(92+treeSpeed);
  }
}

void setCamSpeed(int speed){
  if(camSpeed != speed){
    camSpeed = speed;
  }
  if(abs(speed-90)<4){
    camServo.detach();
  }else{
    camServo.attach(camServoPin);
    camServo.write(speed);
  }
}

void treeToBottom(){
  if(!treeBottom){
    int speed;
    treeGoingToBottom = true;
    if(treeDownLowVals){
      speed = 20;
    }else{
      speed = -20;
    }
    setTreeSpeed(speed);
  }
}

bool userPresent(){
  if(analogRead(frontLDRPin)>220){
    return true;
  }
  return false;
}

void checkUserInteraction(){
  if(worldState == 3 && analogRead(leftLDRPin)>620 || analogRead(rightLDRPin)>600){
    userInteracted = true;
  }
}

void treeDownISR() {
  if(treeSpeed != 92){
    treeGoingToBottom = false;
    if(treeSpeed > 90){
      treeDownLowVals = false;
    }else{ 
      treeDownLowVals = true;
    }
    Serial.print("Tree Down on Low Vals = ");
    Serial.println(treeDownLowVals);
    treeBottom = true;
    treeSpeed = 92;
    treeServo.write(92);
    delay(10);
    treeServo.detach();
  }
}
void camISR() {
  camBottom = true;
  Serial.println("BOTTOM!!!!");
  if(camState == 2){
    setCamSpeed(92);
  }
}
//simple oscillation function
 //use millis() to step across a sin wave as an oscillator
 //map function converts to specified output range
int oscillate(int minVal, int maxVal, int offset, int speed)
{    
                            
 int oscillValue = map((sin(float((millis()+offset)*(float(speed)/1000)) * PI / 180.0)*1000),-1000,1000,minVal,maxVal+1);
 return oscillValue;
  
}