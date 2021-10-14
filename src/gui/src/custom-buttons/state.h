#ifndef CUSTOM_BUTTONS_STATE_H
#define CUSTOM_BUTTONS_STATE_H

#include <QString>


// https://stackoverflow.com/questions/56201976/qt-vs-constexpr-string-literal
/*struct ConstLatin1String : public QLatin1String
{
    constexpr ConstLatin1String(const char* const s) :
        QLatin1String(s, static_cast<int>(std::char_traits<char>::length(s))) {}
};*/

class ZoomWindow;
template <typename scope = ZoomWindow>
using ButtonEffect = void (scope::*)();
//template <typename scope = ZoomWindow, typename ...params>
//using ButtonEffect = void (scope::*)(params...);
//template <typename scope = ZoomWindow, typename R = void, typename ...params>
//using ButtonEffect = R (scope::*)(params...);

/*// https://www.tutorialfor.com/questions-144170.htm
class QString
{
	public:
		constexpr QString (char * str) {mStr = str;}
		char * mStr = 0;
};*/

class ButtonState
{
	public:
		unsigned short type;
		QString text;
		QString toolTip = "";
		ButtonEffect<> function = nullptr;

		//constexpr ButtonState(unsigned short type, const char *text, const char *toolTip) : type(type), text(text), toolTip(toolTip){}
		//constexpr ButtonState(const unsigned short type, const QString text, const QString toolTip) : type(type), text(text), toolTip(toolTip){}
		//constexpr ButtonState(unsigned short type, ConstLatin1String text, ConstLatin1String toolTip) : type(type), text(text), toolTip(toolTip){}
		//constexpr ButtonState(const unsigned short type, std::string text, std::string toolTip) : type(type), text(text), toolTip(toolTip){}
		ButtonState(unsigned short type, QString text, QString toolTip) : type(type), text(std::move(text)), toolTip(std::move(toolTip)) {}

		// QSettings serialization
		ButtonState() {}
		friend QDataStream& operator <<(QDataStream &out, const ButtonState &in);
		friend QDataStream& operator >>(QDataStream &in, ButtonState &out);
};

#endif // CUSTOM_BUTTONS_STATE_H
