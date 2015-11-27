#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QDialog>



namespace Ui
{
	class startWindow;
}



class startWindow : public QDialog
{
	Q_OBJECT

	public:
		startWindow(QWidget *parent);
		~startWindow();

	public slots:
		void save();
		void openOptions();
		void on_buttonFolder_clicked();
		void on_buttonFilenamePlus_clicked();

	signals:
		void languageChanged(QString);
		void settingsChanged();

	private:
		Ui::startWindow *ui;

};

#endif // STARTWINDOW_H
