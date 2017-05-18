#ifndef BUTTON_H
#define BUTTON_H
#include "Arduino.h"

enum class Enums : int8_t {
	STATE_INACTIVE,
	STATE_ACTIVE,
	BUTTON_DOWN,
	BUTTON_UP
};

class Button {

private:
	void(*onClick)() = nullptr;

	Enums state = Enums::STATE_INACTIVE;
	Enums lastState = Enums::STATE_INACTIVE;
	uint8_t clickCount = 0;
	uint16_t debounceStartTime = 0;
	uint16_t lastPollTime = 0;
	uint16_t lastClickTime = 0;

	uint8_t debounceDelay = 0;
	uint8_t pollInterval = 0;
	uint16_t onClickInterval = 0;
	uint8_t pinAddress;
	uint8_t resistorPull;

	Enums readButtonVal();
	Enums determineButtonState(Enums buttonVal);
	bool checkIfReleased(Enums buttonVal);
	bool stateTransition(Enums lastState, Enums state);

public:
	Button(uint8_t pinAddress, uint8_t pinMode);

	void update();

	void setOnClick(void(*onClick)());

	void setDebounceDelay(uint8_t ms);
	void setPollInterval(uint8_t ms);
	void setOnClickInterval(uint16_t ms);

	unsigned int getClickCount();
	bool isClicked();
};

#endif