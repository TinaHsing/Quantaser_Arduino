void scanVar(String, unsigned long &);
void scanVar(String, float &);

void setup() {
	Serial.begin(9600);
	
}

void loop() {
	float v_init, v_step;
	unsigned long dt, pts;
  unsigned int v_dac, v0, dv;
  bool stop_flag = 0;
  int i=0;
	scanVar("initial Voatage(V): ", v_init);
  scanVar("scan step(V): ", v_step);
	scanVar("scan points: ", pts);
  scanVar("integrate time(us): ", dt);
  v0 = (v_init/5.0)*65535;
  dv = (v_step/5.0)*65535;
  Serial.println(v_init);
  Serial.println(v_step);
  Serial.println(pts);
  Serial.println(dt);
  while(1)
  {
//    dacout(v0);
//    delayMicroseconds(dt);
//    adc();
//    uartprint;
//    v0 += dv;
    
    Serial.println(i);
    delay(500);
    i++;
    checkStop(stop_flag);
    if(stop_flag) break;
  }
  
}
void checkStop(bool &stop_flag)
{
  char r;
  while(Serial.available() > 0)
  {
    stop_flag = Serial.parseInt();
    r=Serial.read();
  }  
}

void scanVar(String s, unsigned long &var)
{
	char r;
	Serial.print(s);
	while(Serial.available() == 0);
	var = Serial.parseInt();
	r=Serial.read(); //read "\n"
	Serial.println(var);
}

void scanVar(String s, float &var)
{
	char r;
	Serial.print(s);
	while(Serial.available() == 0);
	var = Serial.parseFloat();
	r=Serial.read(); //read "\n"
	Serial.println(var);
}
