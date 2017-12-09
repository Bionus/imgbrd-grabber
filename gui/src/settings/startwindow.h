#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QDialog>


namespace Ui
{
	class startWindow;
}


class Profile;

class startWindow : public QDialog
{
	Q_OBJECT

	public:
		startWindow(Profile *profile, QWidget *parent = Q_NULLPTR);
		~startWindow() override;

	public slots:
		void save();
		void openOptions();
		void on_buttonFolder_clicked();
		void on_buttonFilenamePlus_clicked();

	signals:
		void sourceChanged(QString);
		void languageChanged(QString);
		void settingsChanged();

	private:
		Ui::startWindow *ui;
		Profile *m_profile;
};

#endif // STARTWINDOW_H
