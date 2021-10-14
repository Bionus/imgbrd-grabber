#ifndef CUSTOM_BUTTONS_H
#define CUSTOM_BUTTONS_H

#include <QPushButton>
#include <QString>


namespace CustomButtons
{
	// Button type masks:

	constexpr unsigned short IsUtilityButton		=          0b00000001 ;	// Nib0.0, bit 0.
		constexpr unsigned short IsButtonDetails	=  0b0000000100000001 ;	// Nib2.0, bit8.
		constexpr unsigned short IsButtonOpen		=  0b0000010000000001 ;	// Nib2.2, bit10.

	constexpr unsigned short IsNavButton			=          0b00000010 ;	// Nib0.1, bit 1.
		//constexpr unsigned short IsQuit			=  0b0000000100000010 ;	// Nib2.0, bit 8.
		constexpr unsigned short IsButtonPrev		=  0b0000001000000010 ;	// Nib2.1, bit 9.
		constexpr unsigned short IsButtonNext		=  0b0000010000000010 ;	// Nib2.2, bit 10.

	constexpr unsigned short IsSavingButton			=          0b00000100 ;	// Nib0.2, bit 2.
		constexpr unsigned short IsButtonSave		=  0b0000000100000100 ;	// Nib2.0, bit8.
		constexpr unsigned short IsButtonSaveAs		=  0b0000001000000100 ;	// Nib2.1, bit9.
		constexpr unsigned short IsButtonSaveNQuit	=  0b0000010000000100 ;	// Nib2.2, bit10.

	constexpr unsigned short IsFavoriteButton		=          0b00001000 ;	// Nib0.3, bit 3.
}

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
		ButtonState() {} // QtSettings seems to require this.

	friend QDataStream & operator << (QDataStream &out, const ButtonState &in)
	{
		out << in.type;
		out << in.text;
		out << in.toolTip;
		return out;
	}
	friend QDataStream & operator >> (QDataStream &in, ButtonState &out)
	{
		in >> out.type;
		in >> out.text;
		in >> out.toolTip;
		return in;
	}
};

class ButtonInstance
{
	public:
		unsigned short type;
		QPushButton *pointer = nullptr;
		QList<ButtonState> states;
		ButtonState *current = nullptr;

		//ButtonEffect *function = nullptr;
};

class ButtonSettings
{
	public:
		/*enum ButtonWindow
		{
			Zoom
		};

		ButtonWindow onWindow;*/
		//unsigned int mask = 0;
		unsigned short type;
		std::string name;
		QList<ButtonState> states;
		bool isEnabled;
		bool isInDrawer;
		unsigned short position;	// Based on horizontal placement of the set bit. Left is top for vertical layouts.
		unsigned short relativeWidth;	// QGridLayout column width span.

	bool operator < (const ButtonSettings& str) const {return (position < str.position);}	// https://stackoverflow.com/questions/1380463/sorting-a-vector-of-custom-objects

	// https://stackoverflow.com/questions/37333084/how-to-save-custom-type-to-qsettings
	friend QDataStream & operator << (QDataStream &out, const ButtonSettings &in)
	{
		out << in.type;
		out << QString::fromStdString(in.name);
		out << in.position;
		out << in.relativeWidth;
		out << in.states;
		out << in.isEnabled;
		out << in.isInDrawer;
		return out;
	}
	friend QDataStream & operator >> (QDataStream &in, ButtonSettings &out)
	{
		in >> out.type;

		// https://forum.qt.io/topic/74962/serializing-std-string-over-qdatastream/2
		/*char *tmp;
		in >> tmp;
		if (tmp) out.name = tmp;
		delete[] tmp;*/
		/*QByteArray tmp;
		in >> tmp;
		if (!tmp.isEmpty) out.name = tmp.toStdString();
		out.name = QString::toStdString(QSt*/
		QString tmp;
		in >> tmp;
		out.name = tmp.toStdString();

		in >> out.position;
		in >> out.relativeWidth;
		in >> out.states;
		in >> out.isEnabled;
		in >> out.isInDrawer;
		return in;
	}
};

Q_DECLARE_METATYPE(ButtonSettings)

#endif // CUSTOM_BUTTONS_H
