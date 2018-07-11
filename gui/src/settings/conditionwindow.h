#ifndef CONDITIONWINDOW_H
#define CONDITIONWINDOW_H

#include <QDialog>


namespace Ui
{
	class conditionWindow;
}

class conditionWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit conditionWindow(QWidget *parent = nullptr);
		~conditionWindow() override;

	public slots:
		void accept() override;

	signals:
		void validated(const QString &condition, const QString &filename, const QString &folder);

	private:
		Ui::conditionWindow *ui;
};

#endif // CONDITIONWINDOW_H
