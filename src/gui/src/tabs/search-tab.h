#ifndef SEARCH_TAB_H
#define SEARCH_TAB_H

#include <QCheckBox>
#include <QLabel>
#include <QLayout>
#include <QList>
#include <QPointer>
#include <QProgressBar>
#include <QPushButton>
#include <QSignalMapper>
#include <QSpinBox>
#include <QStackedWidget>
#include <QWidget>
#include "models/image.h"
#include "models/search-query/search-query.h"


class DownloadQueryGroup;
class DownloadQueryImage;
class DownloadQueue;
class Favorite;
class MainWindow;
class NetworkReply;
class Profile;
class ImagePreview;
class FixedSizeGridLayout;
class TextEdit;
class VerticalScrollArea;
class ViewerWindow;

class SearchTab : public QWidget
{
	Q_OBJECT

	protected:
		SearchTab(Profile *profile, DownloadQueue *downloadQueue, MainWindow *parent, QString screenName);

	public:
		~SearchTab() override;
		void init();
		void mousePressEvent(QMouseEvent *e) override;
		virtual QList<Site*> sources();
		virtual QString tags() const = 0;
		const QList<Tag> &results() const;
		const QString &wiki() const;
		int imagesPerPage() const;
		int columns() const;
		QStringList postFilter(bool includeGlobal = false) const;
		virtual void setTags(const QString &tags, bool preload = true) = 0;
		virtual bool validateImage(const QSharedPointer<Image> &img, QString &error);
		void setSources(const QList<Site*> &sources);
		void setImagesPerPage(int ipp);
		void setColumns(int columns);
		void setPostFilter(const QStringList &postFilter);
		virtual QList<Site*> loadSites() const;
		virtual void onLoad();
		virtual void write(QJsonObject &json) const = 0;
		const QString &screenName() const;
		bool isLocked() const;
		void setLocked(bool locked);

	protected:
		void setSelectedSources(QSettings *settings);
		void setTagsFromPages(const QMap<QString, QList<QSharedPointer<Page>>> &pages);
		void addHistory(const SearchQuery &query, int page, int ipp, int cols);
		QStringList reasonsToFail(Page *page, const QStringList &completion = QStringList(), QString *meant = nullptr);
		void clear();
		TextEdit *createAutocomplete();
		QWidget *createImageThumbnail();
		FixedSizeGridLayout *createImagesLayout(QSettings *settings);
		virtual void thumbnailContextMenu(QMenu *menu, const QSharedPointer<Image> &img);
		QList<QSharedPointer<Page>> getPagesToDownload();

	protected slots:
		void contextSaveSelected();
		void setMergeResultsMode(bool merged);
		void setEndlessLoadingMode(bool enabled);
		void toggleSource(const QString &url);
		void setFavoriteImage(const QString &name);

	private:
		void addLayout(QLayout *layout, int row, int column);

	public slots:
		// Sources
		void openSourcesWindow();
		void pruneSources();
		void saveSources(const QList<Site *> &sel, bool canLoad = true);
		void updateCheckboxes();
		// Zooms
		void openImage(int absolutePosition);
		void openImage(const QSharedPointer<Image> &image);
		// Pagination
		void firstPage();
		void previousPage();
		void nextPage();
		void lastPage();
		// Focus search field
		virtual void focusSearch() = 0;
		// Batch
		void getSel();
		// History
		void historyBack();
		void historyNext();
		// Results
		virtual void load() = 0;
		virtual void updateTitle() = 0;
		void loadTags(SearchQuery query);
		void endlessLoad();
		void loadPage();
		virtual void addResultsPage(Page *page, const QList<QSharedPointer<Image>> &images, bool merged, int filteredImages, const QString &noResultsMessage = nullptr);
		void setMergedLabelText(QLabel *txt, const QList<QSharedPointer<Image>> &images);
		virtual void setPageLabelText(QLabel *txt, Page *page, const QList<QSharedPointer<Image>> &images, int filteredImages, const QString &noResultsMessage = nullptr);
		void addResultsImage(const QSharedPointer<Image> &img, Page *page, bool merge = false);
		void finishedLoadingPreview();
		// Merged
		QList<QSharedPointer<Image>> mergeResults(int page, const QList<QSharedPointer<Image>> &results);
		void addMergedMd5(int page, const QString &md5);
		bool containsMergedMd5(int page, const QString &md5);
		// Loading
		void finishedLoading(Page *page);
		void failedLoading(Page *page);
		void httpsRedirect(Page *page);
		void postLoading(Page *page, const QList<QSharedPointer<Image>> &images);
		void finishedLoadingTags(Page *page);
		void updatePaginationButtons(Page *page);
		// Image selection
		void selectImage(const QSharedPointer<Image> &img);
		void unselectImage(const QSharedPointer<Image> &img);
		void toggleImage(const QSharedPointer<Image> &img);
		void toggleImage(int id, bool toggle, bool range);
		// Others
		void optionsChanged();

