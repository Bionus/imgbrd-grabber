#include "settings.h"
#include <QDataStream>
#include "state.h"


bool ButtonSettings::operator <(const ButtonSettings &str) const
{
	return position < str.position;
}

QDataStream& operator <<(QDataStream &out, const ButtonSettings &in)
{
	out << in.type;
	out << in.name;
	out << in.position;
	out << in.relativeWidth;
	out << in.states;
	out << in.isEnabled;
	out << in.isInDrawer;
	return out;
}

QDataStream& operator >>(QDataStream &in, ButtonSettings &out)
{
	in >> out.type;
	in >> out.name;
	in >> out.position;
	in >> out.relativeWidth;
	in >> out.states;
	in >> out.isEnabled;
	in >> out.isInDrawer;
	return in;
}
