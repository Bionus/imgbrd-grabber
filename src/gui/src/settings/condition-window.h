#ifndef CONDITION_WINDOW_H
#define CONDITION_WINDOW_H

#include <QDialog>


namespace Ui
{
	class ConditionWindow;
}

class ConditionWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit ConditionWindow(QWidget *parent = nullptr);
		~ConditionWindow() override;

	public slots:
		void accept() override;

	signals:
		void validated(const QString &condition, const QString &filename, const QString &folder);

	private:
		Ui::ConditionWindow *ui;
};

#endif // CONDITION_WINDOW_H
