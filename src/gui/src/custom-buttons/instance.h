#ifndef CUSTOM_BUTTONS_INSTANCE_H
#define CUSTOM_BUTTONS_INSTANCE_H

#include <QList>


class ButtonState;
class QPushButton;

class ButtonInstance
{
	public:
		unsigned short type;
		QPushButton *pointer = nullptr;
		QList<ButtonState> states;
		ButtonState *current = nullptr;
		// ButtonEffect *function = nullptr;
};

#endif // CUSTOM_BUTTONS_INSTANCE_H
