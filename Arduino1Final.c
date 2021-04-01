#define F_CPU (16000000UL)

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>


#define SET_BIT(reg, pin)		    (reg) |= (1 << (pin))
#define CLEAR_BIT(reg, pin)		  (reg) &= ~(1 << (pin))
#define WRITE_BIT(reg, pin, value)   (reg) = (((reg) & ~(1 << (pin))) | ((value) << (pin)))
#define BIT_VALUE(reg, pin)		  (((reg) >> (pin)) & 1)
#define BIT_IS_SET(reg, pin)	     (BIT_VALUE((reg),(pin))==1)

void setup(void);
void process(void);
int button_press (void);
void elapsed_time(void);
void uart_init(unsigned int ubrr);
char uart_getchar(void);
void uart_putchar(unsigned char data);
void uart_putstring(unsigned char* s);
void ftoa(float n, char * res, int afterpoint);
int intToStr(int x, char str[], int d);
void reverse(char * str, int len);

//UART definitions
//define baud rate 
#define BAUD (9600)
#define MYUBRR F_CPU/16/BAUD-1
//timer definitions
#define FREQ (16000000.0)
#define PRESCALE (1024.0)

// State machine for button pressed 
bool pressed = false;
uint16_t counter = 0;
volatile int overflow_counter = 0;
volatile double time;
volatile uint8_t switch_closed = 0;
volatile uint8_t state_count = 0;
bool LED1 = false;
bool LED2 = false;
bool LED3 = false;
bool LED4 = false;
bool LED5 = false;
bool LED6 = false;
bool LED7 = false;
bool LED8 = false;
bool LED9 = false;
bool LED10 = false;
bool LED11 = false;
bool LED12 = false;

void setup(void) {  
  
	// Enable timer overflow, and turn on interrupts.
	sei();
	//initialise uart	
	uart_init(MYUBRR);
	//row 1
	SET_BIT(DDRD, 5);   
 	SET_BIT(DDRD, 4);
    SET_BIT(DDRC, 3);
    //row 2
    SET_BIT(DDRC, 2);
    SET_BIT(DDRB, 0);
 	SET_BIT(DDRB, 1);   
     //row 3
    SET_BIT(DDRB, 3);
    SET_BIT(DDRB, 4);
 	SET_BIT(DDRB, 5);   
	//row 4
    SET_BIT(DDRC, 1);
    SET_BIT(DDRB, 2);
 	SET_BIT(DDRD, 2);
  
   //game on / off LEDF
 	SET_BIT(DDRD, 6);
  //initialise button as input
 	CLEAR_BIT(DDRD,7);
 	//piezo
 	SET_BIT(DDRD, 3);

	// Timer 0 in normal mode, with pre-scaler 1024 ==> ~61Hz overflow.
	TCCR0B = 0;
	TCCR0B = 5; 
	TIMSK0 = 1; //timer overflow on
  
   // initialise adc
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    ADMUX = (1 << REFS0);
	
}
///piezo
void setup_timer2(void){ 
  
	// Timer 2 in normal mode, with pre-scaler 8 ==> ~61Hz overflow.
    // and overflow interrupt
	TCCR2A = 0;
	TCCR2B = 5; 
    TIMSK2 = 1; 
  
  // Enable timer overflow, and turn on interrupts.
	sei();    
}
ISR(TIMER2_OVF_vect) {     	  
}

void start_pwm(void)
{
	TCCR2A |= (1 << COM2A1);
	TCCR2B = (1 << CS21);
	TCCR2A |= (1 << WGM21) | (1 << WGM20);
	OCR2B = 100;
}
void stop_pwm(void)
{
	TCCR2A = 0;
	TCCR2B = 0;
}


bool adc_pet(void)
{  	 
    TCCR0A |= (1 << COM0A1)| (0 << COM0A0);
  	TCCR0A |= (1 << WGM01) | (1 << WGM00);
    // set fast PWM Mode       
          int success1;
          int success2;        
        // Start single conversion by setting ADSC bit in ADCSRA
        ADCSRA |= (1 << ADSC);
   		// Wait for ADSC bit to clear, signalling conversion complete.
        while ( ADCSRA & (1 << ADSC) ) {_delay_ms(50);}
        // Result now available.
        uint16_t pot = ADC;  		
        OCR0A = pot / 4;  //allows a full turn of the potentiometer to go on/off
        if (pot == 1023)
        {
        	success1 = 1;            
        	_delay_ms(1000);        	
        }     
        if (pot < 10)
        	{
        		success2 = 1;          
        	}      	       	
          	
        if (success1 == 1 && success2 == 1)
        {
        	return true;
        }
        else 
        {
        	return false;
        }
 
    	
}

void serial_talk(void)
{  
 		//define a character to send
	    unsigned char sent_char_b = 'L';
	     //send serial data
		uart_putchar(sent_char_b);	   
  
}

