#include <Servo.h>
#include <Ultrasonic.h>

Servo myservo;
int photoPin = A0;
int offset = 1;
int minDistance = 30;
  //Ultrasonic ultrasonic(12,13);

void setup(){
	Serial.begin(9600);
	myservo.attach(11);
  pinMode(1,INPUT);

}
void loop() {

	/* Serial.print( ultrasonic.Ranging(CM) ); */
	/* Serial.println( "cm" ); */
    
	/* delay(1000); */


/*	myservo.write(0);
	int minValue = 5000;
	int angle = 0;
	for(int i = 0;i<180;i++){
		myservo.write(i);
                delay(10);
                   int value =  analogRead(1);
		//int value =  ultrasonic.Ranging(CM);
		if(value<minValue){
			minValue=value;
			angle = i;
		}

	}
	if(minValue<minDistance){
            follow(angle);  
	    Serial.println("following");
	  

	}*/
  follow(21,minDistance,1,0,0); 
  //followForward(0,minDistance); 
	
}

/*void follow(int angle){
  //myservo.write(angle);
  int minValue = 5000;
  int bestAngle = 0;

  for(int i = 0;i<8*offset;i+=4){
    if(angle-8*offset < 0) angle = offset; 
    if(angle+8*offset > 0) angle = 180-offset;
    myservo.write(angle-8*offset+i);
   // Serial.print(angle-offset+i);
        delay(60);
   
    int value =  analogRead(1);
    //Serial.print(value);
    //Serial.print(",");
    //ultrasonic.Ranging(CM);
      Serial.println(value);
		if(value<minValue){
			minValue=value;
			bestAngle = angle-8*offset+i;
		}

  }
  Serial.println();
//delay(10000);

  if( minValue < minDistance){
    follow(bestAngle);
  }

}

void followForward(int i,int objectDistance){
  int skip = 0; 
  for(;i<120;i++){
        delay(5);

    myservo.write(i); 
    int value = analogRead(1);
    Serial.println(objectDistance);
      if(value>objectDistance){
        if(skip>offset){
      followBackward(i,objectDistance);
        }
        skip = 0; 
    }else{
      objectDistance=value;
      skip++;
      }
  }
  followBackward(i,minDistance); 
}

void followBackward(int i,int objectDistance){
  int skip = 0; 
  for(;i>20;i--){
        delay(5);
    myservo.write(i); 
    int value = analogRead(1);
    if(value > objectDistance){
      if(skip>offset){
      followForward(i,objectDistance);
      }
      skip = 0;
    }else{
      objectDistance=value;
      skip++;
      }

    
  }
  followForward(i,minDistance); 
}
/*Takes the angle to begin searching at, the minimum distance to be considered an object,
the direction to move the sensor in (-1 or 1), an int 'verifyFound' to describe whether the 
function is currently tracking an object and account for sensor error in finding an object; 
and verifyEdge, an integer used to count how many times the sensor has seen an edge in a row*/
void follow(int angle, int distance, int dir,int verifyFound,int verifyEdge){
  delay(12);
   
 //if(!found) distance = minDistance; 
 
 angle = angle + dir;

 myservo.write(angle);
 
 //If the servo hits the edge of the viewing angle, it's likely lost the object. Start over looking in the opposite directon.
 if(angle > 160){ 
   follow(159,minDistance,-dir,0,verifyEdge); 
 }
 if(angle < 10){
   follow(11,minDistance,-dir,0,verifyEdge);
 }
 
 //grab sensor data
 int newDistance = analogRead(1);
 
 //When the current proximity is closer than the given proximity, we've found an object.
 //Set verifyFound up by one. After a few recursive calls, if this is still true, we will
 //begin tracking the object. 
 if(newDistance <= distance) follow(angle,newDistance,dir,++verifyFound,0);
  
 //If we've found an object, follow it until we hit an edge, then reverse direction
 if(verifyFound > 3){ 
    Serial.print(angle);
 Serial.print(",");
 Serial.print(distance);
 Serial.println();
   /* veriftFound gets set to 0 when we find an edge. This is because we need to find
   the object again when sweeping in the reverse direction, in case it has moved. verifyEdge will
   increase every time there's a disparity between the previous distance and the newDistance. 
   After verifyEdge is greater than a constant, we can be sure we've hit an edge and not a sensor glitch */ 
   if(newDistance >  1.3*distance || newDistance < .7*distance) verifyEdge++; 
   else{
     if (verifyEdge>0) verifyEdge--; //start removing verifyEdge points if there aren't disparities. 
   }
   
   //We're sure there's an edge, reverse sensor direction and look for the object again
   if(verifyEdge>=2){
     follow(angle,distance,-dir,0,0);
   }
   
   //Keep following the object
   //if(newDistance <= distance) follow(angle,newDistance,dir,true,0); 
   follow(angle,distance,dir,verifyFound,verifyEdge);
 }
 
 //catchall. If The object isn't found, just keep scanning
 follow(angle,distance,dir,0,0);

}
