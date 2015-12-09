#include <avr/interrupt.h>

volatile int servotranscount = 0;

ISR(TIM0_OVF_vect, ISR_NAKED) {
	if (PCMSK0 == 0) { // New cycle
		// Disable the compare interrupts
		TIMSK0 &= ~((1 << OCF0A)| (1 << OCF0B));
	} else if (PCMSK0  > 9) { // End of cycle, pulse our servos
		TCNT0 = 0;
		// Pull servo pins high
		PORTA |= (1<<PA5);
		PORTA |= (1<<PA6);		
		// Set compare interupts
		TIFR0 |= (1 << OCF0A) | (1 << OCF1B);
		TIMSK0 |= (1 << OCF0A)| (1 << OCF0B); // Enable just the overflow interrupt
	}
	PCMSK0++;
}

ISR(TIM0_COMPA_vect, ISR_NAKED) { // First servo
	// Set pin low
	PORTA &= ~(1<<PA5);

	// Reset transcount
	PCMSK0 = 0;
}

ISR(TIM0_COMPB_vect, ISR_NAKED) { // First servo
	// Set pin low
	PORTA &= ~(1<<PA6);
}

void setupIO(){
	// Configure Servo pins as output
	DDRA |= (1 << PA5);
	DDRA |= (1 << PA6);
	
	// Pull servo pins low
	PORTA &= ~(1<<PA5);
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
	TIMSK0 &= ~((1 << OCF0A)| (1 << OCF0B)); // Disable the compare interrupts
	TIMSK0 |= (1 << TOIE0); // Enable just the overflow interrupt
	TCCR0B = ((1<<CS01) | (1<<CS00)); // No forced compares, "normal" mode, prescaler /64
}

int main(){
	setupIO();
	setupTimer0();
	sei();
	while (1) {
		
	}
	return 0;
}