#include <avr/interrupt.h>
#include <LocoNet.h>

volatile int servotranscount = 0;

lnMsg        *LnPacket;

ISR(TIM0_OVF_vect) {
	if (servotranscount == 0) { // New cycle
		// Disable the compare interrupts
		TIMSK0 &= ~((1 << OCF0A)| (1 << OCF0A));
	} else if (servotranscount  > 9) { // End of cycle, pulse our servos
		// Set compare interupts
		TIMSK0 |= (1 << OCF0A)| (1 << OCF0A); // Enable just the overflow interrupt
		// Pull servo pins high
		PORTA |= (1<<PA5);
		PORTA |= (1<<PA6);		
	}
	servotranscount++;
	PINA = 0xFF;
}

ISR(TIM0_COMPA_vect) { // First servo
	// Set pin low
	PORTA &= ~(1<<PA5);

	// Reset transcount
	servotranscount = 0;
}

ISR(TIM0_COMPB_vect) { // First servo
	// Set pin low
	PORTA &= ~(1<<PA6);
}

void setupIO(){
	// Configure Servo pins as output
	DDRA |= (1 << PA5);
	DDRA |= (1 << PA6);
	DDRA = 0xFF;
	
	// Pull servo pins low
	PORTA |= (1<<PA5);
	PORTA &= ~(1<<PA6);

}

void setupTimer0() {
	// Basic setup
	// Prescaler to /64 gives us 256 * 64/8e6 * 25 as frequency of the PWM	
	// Set the timer to just count
	TCCR0A = 0; //OCRx outputs disconnected, "normal" mode
	TCNT0 = 0; // Counter to zero
	OCR0A = 200; // = 200 * 64/8e6 = 1.600 ms
	OCR0B = 201; // = 201 * 64/8e6 = 1.608 ms => Difference 0.008/1 * 180 = 1.44 degree of rotation difference...
	TIMSK0 &= ~((1 << OCF0A)| (1 << OCF0A)); // Disable the compare interrupts
	TIMSK0 |= (1 << TOIE0); // Enable just the overflow interrupt
	TCCR0B = ((1<<CS01) | (1<<CS00)); // No forced compares, "normal" mode, prescaler /64
}

int main(){
	setupIO();
	setupTimer0();
	uint8_t pin = PB2;
	uint8_t port = PORTB;
	LocoNet.init(pin, port);
	sei();
	while (1) {
	    LnPacket = LocoNet.receive() ;
	    if( LnPacket ) {
		}
	}
	return 0;
}
