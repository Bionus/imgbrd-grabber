#ifndef OPTIONSWINDOW_H
#define OPTIONSWINDOW_H

#include <QDialog>
#include <QTreeWidgetItem>



namespace Ui
{
	class optionsWindow;
}



class optionsWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit optionsWindow(QWidget *parent = 0);
		~optionsWindow();

	public slots:
		void updateContainer(QTreeWidgetItem *, QTreeWidgetItem *);
		void setCategory(QString);
		void on_comboSourcesLetters_currentIndexChanged(int);
		void on_buttonFolder_clicked();
		void on_buttonFolderFavorites_clicked();
		void on_lineColoringArtists_textChanged();
		void on_lineColoringCircles_textChanged();
		void on_lineColoringCopyrights_textChanged();
		void on_lineColoringCharacters_textChanged();
		void on_lineColoringModels_textChanged();
		void on_lineColoringGenerals_textChanged();
		void on_lineColoringFavorites_textChanged();
		void on_lineColoringBlacklisteds_textChanged();
		void on_lineColoringIgnoreds_textChanged();
		void on_buttonColoringArtistsColor_clicked();
		void on_buttonColoringCirclesColor_clicked();
		void on_buttonColoringCopyrightsColor_clicked();
		void on_buttonColoringCharactersColor_clicked();
		void on_buttonColoringModelsColor_clicked();
		void on_buttonColoringGeneralsColor_clicked();
		void on_buttonColoringFavoritesColor_clicked();
		void on_buttonColoringBlacklistedsColor_clicked();
		void on_buttonColoringIgnoredsColor_clicked();
		void on_buttonColoringArtistsFont_clicked();
		void on_buttonColoringCirclesFont_clicked();
		void on_buttonColoringCopyrightsFont_clicked();
		void on_buttonColoringCharactersFont_clicked();
		void on_buttonColoringModelsFont_clicked();
		void on_buttonColoringGeneralsFont_clicked();
		void on_buttonColoringFavoritesFont_clicked();
		void on_buttonColoringBlacklistedsFont_clicked();
		void on_buttonColoringIgnoredsFont_clicked();
		void on_lineBorderColor_textChanged();
		void on_buttonBorderColor_clicked();
		void on_buttonFilenamePlus_clicked();
		void on_buttonFavoritesPlus_clicked();
		void on_buttonCustom_clicked();
		void addCustom(QString, QString);
		void on_buttonFilenames_clicked();
		void addFilename(QString, QString);
		void save();

	signals:
		void languageChanged(QString);
		void settingsChanged();

	private:
		Ui::optionsWindow *ui;
		QList<QLineEdit*> m_customNames, m_customTags, m_filenamesConditions, m_filenamesFilenames;
};

#endif // OPTIONSWINDOW_H
