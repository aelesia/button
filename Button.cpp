
#include "Button.h"

/************************************************************
* pinAddress: Arduino Pin which it is connected to
* resistorPull: Voltage when button is open, accepted values:
* LOW, HIGH and INPUT_PULLUP to indicate to use internal resistor
************************************************************/
Button::Button(uint8_t pinAddress, uint8_t resistorPull) {
	this->pinAddress = pinAddress;

	if (resistorPull == INPUT_PULLUP) {
		this->resistorPull = HIGH;
		pinMode(pinAddress, resistorPull);
	} else {
		this->resistorPull = resistorPull;
		pinMode(pinAddress, INPUT);
	}
};

void Button::setOnClick(void(*onClick)()) {
	this->onClick = onClick;
}

bool Button::isClicked() {
	return stateTransition(Enums::STATE_INACTIVE, Enums::STATE_ACTIVE);
}

unsigned int Button::getClickCount() {
	return clickCount;
}

/************************************************************
* The amount of time before a button is considered to be 
* released. Accepted values: [0 - 255 milliseconds]
************************************************************/
void Button::setDebounceDelay(uint8_t ms) {
	this->debounceDelay = ms;
}

/************************************************************
* The amount of time between button polls. 
* Accepted values: [0 - 255 milliseconds]
************************************************************/
void Button::setPollInterval(uint8_t ms) {
	this->pollInterval = ms;
}

/************************************************************
* The amount of time before a button can be clicked again.
* Accepted values: [0 - 65535 milliseconds]
************************************************************/
void Button::setOnClickInterval(uint16_t ms) {
	this->onClickInterval = ms;
}


/************************************************************
* Checks the Button value for a maximum number of times of
* <UPDATE_RATE_MS> per second.
* Inverts the value if button is pulled up.
************************************************************/
Enums Button::readButtonVal() {
	return (digitalRead(pinAddress) == resistorPull) ? Enums::BUTTON_UP : Enums::BUTTON_DOWN;
}

/************************************************************
* Checks to make sure that the button has been released for
* <debounceDelay> milliseconds.
************************************************************/
bool Button::checkIfReleased(Enums buttonVal) {

	if (buttonVal == Enums::BUTTON_UP) {
		if (debounceStartTime == 0) {
			debounceStartTime = millis();
		} else if ((millis() - debounceStartTime) >= debounceDelay) {
			debounceStartTime = 0;
			return true;
		}
	} else {
		debounceStartTime = 0;
	}
	return false;
}

Enums Button::determineButtonState(Enums buttonVal) {
	if (state == Enums::STATE_INACTIVE) {
		if (buttonVal == Enums::BUTTON_DOWN && ((millis() - lastClickTime) >= onClickInterval)) {
			return Enums::STATE_ACTIVE;
		}
	} else if (checkIfReleased(buttonVal)) {
		return Enums::STATE_INACTIVE;
	}
	return state;
}

bool Button::stateTransition(Enums lastState, Enums state) {
	if (this->lastState == lastState && this->state == state) {
		return true;
	}
	return false;
}

/************************************************************
* Determines button state and triggers onClick actions.
************************************************************/
void Button::update() {

	if (millis() - lastPollTime < pollInterval) {
		return;
	}

	Enums buttonVal = readButtonVal();
	lastPollTime = millis();

	lastState = state;
	state = determineButtonState(buttonVal);

	if (lastState != state) {
		if (stateTransition(Enums::STATE_INACTIVE, Enums::STATE_ACTIVE)) {
			clickCount++;
			lastClickTime = millis();
			if (onClick != nullptr) onClick();
		}
	}
}

