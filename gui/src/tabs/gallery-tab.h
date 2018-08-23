#ifndef GALLERY_TAB_H
#define GALLERY_TAB_H

#include <QJsonObject>
#include "tabs/search-tab.h"


namespace Ui
{
	class GalleryTab;
}


class MainWindow;
class TextEdit;

class GalleryTab : public SearchTab
{
	Q_OBJECT

	public:
		explicit GalleryTab(Site *site, QString name, QUrl url, Profile *profile, MainWindow *parent);
		~GalleryTab() override;
		Ui::GalleryTab *ui;
		QString tags() const override;
		QList<Site*> loadSites() const override;
		void write(QJsonObject &json) const override;
		bool read(const QJsonObject &json, bool preload = true);

	protected:
		void changeEvent(QEvent *event) override;

	public slots:
		// Loading
		void setTags(const QString &tags, bool preload = true) override;
		void load() override;
		// Batch
		void getPage();
		void getAll();
		// Others
		void closeEvent(QCloseEvent *) override;
		void focusSearch() override;
		void updateTitle() override;

	private:
		Site *m_site;
		QString m_name;
		QUrl m_url;
};

#endif // GALLERY_TAB_H
