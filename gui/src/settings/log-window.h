#ifndef LOG_WINDOW_H
#define LOG_WINDOW_H

#include <QDialog>
#include <QMap>
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
		explicit LogWindow(int index, Profile *profile, QWidget *parent = Q_NULLPTR);
		~LogWindow() override;

	protected slots:
		void save();

	signals:
		void validated(int index, QMap<QString, QVariant> logFile);

	private:
		Ui::LogWindow *ui;
		Profile *m_profile;
		int m_index;
};

#endif // LOG_WINDOW_H
