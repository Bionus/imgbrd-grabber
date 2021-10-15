#ifndef CUSTOM_BUTTONS_STATE_H
#define CUSTOM_BUTTONS_STATE_H

#include <QString>


class ZoomWindow;
template <typename scope = ZoomWindow>
using ButtonEffect = void (scope::*)();
//template <typename scope = ZoomWindow, typename ...params>
//using ButtonEffect = void (scope::*)(params...);
//template <typename scope = ZoomWindow, typename R = void, typename ...params>
//using ButtonEffect = R (scope::*)(params...);

class ButtonState
{
	public:
		unsigned short type;
		QString text;
		QString toolTip = "";
		ButtonEffect<> function = nullptr;

		ButtonState(unsigned short type, QString text, QString toolTip = "");

		// QSettings serialization
		ButtonState() = default;
		friend QDataStream& operator <<(QDataStream &out, const ButtonState &in);
		friend QDataStream& operator >>(QDataStream &in, ButtonState &out);
};

#endif // CUSTOM_BUTTONS_STATE_H
