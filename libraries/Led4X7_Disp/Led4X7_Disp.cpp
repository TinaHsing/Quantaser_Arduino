# include <Led4X7_Disp.h>

const bool led4X7_disp::p_data[10][4]= {
							{0,0,0,0}, //0
							{1,0,0,0}, //1
						    {0,1,0,0}, //2
						    {1,1,0,0}, //3
						    {0,0,1,0}, //4
						    {1,0,1,0}, //5
						    {0,1,1,0}, //6
						    {1,1,1,0}, //7
						    {0,0,0,1}, //8								    
							{1,0,0,1}, //9
						    };

led4X7_disp::led4X7_disp(uint8_t pa, uint8_t pb, uint8_t pc, uint8_t pd, uint8_t p0, uint8_t p1, uint8_t p2, uint8_t p3, uint8_t ph)
{
		
	p_BCD_pins[0] = pa;
	p_BCD_pins[1] = pb;
	p_BCD_pins[2] = pc;
	p_BCD_pins[3] = pd;
	p_pos_pins[0] = p0-8;
	p_pos_pins[1] = p1-8;
	p_pos_pins[2] = p2-8;
	p_pos_pins[3] = p3-8;
	p_pinh = ph-8;
	pinMode(pa, OUTPUT);
	pinMode(pb, OUTPUT);
	pinMode(pc, OUTPUT);
	pinMode(pd, OUTPUT);
	pinMode(p0, OUTPUT);
	pinMode(p1, OUTPUT);
	pinMode(p2, OUTPUT);
	pinMode(p3, OUTPUT);
	pinMode(ph, OUTPUT);
		}

void led4X7_disp::init(uint8_t adc_bit, uint8_t gain)
{
	switch (adc_bit) {
		case 10 :
			p_adcbase = 1023;
			break;
		case 12 :
			p_adcbase = 4095;
			break;
		case 16 :
			p_adcbase = 65535;
		break;
	}
	p_gain = gain;
	get_mask();
}
void led4X7_disp::print(unsigned int code) {
	float number;
	uint8_t n[4], case_select, dt=5;
	
	number = float(code) /p_adcbase*5*p_gain;
	if (number >= 100) case_select = 3;
	else if (number >= 10) case_select = 2;
	else case_select = 1;
	
	switch (case_select) {
    case 3 :
      n[3] = int(number)/100;
      number -= n[3]*100;
      SetDisplay(n[3], 3);
      delay(dt);

    case 2 :
      n[2] = int(number)/10;
      number -= n[2]*10;
      SetDisplay(n[2], 2);
      delay(dt);

    case 1 :
      n[1] = int(number);
      number -= n[1];  
      SetDisplay(n[1], 1);
      delay(dt);
      n[0] = number*10;
      SetDisplay(n[0], 0);
      delay(dt);
    break;
  }
	
}

void led4X7_disp::get_mask() {
	p_BCD_mask = 255 & (~(1<<p_BCD_pins[0]) & ~(1<<p_BCD_pins[1]) & ~(1<<p_BCD_pins[2]) & ~(1<<p_BCD_pins[3]));
	p_PORTB_mask = 255 & (~(1<< p_pos_pins[0]) & ~(1<< p_pos_pins[1]) & ~(1<< p_pos_pins[2]) & ~(1<< p_pos_pins[3]) & ~(1<< p_pinh)); 
}

void led4X7_disp::SetDisplay(uint8_t number, uint8_t position) {
	PORTD = (PORTD & p_BCD_mask) | (p_data[number][0] << p_BCD_pins[0]) | (p_data[number][1] << p_BCD_pins[1])
	 | (p_data[number][2] << p_BCD_pins[2]) | (p_data[number][3] << p_BCD_pins[3]);
	PORTB = (PORTB & p_PORTB_mask ) | (1 << p_pos_pins[position]) | (position !=1? (1<<p_pinh) : 0 );   

}

