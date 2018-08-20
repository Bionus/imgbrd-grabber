#ifndef LOG_TAB_H
#define LOG_TAB_H

#include <QWidget>


namespace Ui
{
	class LogTab;
}


class LogTab : public QWidget
{
	Q_OBJECT

	public:
		explicit LogTab(QWidget *parent = nullptr);
		~LogTab() override;

	public slots:
		void write(const QString &msg);
		void clear();
		void open();

	protected:
		void changeEvent(QEvent *event) override;

	private:
		Ui::LogTab *ui;
};

#endif // LOG_TAB_H
