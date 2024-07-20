#ifndef CUSTOM_BUTTONS_SETTINGS_H
#define CUSTOM_BUTTONS_SETTINGS_H

#include <QList>
#include <QMetaType>
#include <QString>
#include "custom-buttons/state.h"


class ButtonSettings
{
	public:
		/*enum ButtonWindow
		{
			Viewer
		};

		ButtonWindow onWindow;*/
		//unsigned int mask = 0;
		unsigned short type;
		QString name;
		QList<ButtonState> states;
		bool isEnabled;
		bool isInDrawer;
		unsigned short position; // Based on horizontal placement of the set bit. Left is top for vertical layouts.
		unsigned short relativeWidth; // QGridLayout column width span.

		// Sorting
		bool operator <(const ButtonSettings &str) const;

		// QSettings serialization
		friend QDataStream& operator <<(QDataStream &out, const ButtonSettings &in);
		friend QDataStream& operator >>(QDataStream &in, ButtonSettings &out);
};

Q_DECLARE_METATYPE(ButtonSettings)

#endif // CUSTOM_BUTTONS_SETTINGS_H
