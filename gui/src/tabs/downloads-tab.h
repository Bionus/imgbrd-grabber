#ifndef DOWNLOADS_TAB_H
#define DOWNLOADS_TAB_H

#include <QProgressBar>
#include <QQueue>
#include <QSettings>
#include <QTableWidget>
#include <QWidget>
#include "models/image.h"
#include "models/site.h"


namespace Ui
{
	class DownloadsTab;
}


class BatchDownloadImage;
class BatchWindow;
class DownloadQueryGroup;
class DownloadQueryImage;
class ImageDownloader;
struct ImageSaveResult;
class PackLoader;
class Page;
class Profile;
class MainWindow;

class DownloadsTab : public QWidget
{
	Q_OBJECT

	public:
		explicit DownloadsTab(Profile *profile, MainWindow *parent);
		~DownloadsTab() override;

	public slots:
		// Clear
		void batchClear();
		void batchClearSel();
		void batchClearSelGroups();
		void batchClearSelUniques();
		void batchRemoveGroups(QList<int> rows);
		void batchRemoveUniques(QList<int> rows);
		void siteDeleted(Site *site);

		// Move
		void batchMove(int);
		void batchMoveUp();
		void batchMoveDown();

		// Add
		void addGroup();
		void addUnique();
		void batchAddGroup(const DownloadQueryGroup &values);
		void batchAddUnique(const DownloadQueryImage &query, bool save = true);
		void addTableItem(QTableWidget *table, int row, int col, const QString &text);

		// Update
		void updateBatchGroups(int, int);
		void updateGroupCount();

		// Downloads lists
		void on_buttonSaveLinkList_clicked();
		void on_buttonLoadLinkList_clicked();
		bool saveLinkList(const QString &filename);
		bool loadLinkList(const QString &filename);

		// Download
		void batchSel();
		void getAll(bool all = true);
		void getAllFinishedPage(Page *page);
		void getAllFinishedImages(const QList<QSharedPointer<Image>> &images);
		void getAllImages();
		void getAllGetImage(const BatchDownloadImage &download, int siteId);
		void getAllGetImageSaved(const QSharedPointer<Image> &img, QList<ImageSaveResult> result);
		void getAllProgress(const QSharedPointer<Image> &img, qint64 bytesReceived, qint64 bytesTotal);
		void getAllCancel();
		void getAllPause();
		void getAllSkip();
		void getAllLogin();
		void getNextPack();
		void getAllGetPages();
		void getAllFinished();
		void getAllFinishedLogin(Site *site, Site::LoginResult result);
		void getAllFinishedLogins();
		int getRowForSite(int siteId);
		void getAllImageOk(const BatchDownloadImage &download, int siteId, bool retry = false);
		void imageUrlChanged(const QUrl &before, const QUrl &after);
		void _getAll();

		// Others
		QIcon &getIcon(const QString &path);
		bool isDownloading() const;

	protected:
		void changeEvent(QEvent *event) override;
		void closeEvent(QCloseEvent *event) override;

	private:
		Ui::DownloadsTab *ui;
		Profile *m_profile;
		QSettings *m_settings;
		MainWindow *m_parent;

		int m_getAllDownloaded, m_getAllExists, m_getAllIgnored, m_getAllIgnoredPre, m_getAll404s, m_getAllErrors, m_getAllSkipped, m_getAllLimit;
		bool m_allow, m_getAll;
		BatchWindow *m_progressDialog;
		QMap<QUrl, QTime> m_downloadTime;
		QMap<QUrl, QTime> m_downloadTimeLast;
		QList<QProgressBar*> m_progressBars;
		QList<DownloadQueryImage> m_batchs;
		QMap<int, DownloadQueryGroup> m_batchPending;
		QSet<int> m_batchDownloading;
		QSet<int> m_batchUniqueDownloading;
		QList<DownloadQueryGroup> m_groupBatchs;
		QList<BatchDownloadImage> m_getAllRemaining, m_getAllDownloading, m_getAllFailed, m_getAllSkippedImages;
		QMap<QSharedPointer<Image>, ImageDownloader*> m_getAllImageDownloaders;
		QMap<QString, QIcon> m_icons;
		QQueue<PackLoader*> m_waitingPackLoaders;
		PackLoader *m_currentPackLoader = nullptr;
		QList<Site*> m_getAllLogins;
		int m_batchAutomaticRetries, m_getAllImagesCount, m_batchCurrentPackSize;
		QAtomicInt m_getAllCurrentlyProcessing;
};

#endif // DOWNLOADS_TAB_H
