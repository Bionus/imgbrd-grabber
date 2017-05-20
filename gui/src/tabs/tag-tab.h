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
		QString tags() const;
		QList<Site*> loadSites() const override;
		void write(QJsonObject &json) const override;
		bool read(const QJsonObject &json);

	protected:
		void changeEvent(QEvent *event) override;

	public slots:
		// Zooms
		void setTags(QString);
		// Loading
		void load();
		bool validateImage(QSharedPointer<Image> img);
		// Batch
		void getPage();
		void getAll();
		// Others
		void closeEvent(QCloseEvent*);
		void on_buttonSearch_clicked();
		void focusSearch();

	private:
		TextEdit *m_search;
};

#endif // TAG_TAB_H
