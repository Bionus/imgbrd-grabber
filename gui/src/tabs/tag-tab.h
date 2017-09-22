#ifndef TAG_TAB_H
#define TAG_TAB_H

#include <QWidget>
#include <QMap>
#include <QCalendarWidget>
#include <QJsonObject>
#include "search-tab.h"


namespace Ui
{
	class tagTab;
}


class mainWindow;
class Downloader;
class TextEdit;

class tagTab : public searchTab
{
	Q_OBJECT

	public:
		explicit tagTab(QMap<QString, Site*> *sites, Profile *profile, mainWindow *parent);
		~tagTab();
		Ui::tagTab *ui;
		QString tags() const override;
		void write(QJsonObject &json) const override;
		bool read(const QJsonObject &json, bool preload = true);

	protected:
		void changeEvent(QEvent *event) override;

	public slots:
		// Zooms
		void setTags(QString tags, bool preload = true) override;
		// Loading
		void load() override;
		// Batch
		void getPage();
		void getAll();
		// Others
		void closeEvent(QCloseEvent*) override;
		void on_buttonSearch_clicked();
		void focusSearch() override;

	private:
		TextEdit *m_search;
};

#endif // TAG_TAB_H
