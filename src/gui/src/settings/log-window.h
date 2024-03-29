#ifndef LOG_WINDOW_H
#define LOG_WINDOW_H

#include <QDialog>
#include <QVariant>


namespace Ui
{
	class LogWindow;
}


class Profile;

class LogWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit LogWindow(int index, Profile *profile, QWidget *parent = nullptr);
		~LogWindow() override;

	protected slots:
		void choosePath();
		void chooseFilename();
		void chooseUniquePath();
		void save();

	signals:
		void validated(int index, const QMap<QString, QVariant> &logFile);

	private:
		Ui::LogWindow *ui;
		Profile *m_profile;
		int m_index;
};

#endif // LOG_WINDOW_H
