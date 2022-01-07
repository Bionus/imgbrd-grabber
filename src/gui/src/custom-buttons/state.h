#ifndef CUSTOM_BUTTONS_STATE_H
#define CUSTOM_BUTTONS_STATE_H

#include <QString>
#include <functional>


class ButtonState
{
	public:
		unsigned short type;
		QString text;
		QString toolTip = "";
		std::function<void()> function = nullptr;

		ButtonState(unsigned short type, QString text, QString toolTip = "");

		// QSettings serialization
		ButtonState() = default;
		friend QDataStream& operator <<(QDataStream &out, const ButtonState &in);
		friend QDataStream& operator >>(QDataStream &in, ButtonState &out);
};

#endif // CUSTOM_BUTTONS_STATE_H