void serial_talk_win(void)
{
	   //define a character to send
	    unsigned char sent_char_b = 'W';
	     //send serial data
		uart_putchar(sent_char_b);	
  
}

void game_over (void)
{
  
   	serial_talk();
   	start_pwm();
   	int i;
   	 while(i < 2)
      {
           _delay_ms(50);
                for(int i; i < 60; i++){
                _delay_ms(2);
            //start_pwm();
            SET_BIT(PORTD, 3);
            _delay_ms(1300/(261/2));
            CLEAR_BIT(PORTD, 3);  
            }  
            //_delay_ms(50);
          for(int a; a < 60; a++){
             _delay_ms(2);
            //start_pwm();
            SET_BIT(PORTD, 3);
            _delay_ms(900/(261/2));
            CLEAR_BIT(PORTD, 3); 
          }
            //_delay_ms(50);
           for(int b; b < 60; b++){
            _delay_ms(2);
            //start_pwm();
            SET_BIT(PORTD, 3);
            _delay_ms(700/(261/2));
            CLEAR_BIT(PORTD, 3);
           } 
           for (int c; c < 80; c++){
            _delay_ms(2);
            //start_pwm();
            SET_BIT(PORTD, 3);
            _delay_ms(1500/(261/2));
            CLEAR_BIT(PORTD, 3);  ;
           }
          i++;
      }
      stop_pwm();

  	for (int i = 0; i < 3; i++)
	{
      
	 PORTD = 0b00110100;  
 	PORTB = 0b00111111;
 	PORTC = 0b000001110;
 	_delay_ms(200); 
	PORTD = 0b00000000;
 	PORTB = 0b00000000;
 	PORTC = 0b00000000;
 	_delay_ms(200); 
 }

 _delay_ms(1000);
 first_row();
}

void game_win(void)
{
	_delay_ms(50);
  	serial_talk_win();
  	int i;
	      while(i < 2)
	      {
	           
                for(int i; i < 60; i++){
                _delay_ms(2);
	            //start_pwm();
	            SET_BIT(PORTD, 3);
	            _delay_ms(200/(261/2));
	            CLEAR_BIT(PORTD, 3);  
	            }  
	            //_delay_ms(100);
	          for(int a; a < 60; a++){
	             _delay_ms(2);
	            //start_pwm();
	            SET_BIT(PORTD, 3);
	            _delay_ms(200/(261/2));
	            CLEAR_BIT(PORTD, 3); 
	          }
	            //_delay_ms(100);
	           for(int b; b < 60; b++){
	            _delay_ms(2);
	            //start_pwm();
	            SET_BIT(PORTD, 3);
	            _delay_ms(400/(261/2));
	            CLEAR_BIT(PORTD, 3);
	           } 
	          for(int z; z < 60; z++){
	             _delay_ms(2);
	            //start_pwm();
	            SET_BIT(PORTD, 3);
	            _delay_ms(200/(261/2));
	            CLEAR_BIT(PORTD, 3); 
	          }
	          i++;
	      }
	      stop_pwm();
    PORTD = 0b00110100;  
 	PORTB = 0b00111111;
 	PORTC = 0b00001110;
 	_delay_ms(1000); 
 	PORTD = 0b00000000;
 	PORTB = 0b00000000;
 	PORTC = 0b00000000; 
 	_delay_ms(1000);
 	first_row();

}

void first_row(void)
{
		int flag;
	
	  //loop through first row of LEDs, 
	     //checking to see if button has been clicked after
	     //each LED has been turned on. 

	while(true)
	{ 
		CLEAR_BIT(PORTC, 3);
		 elapsed_time();
		 PORTD=0b00100000;
		 _delay_ms(500);  
		 //if the button has been pressed, set the LED that was on at the time
		 //to ON
	      if (switch_closed == 1) 
	      {
	      	LED1 = true;
	      	flag = 1;
	     	break;	      		      	  
	      }    
		elapsed_time();
		 PORTD=0b00010000;
	       _delay_ms(500);
	      if (switch_closed == 1)
	      {
	      	LED2 = true;
	      	flag = 2;
  			break;	
	      	
	      }	 
	      CLEAR_BIT(PORTD, 4);
 		 elapsed_time();
	       SET_BIT(PORTC, 3);
	      _delay_ms(500); 
	       if (switch_closed == 1)
	      {
	      	LED3 = true;
	      	flag = 3;
  			break;		      
	      }     		 
	}

	if (flag == 1)
	{
		SET_BIT(PORTD, 5);
      second_row();
	}
	else if (flag == 2)
	{
		SET_BIT(PORTD, 4);
       second_row();
	}
	else if (flag == 3)
	{
		SET_BIT(PORTD, 3);
       second_row();
	}	
  	else;	
		 
}