	signals:
		// Tab events
		void titleChanged(SearchTab*);
		void changed(SearchTab*);
		void closed(SearchTab*);

		// Batch
		void batchAddGroup(const DownloadQueryGroup &);
		void batchAddUnique(const DownloadQueryImage &);

		void tagsChanged();
		void wikiChanged();

	protected:
		Profile *m_profile;
		DownloadQueue *m_downloadQueue;
		QString m_screenName;
		int m_lastPage;
		qulonglong m_lastPageMaxId, m_lastPageMinId;
		QString m_lastPageMinDate, m_lastPageMaxDate;
		const QMap<QString, Site*> &m_sites;
		QMap<Image*, ImagePreview*> m_boutons;
		QList<QUrl> m_selectedImages;
		QList<QSharedPointer<Image>> m_selectedImagesPtrs;
		QList<Site*> m_selectedSources;
		QSignalMapper *m_checkboxesSignalMapper;
		QList<QCheckBox*> m_checkboxes;
		QList<Favorite> &m_favorites;
		QList<Tag> m_tags;
		MainWindow *m_parent;
		QSettings *m_settings;
		QString m_wiki;
		QMap<Page*, QList<QSharedPointer<Image>>> m_validImages;
		QMap<QString, QMap<QString, QString>> m_lastUrls;
		bool m_isLocked = false;

		QStringList m_completion;
		QMap<ImagePreview*, QSharedPointer<Image>> m_thumbnailsLoading;
		QList<QSharedPointer<Image>> m_images;
		QMap<QString, QList<QSharedPointer<Page>>> m_pages;
		QMap<QString, QSharedPointer<Page>> m_lastPages;
		QMap<Site*, QLabel*> m_siteLabels;
		QMap<Site*, QVBoxLayout*> m_siteLayouts;
		QMap<Page*, FixedSizeGridLayout*> m_layouts;
		int m_page;
		int m_pageMax;
		bool m_stop;
		int m_lastToggle;
		bool m_endlessLoadingEnabled, m_endlessLoadingEnabledPast;
		int m_endlessLoadOffset;
		bool m_pageMergedMode;
		QPointer<ViewerWindow> m_lastViewerWindow;

		// History
		bool m_from_history;
		int m_history_cursor;
		QList<QMap<QString, QString>> m_history;
		SearchQuery m_lastQuery;
		bool m_hasLastQuery = false;
		QList<QPair<int, QSet<QString>>> m_mergedMd5s;

		// UI stuff
		TextEdit *m_postFiltering = nullptr;
		QCheckBox *ui_checkMergeResults = nullptr;
		QProgressBar *ui_progressMergeResults = nullptr;
		QStackedWidget *ui_stackedMergeResults = nullptr;
		QSpinBox *ui_spinPage = nullptr;
		QSpinBox *ui_spinImagesPerPage = nullptr;
		QSpinBox *ui_spinColumns = nullptr;
		QWidget *ui_widgetMeant = nullptr;
		QLabel *ui_labelMeant = nullptr;
		QGridLayout *ui_layoutResults = nullptr;
		QLayout *ui_layoutSourcesList = nullptr;
		QPushButton *ui_buttonHistoryBack = nullptr;
		QPushButton *ui_buttonHistoryNext = nullptr;
		QPushButton *ui_buttonNextPage = nullptr;
		QPushButton *ui_buttonLastPage = nullptr;
		QPushButton *ui_buttonGetAll = nullptr;
		QPushButton *ui_buttonGetPage = nullptr;
		QPushButton *ui_buttonGetSel = nullptr;
		QPushButton *ui_buttonFirstPage = nullptr;
		QPushButton *ui_buttonPreviousPage = nullptr;
		QPushButton *ui_buttonEndlessLoad = nullptr;
		VerticalScrollArea *ui_scrollAreaResults = nullptr;
};

#endif // SEARCH_TAB_H
