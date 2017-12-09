#ifndef OPTIONSWINDOW_H
#define OPTIONSWINDOW_H

#include <QDialog>
#include <QSettings>
#include <QTreeWidgetItem>
#include "reverse-search/reverse-search-engine.h"


namespace Ui
{
	class optionsWindow;
}


class Profile;

class optionsWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit optionsWindow(Profile *profile, QWidget *parent = Q_NULLPTR);
		~optionsWindow() override;
		void setColor(QLineEdit *lineEdit, bool button = false);
		void setFont(QLineEdit *lineEdit);

	public slots:
		void updateContainer(QTreeWidgetItem *, QTreeWidgetItem *);
		void on_comboSourcesLetters_currentIndexChanged(int);
		void on_buttonFolder_clicked();
		void on_buttonFolderFavorites_clicked();
		void on_lineColoringArtists_textChanged();
		void on_lineColoringCircles_textChanged();
		void on_lineColoringCopyrights_textChanged();
		void on_lineColoringCharacters_textChanged();
		void on_lineColoringSpecies_textChanged();
		void on_lineColoringModels_textChanged();
		void on_lineColoringGenerals_textChanged();
		void on_lineColoringFavorites_textChanged();
		void on_lineColoringKeptForLater_textChanged();
		void on_lineColoringBlacklisteds_textChanged();
		void on_lineColoringIgnoreds_textChanged();
		void on_buttonColoringArtistsColor_clicked();
		void on_buttonColoringCirclesColor_clicked();
		void on_buttonColoringCopyrightsColor_clicked();
		void on_buttonColoringCharactersColor_clicked();
		void on_buttonColoringSpeciesColor_clicked();
		void on_buttonColoringModelsColor_clicked();
		void on_buttonColoringGeneralsColor_clicked();
		void on_buttonColoringFavoritesColor_clicked();
		void on_buttonColoringKeptForLaterColor_clicked();
		void on_buttonColoringBlacklistedsColor_clicked();
		void on_buttonColoringIgnoredsColor_clicked();
		void on_buttonColoringArtistsFont_clicked();
		void on_buttonColoringCirclesFont_clicked();
		void on_buttonColoringCopyrightsFont_clicked();
		void on_buttonColoringCharactersFont_clicked();
		void on_buttonColoringSpeciesFont_clicked();
		void on_buttonColoringModelsFont_clicked();
		void on_buttonColoringGeneralsFont_clicked();
		void on_buttonColoringFavoritesFont_clicked();
		void on_buttonColoringKeptForLaterFont_clicked();
		void on_buttonColoringBlacklistedsFont_clicked();
		void on_buttonColoringIgnoredsFont_clicked();
		void on_lineBorderColor_textChanged();
		void on_buttonBorderColor_clicked();
		void on_buttonFilenamePlus_clicked();
		void on_buttonFavoritesPlus_clicked();
		void on_buttonCustom_clicked();
		void on_buttonImageBackgroundColor_textChanged();
		void on_buttonImageBackgroundColor_clicked();
		void addCustom(QString, QString);
		void on_buttonFilenames_clicked();
		void addFilename(QString, QString, QString);

		// Log files
		void addLogFile();
		void showLogFiles(QSettings *settings);
		void editLogFile(int index);
		void removeLogFile(int index);
		void setLogFile(int index, QMap<QString, QVariant> logFile);

		// Web services
		void addWebService();
		void showWebServices();
		void editWebService(int id);
		void removeWebService(int id);
		void setWebService(ReverseSearchEngine rse, QByteArray favicon);
		void moveUpWebService(int id);
		void moveDownWebService(int id);
		void swapWebServices(int a, int b);

		void save();

	signals:
		void languageChanged(QString);
		void settingsChanged();

	private:
		Ui::optionsWindow *ui;
		Profile *m_profile;
		QList<ReverseSearchEngine> m_webServices;
		QMap<int, int> m_webServicesIds;
		QList<QLineEdit*> m_customNames, m_customTags, m_filenamesConditions, m_filenamesFilenames, m_filenamesFolders;
};

#endif // OPTIONSWINDOW_H