void second_row(void)
{
  pressed = false;
  int flags;
	  //loop through second row of LEDs, 
	     //checking to see if button has been clicked after
	     //each LED has been turned on. 
  while(true)
	{
		elapsed_time();
  		CLEAR_BIT(PORTB, 1);
		SET_BIT(PORTC, 2);
		 _delay_ms(350);  
		 //if the button has been pressed, set the LED that was on at the time
		 //to ON
	      if (switch_closed == 1) 
	      {
	      	
	      flags = 1;
	     break;
	      }   
  
		elapsed_time();
  		CLEAR_BIT(PORTC,2 );
		 SET_BIT(PORTB, 0);
	       _delay_ms(350);
	      if (switch_closed == 1)
	      {
	      	
	      	 flags = 2;
	      	  break;
	      }	 
  
 		 elapsed_time();
 		 CLEAR_BIT(PORTB, 0);
	       SET_BIT(PORTB, 1);
	      _delay_ms(350); 
	       if (switch_closed == 1)
	      {
	      	
	      	flags = 3;
	      	  break;
	      }    
  		
	 }
 	if (flags == 1)
	{
		if(LED1 == true)
		{
			LED1 = false;
			LED2 = false;
			LED3 = false;
			LED4 = true;          
			SET_BIT(PORTC, 2);
      		third_row();
		}
		else
		{
          	_delay_ms(350);
			 game_over();
		} 
		
	}
	else if (flags == 2)
	{
		if(LED2 == true)
		{
			LED1 = false;
			LED2 = false;
			LED3 = false;
			LED5 = true;
			SET_BIT(PORTB, 0);    
  		  	third_row();
  		  }
  		  else
		{
          	_delay_ms(350);
			 game_over();
		} 
	}
	else if (flags == 3)
	{
		if (LED3 == true)
		{
			LED1 = false;
			LED2 = false;
			LED3 = false;
			LED6 = true;
			SET_BIT(PORTB, 1);    
  		  	third_row();
		}
		else
		{
          	_delay_ms(350);
			 game_over();
		} 		
	}	
  	else;
}

void third_row(void)
{
  pressed = false;
  int flags3;
	  //loop through second row of LEDs, 
	     //checking to see if button has been clicked after
	     //each LED has been turned on. 
  while(true)
	{
		elapsed_time();
  		CLEAR_BIT(PORTB, 5);
		SET_BIT(PORTB, 3);
		 _delay_ms(250);  
		 //if the button has been pressed, set the LED that was on at the time
		 //to ON
	      if (switch_closed == 1) 
	      {
	      	
	      flags3 = 1;
	      break;
	      }   
  
		elapsed_time();
  		CLEAR_BIT(PORTB, 3);
		 SET_BIT(PORTB, 4);
	       _delay_ms(250);
	      if (switch_closed == 1)
	      {
	      	
	      	 flags3 = 2;
	      	   break;
	      }	 
  
 		 elapsed_time();
 		 CLEAR_BIT(PORTB, 4);
	       SET_BIT(PORTB, 5);
	      _delay_ms(250); 
	       if (switch_closed== 1)
	      {
	      	
	      	flags3 = 3;
	      	  break;
	      }    
  		 
	 }

 	if (flags3 == 1)
	{
		if(LED4 == true)
		{
			LED4 = false;
			LED5 = false;
			LED6 = false;
			LED7 = true;
			SET_BIT(PORTB, 3);
      		forth_row();
		}
		else
		{
			_delay_ms(350);
			 game_over();
		} 
		
	}
	else if (flags3 == 2)
	{
		if(LED5 == true)
		{
			LED4 = false;
			LED5 = false;
			LED6 = false;
			LED8 = true;
			SET_BIT(PORTB, 4);    
  		  	forth_row();
		}
		else
		{
			_delay_ms(350);
			 game_over();
		} 
		
	}
	else if (flags3 == 3)
	{
		if(LED6 == true)
		{
			LED4 = false;
			LED5 = false;
			LED6 = false;
			LED9 = true;
			SET_BIT(PORTB, 5);    
  		  	forth_row();
		}
		else
		{
			_delay_ms(350);
			 game_over();
		} 
	
	}	
  	else;
}

