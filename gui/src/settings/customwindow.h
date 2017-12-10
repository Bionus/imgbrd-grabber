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
		explicit CustomWindow(QWidget *parent = Q_NULLPTR);
		~CustomWindow() override;

	public slots:
		void accept() override;

	signals:
		void validated(const QString &name, const QString &tags);

	private:
		Ui::CustomWindow *ui;
};

#endif // CUSTOM_WINDOW_H
