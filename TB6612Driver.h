// Driver for TB6612FNG motor driver board
// TB6612 Function Controls
//
// When STBY is low, motor is off (high impedance)
//	STBY high for normal operation
//
// These are the I/O as labeled on the TB6612 board:
//
//         Right Motor	  Left Motor
//----------------------------------------
//Inputs   AIN1, AIN2     BIN1, BIN2
//Outputs  AO1,  AO2      BO1,  BO2
//
// Truth Table below is how the motor functions when the motor is wired with
// positive polarity.  IF the motor is wired with negative polarity, the directions
// will be reversed.  The Personal Robot design has the left motor wired with 
// negative polarity and the right motor positive polarity.
//
//   IN1    IN2	   PWM        Motor Behavior
//----------------------------------------
//   Low    High   Low        Brake
//   Low    High   Pulsing    Running CCW
//
//   High   Low	   Low	      Brake
//   High   Low	   Pulsing    Running CW
//	
//   High   High   D/C        Brake


void SetDriveMotorsTB6612(int leftMotorPwr, int rightMotorPwr)
  {
    // MotorPwr will be -255 to +255; 0 represents stopped.  
    // Note: AnalogWrite requires input parameter to be between
    // 0 and 255, inclusive.  Thus, absolute value function must
    // be used.
				
    if( 
        ((leftMotorPwr > 255) || (leftMotorPwr < -255)) ||
        ((rightMotorPwr > 255) || (rightMotorPwr < -255))
        )
          return;  // bail out when data is out of bounds
 
    // determine direction for each motor     
    if(leftMotorPwr < 0)  		// Go'in Backwards
        {
        digitalWrite(AIn1Pin, LOW);	
        digitalWrite(AIn2Pin, HIGH);
	leftMotorPwr = abs(leftMotorPwr);  // change to > 0
        }
    else				//  forwards
        {
        digitalWrite(AIn1Pin, HIGH);			
        digitalWrite(AIn2Pin, LOW);
        }
      
     if(rightMotorPwr < 0)  		// backwards
        {
        digitalWrite(BIn1Pin, LOW);	
        digitalWrite(BIn2Pin, HIGH);
	rightMotorPwr = abs(rightMotorPwr);  // change to > 0
        }
    else				//  forwards
        {
        digitalWrite(BIn1Pin, HIGH);	
        digitalWrite(BIn2Pin, LOW);
        }     
      
    // Set the PWM outputs for the two motors
    // a value of zero will shut off the motor
    analogWrite(APWMPin, leftMotorPwr);  
    analogWrite(BPWMPin, rightMotorPwr); 
    return;		
  }