void forth_row(void)
{
  pressed = false;
  int flags4;
	  //loop through second row of LEDs, 
	     //checking to see if button has been clicked after
	     //each LED has been turned on. 
  while(true)
	{		
  
		elapsed_time();  
  		CLEAR_BIT(PORTD, 2);
		 SET_BIT(PORTC, 1);
	       _delay_ms(150);
	       if (switch_closed == 1)
	      {
	      	
	      	flags4 = 1;  
	      	  break;
	      }
  
 		  elapsed_time();  
  		CLEAR_BIT(PORTC, 1);
		 SET_BIT(PORTB, 2);
	       _delay_ms(150);
	       if (switch_closed== 1)
	      {
	      	
	      	flags4 = 2;  
	      	  break;
	      }  
  
  		 elapsed_time(); 
         CLEAR_BIT(PORTB, 2);
         SET_BIT(PORTD, 2);
		
	       _delay_ms(150);
	       if (switch_closed== 1)
	      {
	      	
	      	flags4 = 3;
  			break;		
	      		      
	      }
	 }


 	if (flags4 == 1)
	{
		if (LED7 == true)
		{
			LED7 = false;
			LED8 = false;
			LED9 = false;
			LED10 = true;
			SET_BIT(PORTC, 1);
			_delay_ms(1000); 
			game_win();
		}
		else
		{
			_delay_ms(500); 
		  	game_over();
		}
      	
	}
	else if (flags4 == 2)
	{
		if (LED8 == true)
		{
			LED7 = false;
			LED8 = false;
			LED9 = false;
			LED11 = true;
			SET_BIT(PORTB, 2); 
			_delay_ms(1000);    
			game_win();
		}
		else
		{
			_delay_ms(500); 
		  	game_over();
		}	
  		  
	}
	else if (flags4 == 3)
	{
		if (LED9 == true)
		{
			LED7 = false;
			LED8 = false;
			LED9 = false;
			LED12 = true;
			SET_BIT(PORTD, 3);    
			_delay_ms(1000); 
			game_win();
		}
		else
		{
			_delay_ms(500); 
		  	game_over();
		}		
	}	
  	else;
}

ISR(TIMER0_OVF_vect) {
  uint32_t mask = 0b0000011;
  state_count = ((state_count<<1) & mask) | BIT_IS_SET(PIND, 7);
  if(state_count == mask) {
    switch_closed = 1;
  }
  else if (state_count == 0) {
    switch_closed = 0;
  }
overflow_counter++;
  
}

//used to send the elapsed run time to serial output
void elapsed_time (void)
{
		char temp_buf[64];
	    char *ch;

	    //compute elapsed time 
   		time = ( overflow_counter * 256.0 + TCNT0 ) * PRESCALE  / FREQ;
  
	    //convert float to a string 
	    ftoa(time, temp_buf, 4);

		 //send serial data
		 uart_putstring((unsigned char *) temp_buf);
		 uart_putchar('\n');  

}
int main(void) {
	setup(); 
	 _delay_ms(100);
   while (adc_pet() != true)
   {
       _delay_ms(100);
     adc_pet();     
   }   
  _delay_ms(50);
  first_row();	

}

//**************

//All code below has been sourced from the CAB202 Learning materials lecture notes
// Reverses a string 'str' of length 'len' 
void reverse(char * str, int len) {
  int i = 0, j = len - 1, temp;
  while (i < j) {
    temp = str[i];
    str[i] = str[j];
    str[j] = temp;
    i++;
    j--;
  }
}

// Converts a given integer x to string str[].  
// d is the number of digits required in the output.  
// If d is more than the number of digits in x,  
// then 0s are added at the beginning. 
int intToStr(int x, char str[], int d) {
  int i = 0;
  while (x) {
    str[i++] = (x % 10) + '0';
    x = x / 10;
  }

  // If number of digits required is more, then 
  // add 0s at the beginning 
  while (i < d)
    str[i++] = '0';

  reverse(str, i);
  str[i] = '\0';
  return i;
}

// Converts a floating-point/double number to a string. 
void ftoa(float n, char * res, int afterpoint) {
  // Extract integer part 
  int ipart = (int) n;

  // Extract floating part 
  float fpart = n - (float) ipart;

  // convert integer part to string 
  int i = intToStr(ipart, res, 0);

  // check for display option after point 
  if (afterpoint != 0) {
    res[i] = '.'; // add dot 

    // Get the value of fraction part upto given no. 
    // of points after dot. The third parameter  
    // is needed to handle cases like 233.007 
    fpart = fpart * pow(10, afterpoint);

    intToStr((int) fpart, res + i + 1, afterpoint);
  }
}
/********* serial definitions ****************/

// Initialize the UART
void uart_init(unsigned int ubrr) {
	
	UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)(ubrr);
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (3 << UCSZ00);
	
}

// Transmit a data
void uart_putchar(unsigned char data) {
	
    while (!( UCSR0A & (1<<UDRE0))); /* Wait for empty transmit buffer*/
    
  	UDR0 = data;            /* Put data into buffer, sends the data */
         	
}

// Receive data
char uart_getchar(void) {
  /* Wait for data to be received */ 
  while ( !(UCSR0A & (1<<RXC0)) );    	
/* Get and return received data from buffer */
return UDR0;	
}
// Transmit a string
void uart_putstring(unsigned char* s)
{
    // transmit character until NULL is reached
    while(*s > 0) uart_putchar(*s++);
}


	
