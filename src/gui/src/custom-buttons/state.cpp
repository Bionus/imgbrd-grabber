#include "state.h"
#include <QDataStream>


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
