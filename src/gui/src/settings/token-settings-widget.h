#ifndef TOKEN_SETTINGS_WIDGET_H
#define TOKEN_SETTINGS_WIDGET_H

#include <QString>
#include <QWidget>


namespace Ui {
	class TokenSettingsWidget;
}


class QSettings;

class TokenSettingsWidget : public QWidget
{
	Q_OBJECT

	public:
		explicit TokenSettingsWidget(QSettings *settings, QString name, bool enableShorter, const QString &defaultEmpty = "unknown", const QString &defaultMultiple = "multiple", QWidget *parent = nullptr);
		~TokenSettingsWidget() override;

	public slots:
		void save();

	private:
		Ui::TokenSettingsWidget *ui;
		QSettings *m_settings;
		QString m_name;
		bool m_enableShorter;
};

#endif // TOKEN_SETTINGS_WIDGET_H
