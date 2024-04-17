#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>
# define INDEX_DISTANCE 5

const int MIN_ANGLE = 3;
const int MAX_ANGLE = 41;

const int MAX_MOTOR = 255;
const int MIN_MOTOR = 200;

void init();
void initLEDs();
void initButtons();
void initServoControl();
void initDistanceMeasurement();
void initMotorCotntrol();
void blinkResetLed();
void saveAngle();
void sendPulse();
unsigned long measurePulse();
unsigned long measureDistance();
void motorStop();
void motorForward(uint8_t dutyCycle);
void motorBackward(uint8_t dutyCycle);

void startCoffee();
void servoUP();
void servoDOWN();
int distanceStop();
void stir1();
void stir2();

int stage = 0;
int distVector[INDEX_DISTANCE] = {1000,1000,1000,1000,1000};
int indexVector = 0;
int stop = 0;
unsigned long distance = 0;

int main(void) {
	init();
	while(1) {
		saveAngle();
		distance = measureDistance();

		_delay_ms(1);
		switch (stage){
		case 0:
			if (bit_is_clear(PINC,PC3)){
				startCoffee();
				_delay_ms(10000);
			}
			if (bit_is_clear(PINC,PC2)){
				servoUP();
			}
			if (bit_is_clear(PINC,PC1)){
				servoDOWN();
			}
			break;
		case 1:
			stir1();
			_delay_ms(3000);
			break;
		case 2:
			stir2();
			_delay_ms(3000);
			break;
		case 3:
			motorStop();
			stage = 4;
			_delay_ms(4000);
			break;
		case 4:
			servoDOWN();
			stage = 5;
			break;
		case 5:
			PORTC |= (1<<PC4);
			_delay_ms(10000);
			PORTC &= ~(1<<PC4);
			stage = 6;
			break;
		case 6:
			servoUP();
			stage = 0;
		}
	}
}

void init(void){
	initLEDs();
	initButtons();
	initServoControl();
	initDistanceMeasurement();
	initMotorCotntrol();
	blinkResetLed();
}

void initLEDs(){
	DDRC |= (1<<PC5); // Set Reset Led Output
	DDRC |= (1<<PC4); // Set Debug Led Output
}

void initButtons(){
	DDRC &= ~(1<<PC3); // Set Start Button Input
	PORTC |= (1<<PC3); // Enale Start Button Pull-up
	DDRC &= ~(1<<PC2); // Set Servo-Up Button Input
	PORTC |= (1<<PC2); // Enable Servo-Up Button Pull-up
	DDRC &= ~(1<<PC1); // Set Servo-Down Button Input
	PORTC |= (1<<PC1); // Enable Servo-Down Button Pull-up
}

void initServoControl(){
	DDRB |= (1<<PB1); // Set Servo Pin
	// Set Timer
	TCCR1A |= (1<<COM1A1)|(1<<WGM11);
	TCCR1B |= (1<<WGM13)|(1<<WGM12)|(1<<CS11)|(1<<CS10);
	ICR1 = (F_CPU/64UL/60ul)-1;
	uint8_t byteRead = eeprom_read_byte((uint8_t*)23); // read the byte in location 23
	if (byteRead>=3 && byteRead <= 41){
		OCR1A = byteRead; // Set angle to last value (if in memory)
	} else {
		OCR1A = MAX_ANGLE; // Set default angle if nothing in memory
	}
}

void initDistanceMeasurement(){
	DDRD |= (1 << PD0); // Set trigger pin (PD0) as output
	DDRD &= ~(1 << PD1); // Set echo pin (PD1) as input
}

void initMotorCotntrol(){
	TCCR2 |= (1 << WGM20) | (1 << WGM21) | (1 << COM21) | (1 << CS20); // Set Fast PWM mode with non-inverted output
	DDRB |= (1 << PB3); // Set OC2 (PB3) as output
	DDRD |= (1<<PD2); // Set Direction 1 Control Output
	DDRD |= (1<<PD3); // Set Direction 2 Control Output
	OCR2 = 0;
	motorStop();
}

void blinkResetLed(){
	for (int i=0; i<10; i++){
		PORTC ^= (1<<PC5);
		_delay_ms(100);
	}
}

void saveAngle(){
	eeprom_write_byte ((uint8_t*) 23, OCR1A); //  write OCR1A (angle) to location 23 of the EEPROM
}

void sendPulse() {
    PORTD |= (1 << PD0); // Set trigger pin (PD0) HIGH
    _delay_us(10); // Wait for 10 microseconds
    PORTD &= ~(1 << PD0); // Set trigger pin LOW
}

unsigned long measurePulse() {
    unsigned long pulseDuration = 0;
    while (!(PIND & (1 << PD1))); // Wait for the echo pin to go HIGH
    while (PIND & (1 << PD1)) pulseDuration++; // Measure pulse duration
    return pulseDuration;
}

unsigned long measureDistance(){
	sendPulse(); // Send pulse to trigger the sensor
	unsigned long duration = measurePulse(); // Measure the pulse duration
	// Calculate distance using duration and the speed of sound
	unsigned long new_distance = (duration * 343) / (2 * 10000); // Convert to centimeters

	if (indexVector == INDEX_DISTANCE){
		indexVector = 0;
	}
	distVector[indexVector++] = distance;
	return new_distance;
}

void motorStop(){
	OCR2 = 0;
	PORTD &= ~(1<<PD2);
	PORTD &= ~(1<<PD3);
}
void motorForward(uint8_t dutyCycle){
	PORTD &= ~(1<<PD2);
	PORTD |= (1<<PD3);
	OCR2 = dutyCycle;

}
void motorBackward(uint8_t dutyCycle){
	PORTD &= ~(1<<PD3);
	PORTD |= (1<<PD2);
	OCR2 = dutyCycle;
}

void startCoffee(){
	PORTC |= (1<<PC4);
	servoUP();
	stage = 1;
}

void servoUP(){
	for (int i = OCR1A; i <= MAX_ANGLE; i++){
		OCR1A = i;
		_delay_ms(100);
		saveAngle();
	}
}

void servoDOWN(){
	for (int i = OCR1A; i >= MIN_ANGLE; i--){
		if (!distanceStop()){
			OCR1A = i;
			_delay_ms(100);
			saveAngle();
		}
		else {
			break;
		}
	}
}

int distanceStop(){
	distance = measureDistance();
	for(int j = 0; j < INDEX_DISTANCE; j++) {
		if(distVector[j] > 7) {
			return 0;
		}
	}
	return 1;
}

void stir1(){
	PORTC &= ~(1<<PC4);
	motorStop();
	_delay_ms(50);
	motorForward(MAX_MOTOR);
	_delay_ms(1000);
	for (int i = MAX_MOTOR; i >= MIN_MOTOR; i--){
		motorForward(i);
		_delay_ms(10);
	}
	stage = 2;
}

void stir2(){
	motorStop();
	_delay_ms(50);
	motorBackward(MAX_MOTOR);
	_delay_ms(1000);
	for (int i = MAX_MOTOR; i >= MIN_MOTOR; i--){
		motorBackward(i);
		_delay_ms(10);
	}
	stage = 3;
}
