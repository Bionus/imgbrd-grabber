#ifndef HEADER_OPTIONSWINDOW
#define HEADER_OPTIONSWINDOW

#include <QtGui>
#include "mainWindow.h"



class optionsWindow : public QWidget
{
    Q_OBJECT

	public:
		optionsWindow(mainWindow *parent = 0);
	
	public slots:
		void save();
	
	private:
		mainWindow *parent;
		QStringList languages, codes, sources;
		QComboBox *comboLanguages, *comboSource1, *comboSource2, *comboSource3;
		QSpinBox *spinUpdates, *spinColumns, *spinLimit;
		QLineEdit *linePath, *lineFilename, *lineArtistEmpty, *lineArtistSep, *lineArtistValue, *lineCopyrightEmpty, *lineCopyrightSep, *lineCopyrightValue, *lineCharacterEmpty, *lineCharacterSep, *lineCharacterValue, *lineBlacklistedtags;
		QCheckBox *checkDownloadBlacklist, *checkArtistUseall, *checkCopyrightUseall, *checkCharacterUseall;
};

#endif
