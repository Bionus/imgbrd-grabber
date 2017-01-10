#ifndef CUSTOM_WINDOW_H
#define CUSTOM_WINDOW_H

#include <QDialog>


namespace Ui
{
	class CustomWindow;
}

class CustomWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit CustomWindow(QWidget *parent = 0);
		~CustomWindow();

	public slots:
		void accept();

	signals:
		void validated(QString, QString);

	private:
		Ui::CustomWindow *ui;
};

#endif // CUSTOM_WINDOW_H
