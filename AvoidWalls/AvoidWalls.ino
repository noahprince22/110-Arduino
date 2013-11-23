#include <Ultrasonic.h>

int leftMotor = 9;
int rightMotor = 10;
Ultrasonic ultrasonic(12,13);

void setup(){
y  Serial.begin(9600);
  pinMode(leftMotor,OUTPUT); //set the left motor to output
  pinMode(rightMotor,OUTPUT);
}

void loop(){

    Serial.print(ultrasonic.Ranging(CM));
    if(ultrasonic.Ranging(CM) >= 20){
	    analogWrite(leftMotor,255);
	    analogWrite(rightMotor,255);
    }else{
	    analogWrite(leftMotor,0);
	    analogWrite(rightMotor,255);	    
    }
    delay(30);

}

void setLeft(int percent, int direction){
	double percentModifier = (percent/100);
	analogWrite(leftMotor,(int) percentModifier*255);
}

void serRight(int percent, int direction){
	double percentModifier = (percent/100);
	analogWrite(rightMotor,(int) percentModifier*255);
}
