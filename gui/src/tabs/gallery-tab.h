#ifndef GALLERY_TAB_H
#define GALLERY_TAB_H

#include <QJsonObject>
#include <QSharedPointer>
#include "tabs/search-tab.h"


namespace Ui
{
	class GalleryTab;
}


class Image;
class MainWindow;

class GalleryTab : public SearchTab
{
	Q_OBJECT

	public:
		explicit GalleryTab(Site *site, QSharedPointer<Image> gallery, Profile *profile, DownloadQueue *downloadQueue, MainWindow *parent);
		explicit GalleryTab(Profile *profile, DownloadQueue *downloadQueue, MainWindow *parent);
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
		QSharedPointer<Image> m_gallery;
};

#endif // GALLERY_TAB_H
