#ifndef START_WINDOW_H
#define START_WINDOW_H

#include <QDialog>


namespace Ui
{
	class StartWindow;
}


class Profile;

class StartWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit StartWindow(Profile *profile, QWidget *parent = nullptr);
		~StartWindow() override;

	public slots:
		void save();
		void openOptions();
		void chooseFolder();
		void chooseFormat();

	signals:
		void sourceChanged(const QString &source);
		void languageChanged(const QString &lang, bool useSystemLocale);
		void settingsChanged();

	private:
		Ui::StartWindow *ui;
		Profile *m_profile;
};

#endif // START_WINDOW_H
