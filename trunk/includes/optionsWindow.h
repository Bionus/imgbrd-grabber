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
		mainWindow	*parent;
		QStringList	languages, codes, sources;
		QComboBox	*comboLanguages, *comboSource1, *comboSource2, *comboSource3;
		QSpinBox	*spinColumns, *spinLimit, *m_spinUpdatesRate;
		QLineEdit	*m_lineDateFormat, *linePath, *lineFilename, *lineArtistEmpty, *lineArtistSep, *lineArtistValue, *lineCopyrightEmpty, *lineCopyrightSep, *lineCopyrightValue, *lineCharacterEmpty, *lineCharacterSep, *lineCharacterValue, *lineBlacklistedtags, *m_lineExecInit, *m_lineExecImage, *m_lineExecTag, *m_lineSaveSeparator;
		QCheckBox	*checkDownloadBlacklist, *checkArtistUseall, *checkCopyrightUseall, *checkCharacterUseall;
};

#endif
