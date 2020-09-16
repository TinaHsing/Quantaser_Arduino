//for integrator usage 
#define S1 6
#define S2 5
#define S3 4
#define S4 3
#define INJECTION_CHARGE_TIME 5
#define NEGATIVE_SAMPLING 1

#define TEST 0

long cnt=0;
void setup() {
	//for integrator usage 
	pinMode(S1, OUTPUT);
	pinMode(S2, OUTPUT);
	pinMode(S3, OUTPUT);
	pinMode(S4, OUTPUT);
	digitalWrite(S3, LOW);
	digitalWrite(S4, LOW);
	reset(30);
}

void loop() {
  unsigned long dt;
#if TEST
	Serial.println(dt);
#endif

	while(1)
	{
		/////// integrate process /////
		reset(30);//不得<30, int hold end
		hold(10);
		integrate(dt);
    hold(10);
	}
  
}

void reset(int wait) //10
{
  PORTD = ((PORTD & B10011111) | (1<<S1)); 
  delayMicroseconds(wait);
}

void hold(int wait) //11
{
  PORTD = ((PORTD & B10011111) | (1<<S1) | (1<<S2)); 
  delayMicroseconds(wait);
}

void hold_sample(int wait) //11
{
//  PORTD = ((PORTD & B10001111) | (1<<S1) | (1<<S2) | (1<<S3)); //start sampling ad620 positive input
  PORTD = ((PORTD & B10000111) | (1<<S1) | (1<<S2) | (1<<S4)); //start sampling ad620 negative input
  delayMicroseconds(wait);
  PORTD = (PORTD & B11100111) ; // //stop sampling ad620 negative input
}

void integrate(unsigned long wait) //01
{
  unsigned int bg;
  PORTD = ((PORTD & B10011111) | (1<<S2)); //int start
//  delayMicroseconds(INJECTION_CHARGE_TIME); 
//  PORTD = ((PORTD & B11100111) | (1<<S3) ); //start sampling ad620 positive input
//  delayMicroseconds(NEGATIVE_SAMPLING); 
//  PORTD = (PORTD & B11100111); //stop sampling ad620 positive input
//  delay(wait);
  delayMicroseconds(wait);
}
