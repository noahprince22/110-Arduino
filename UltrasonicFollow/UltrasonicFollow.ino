#include <Servo.h>
#include <Ultrasonic.h>

Servo myservo;
//int photoPin = A0;
int offset = 1;
int minDistance = 50;
  //Ultrasonic ultrasonic(12,13);

void setup(){
	Serial.begin(9600);
	myservo.attach(11);
	pinMode(A1,INPUT);
}
void loop() {
  int distance = 0;
  for(int i = 15;i<160;i++){
    myservo.write(i); 
    int newDistance = 0;
     for(int i = 0; i<10;i++){
	 newDistance += analogRead(A1);
          delay(1);
     }
      distance+= newDistance = (int) newDistance/10;
  }
  minDistance = (int) (distance/145)/2;
  Serial.print("MINDISTANCE =");
  Serial.println(minDistance);
	//Serial.print(ultrasonic.Ranging(CM));
  follow(21,minDistance,3,0,0,2); 
}

/*GENERAL IDEA */
/*The sensor begins sweeping in one direction, and will turn around to the other direction
when it hits the servo's maximum angle or minimum angle. If the sensor catches an object less
than the minimum distance away at 3 consecutive servo angles, it enters an object-found state.
When in the object found state, the sensor looks for an edge to the object. If the sensor picks
up a value greater than 1.3 times the object's original distance at 3 consecutive angles, it has
found an edge. Once an edge is found, the sensor sweep reverses direction and begins looking for
the object again. */

/*Takes the angle to begin searching at, the minimum distance to be considered an object,
the direction to move the sensor in (-1 or 1), an int 'verifyFound' to describe whether the 
function is currently tracking an object and account for sensor error in finding an object; 
and verifyEdge, an integer used to count how many times the sensor has seen an edge in a row*/
void follow(int angle, int distance, int dir,int verifyFound,int verifyEdge,int delayValue){
 boolean direction = dir > 0; 
   
 angle = angle + dir;
 myservo.write(angle);
 int newDistance = 0;
 for(int i = 0; i<14;i++){
	 newDistance += analogRead(A1);
          delay(delayValue);
 }
 newDistance = (int) newDistance/14;

 //If the servo hits the edge of the viewing angle, it's likely lost the object. Start over looking in the opposite directon.
 if(angle > 160){ 
   follow(159,minDistance,-dir,0,0,delayValue); 
 }
 if(angle < 15){
   follow(16,minDistance,-dir,0,0,delayValue);
 }
 
 //grab sensor data

 //int newDistance = ultrasonic.Ranging(CM);
  
 //If we've found an object, follow it until we hit an edge, then reverse direction
 if(verifyFound >= 3){   
    	 Serial.print(angle);
	 Serial.print(",");
	 Serial.print(newDistance);
	 Serial.print("; ");
         if(direction) dir = 2;
         else dir = -2; 
         delayValue = 2;

	 /* verifyFound gets set to 0 when we find an edge. This is because we need to find
	    the object again when sweeping in the reverse direction, in case it has moved. verifyEdge will
	    increase every time there's a disparity between the previous distance and the newDistance. 
	    After verifyEdge is greater than a constant, we can be sure we've hit an edge and not a sensor glitch */ 
	 if(newDistance >  distance+10) {
                Serial.print("    EDGE   (");
                Serial.print(newDistance);
                Serial.print(","); 
                Serial.print(1.3*distance);
                Serial.print(")           ");   
                verifyEdge++; 
         }
	 else{
		 if (verifyEdge>0) verifyEdge--; //start removing verifyEdge points if there aren't disparities. 
                 distance = newDistance+10;
	 }
   
	 //We're sure there's an edge, reverse sensor direction and look for the object again
	 if(verifyEdge>=3){
                   Serial.println();
		 follow(angle,distance+15,-dir,--verifyFound,0,delayValue);
	 }
   
	 //Keep following the object
	 //if(newDistance <= distance) follow(angle,newDistance,dir,true,0); 
	 follow(angle,distance,dir,verifyFound,verifyEdge,delayValue);
 }
 if(verifyFound==0){
         if(direction) dir = 4;
         else dir = -4;
         delayValue = 2; 
 } 
 
  //When the current proximity is closer than the given distance threshold, we've found an object.
 //Set verifyFound up by one. After a few recursive calls, if this is true for consecutive angles, 
 //we will begin tracking the object. 
 if(newDistance > distance && verifyFound > 0) verifyFound--;
 else if(newDistance <= distance) verifyFound++; 
 else if(verifyFound >= 3) distance = newDistance+10; 
 
 //catchall. If The object isn't found, just keep scanning
 follow(angle,distance,dir,verifyFound,0,delayValue);

}
