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

	//Do a sweep and take the average sensor value over that sweep, store it in distance.
	int distance = 0;
	for(int i = 15;i<160;i++){
		myservo.write(i); 
		int newDistance = 0;
		for(int i = 0; i<7;i++){
			newDistance += analogRead(A1);
			delay(1);
		}
		distance+= newDistance = (int) newDistance/7;
	}

	/*the starting distance the sensor will look for objects is the average distance in the room
	divided by a constant arbitrary number. This just gives some benchmark for what distances
	we're expected to see.*/
	minDistance = (int) (distance/145)/2.5;
	Serial.print("MINDISTANCE =");
	Serial.println(minDistance);

	//Start object tracking
	startTracking(minDistance); 
}

void startTracking(int minDistance){
	follow(16,minDistance,3,0,0,400);
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
and verifyEdge, an integer used to count how many times the sensor has seen an edge in a row,
and delayValue, a value to set the delay time between each tick, which is inversely proportional
to the time the sensor spends not tracking an object*/
void follow(int angle, int distance, int dir,int verifyFound,int verifyEdge,int delayValue){
 boolean direction = dir > 0; 
   
 angle = angle + dir;
 myservo.write(angle);
 
 //Take the average sensor value over 14*delay value ms. Store it as the new distance. 
 int newDistance = 0;
 for(int i = 0; i<14;i++){
	 newDistance += analogRead(A1);
          delayMicroseconds(delayValue*10);
 }
 newDistance = (int) newDistance/14;

 //If the servo hits the edge of the viewing angle, it's likely lost the object. Start over looking in the opposite directon.
 if(angle > 170){ 
   follow(169,minDistance,-dir,0,0,delayValue); 
 }
 if(angle < 5){
   follow(6,minDistance,-dir,0,0,delayValue);
 }
 
 //If we've found an object, follow it until we hit an edge, then reverse direction
 if(verifyFound >= 3){   
	 //print some lovely information
    	 Serial.print(angle);
	 Serial.print(",");
	 Serial.print(newDistance);
	 Serial.print("; ");
	 
	 //set the magnitude of the ticks of the servo and the delay between ticks(arbitrary)
         if(direction) dir = 2;
         else dir = -2; 
         delayValue = 150;

	 /* verifyFound gets set to 0 when we find an edge. This is because we need to find
	    the object again when sweeping in the reverse direction, in case it has moved. verifyEdge will
	    increase every time there's a disparity between the previous distance and the newDistance. 
	    After verifyEdge is greater than a constant, we can be sure we've hit an edge and not a sensor glitch */ 
	 if(newDistance >  distance+5 ) {
		 //notify Serial that we found an edge
                Serial.print("    EDGE   (");
                Serial.print(newDistance);
                Serial.print(","); 
                Serial.print(1.3*distance);
                Serial.print(")           ");   

		//increment verify edge, make the servo tick on smaller intervals for precision edge finding.
                verifyEdge++; 
                if(direction) dir = 1;
                 else dir = -1; 
                delayValue=150; 
                newDistance = distance; 
         }
	 else{
		 if (verifyEdge>0) verifyEdge--; //start removing verifyEdge points if there aren't disparities. 
                 distance = newDistance;

		 //Start decreasing the delayValue, the sensor will pick up speed as it tracks a longer object
                 if(delayValue > 50) delayValue-=9; 
	 }
   
	 //We're sure there's an edge, reverse sensor direction and look for the object again
	 if(verifyEdge>=3){
                   Serial.println();
		 follow(angle+dir,distance,-dir,--verifyFound,0,400);
	 }
   
	 //Keep following the objectg
	 //if(newDistance <= distance) follow(angle,newDistance,dir,true,0); 
	 follow(angle,newDistance,dir,verifyFound,verifyEdge,delayValue);
 }

 //A bunch of technical stuff that happens while it's looking for an object to accelerate the servo
 if(verifyFound<3){
         if(direction && dir<=4) dir++; 
         else if (!direction && dir >= -4) dir--;
         if(delayValue> 150) delayValue-=9; 
 }else{
   if(dir !=0){
     if(direction) dir--;
     else dir++;
   }
 }
 
 //When the current proximity is closer than the given distance threshold, we've found an object.
 //Set verifyFound up by one. After a few recursive calls, if this is true for consecutive angles, 
 //we will begin tracking the object. 
 if(newDistance > distance+15 && verifyFound > 0) verifyFound--;
 else if(newDistance <= distance + 5) verifyFound++; 
 else if(verifyFound >= 3) distance = newDistance+10; 
 
 //catchall. If The object isn't found, just keep scanning
 follow(angle,distance,dir,verifyFound,0,delayValue);
}
