#include "state.h"
#include <QDataStream>


ButtonState::ButtonState(unsigned short type, QString text, QString toolTip)
	: type(type), text(std::move(text)), toolTip(std::move(toolTip))
{}


QDataStream& operator <<(QDataStream &out, const ButtonState &in)
{
	out << in.type;
	out << in.text;
	out << in.toolTip;
	return out;
}

QDataStream& operator >>(QDataStream &in, ButtonState &out)
{
	in >> out.type;
	in >> out.text;
	in >> out.toolTip;
	return in;
}
