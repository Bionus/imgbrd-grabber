#ifndef OPTIONS_WINDOW_H
#define OPTIONS_WINDOW_H

#include <QDialog>
#include <QSettings>
#include <QTreeWidgetItem>
#include "reverse-search/reverse-search-engine.h"


namespace Ui
{
	class OptionsWindow;
}


class Profile;
class QCheckBox;
class QSpinBox;
class ThemeLoader;
class TokenSettingsWidget;

class OptionsWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit OptionsWindow(Profile *profile, QWidget *parent = nullptr);
		explicit OptionsWindow(Profile *profile, ThemeLoader *themeLoader, QWidget *parent = nullptr);
		~OptionsWindow() override;

		void setColor(QLineEdit *lineEdit, bool button = false);
		void setFont(QLineEdit *lineEdit);

	public slots:
		void updateContainer(QTreeWidgetItem *current, QTreeWidgetItem *previous);
		void on_comboSourcesLetters_currentIndexChanged(int);
		void on_buttonFolder_clicked();
		void on_buttonFolderFavorites_clicked();
		void on_buttonTempPathOverride_clicked();
		void on_lineColoringArtists_textChanged();
		void on_lineColoringCircles_textChanged();
		void on_lineColoringCopyrights_textChanged();
		void on_lineColoringCharacters_textChanged();
		void on_lineColoringSpecies_textChanged();
		void on_lineColoringMetas_textChanged();
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
		void on_buttonColoringMetasColor_clicked();
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
		void on_buttonColoringMetasFont_clicked();
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
		void on_lineImageBackgroundColor_textChanged();
		void on_buttonImageBackgroundColor_clicked();
		void addCustom(const QString &, const QString &);
		void on_buttonFilenames_clicked();
		void addFilename(const QString &, const QString &, const QString &);
		void on_buttonMetadataPropsysAdd_clicked();
		void on_buttonMetadataExiftoolAdd_clicked();

		// Custom image window buttons
		void initButtonSettingPairs();
		void loadButtonSettings(QSettings *settings);
		void saveButtonSettings(QSettings *settings);
		void checkAllSpinners();

		// Log files
		void addLogFile();
		void showLogFiles(QSettings *settings);
		void editLogFile(int index);
		void removeLogFile(int index);
		void setLogFile(int index, const QMap<QString, QVariant> &logFile);

		// Web services
		void addWebService();
		void showWebServices();
		void editWebService(int id);
		void removeWebService(int id);
		void setWebService(ReverseSearchEngine rse, const QByteArray &favicon);
		void moveUpWebService(int id);
		void moveDownWebService(int id);
		void swapWebServices(int a, int b);

		void save();

	signals:
		void languageChanged(const QString &lang, bool useSystemLocale);
		void settingsChanged();

	private:
		Ui::OptionsWindow *ui;
		Profile *m_profile;
		ThemeLoader *m_themeLoader;
		QList<ReverseSearchEngine> m_webServices;
		QMap<int, int> m_webServicesIds;
		QList<QLineEdit*> m_customNames, m_customTags, m_filenamesConditions, m_filenamesFilenames, m_filenamesFolders;
		QList<TokenSettingsWidget*> m_tokenSettings;
		QList<QPair<QLineEdit*, QLineEdit*>> m_metadataPropsys, m_metadataExiftool;
		QList<QPair<QCheckBox*, QSpinBox*>> m_buttonSettingPairs;
};

#endif // OPTIONS_WINDOW_H
