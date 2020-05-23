#ifndef SETTINGS_DOCK_H
#define SETTINGS_DOCK_H

#include <QStringList>
#include <QWidget>
#include "dock.h"


namespace Ui
{
	class SettingsDock;
}


class Profile;
class QEvent;
class QSettings;

class SettingsDock : public Dock
{
	Q_OBJECT

	public:
		explicit SettingsDock(Profile *profile, QWidget *parent);
		~SettingsDock() override;

	protected:
		void changeEvent(QEvent *event) override;
		void saveSettings();

	public slots:
		void reset();

	protected slots:
		void chooseFolder();
		void save();
		void updateCompleters();

	private:
		Ui::SettingsDock *ui;
		Profile *m_profile;
		QSettings *m_settings;
		QStringList m_lineFilename_completer;
		QStringList m_lineFolder_completer;
};

#endif // SETTINGS_DOCK_H
