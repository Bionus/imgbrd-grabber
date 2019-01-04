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
class Favorite;
class MainWindow;
class Profile;
class QBouton;
class FixedSizeGridLayout;
class TextEdit;
class VerticalScrollArea;
class ZoomWindow;

class SearchTab : public QWidget
{
	Q_OBJECT

	public:
		SearchTab(Profile *profile, MainWindow *parent);
		~SearchTab() override;
		void init();
		void mouseReleaseEvent(QMouseEvent *e) override;
		virtual QList<Site*> sources();
		virtual QString tags() const = 0;
		const QList<Tag> &results() const;
		const QString &wiki() const;
		int imagesPerPage();
		int columns();
		QString postFilter();
		virtual void setTags(const QString &tags, bool preload = true) = 0;
		virtual bool validateImage(const QSharedPointer<Image> &img, QString &error);
		void setSources(const QList<Site*> &sources);
		void setImagesPerPage(int ipp);
		void setColumns(int columns);
		void setPostFilter(const QString &postfilter);
		virtual QList<Site*> loadSites() const;
		virtual void onLoad();
		virtual void write(QJsonObject &json) const = 0;

	protected:
		void setSelectedSources(QSettings *settings);
		void setTagsFromPages(const QMap<QString, QList<QSharedPointer<Page>>> &pages);
		void addHistory(const SearchQuery &query, int page, int ipp, int cols);
		QStringList reasonsToFail(Page *page, const QStringList &completion = QStringList(), QString *meant = nullptr);
		void clear();
		TextEdit *createAutocomplete();
		void loadImageThumbnail(Page *page, QSharedPointer<Image> img, const QUrl &url);
		QBouton *createImageThumbnail(int position, const QSharedPointer<Image> &img);
		FixedSizeGridLayout *createImagesLayout(QSettings *settings);
		void thumbnailContextMenu(int position, const QSharedPointer<Image> &img);
		QList<QSharedPointer<Page>> getPagesToDownload();

	protected slots:
		void contextSaveImage(int position);
		void contextSaveImageAs(int position);
		void contextSaveSelected();
		void contextSaveImageProgress(const QSharedPointer<Image> &img, qint64 v1, qint64 v2);
		void setMergeResultsMode(bool merged);
		void setEndlessLoadingMode(bool enabled);
		void toggleSource(const QString &url);
		void setFavoriteImage(const QString &name);

	private:
		void addLayout(QLayout *layout, int row, int column);

	public slots:
		// Sources
		void openSourcesWindow();
		void saveSources(const QList<Site *> &sel, bool canLoad = true);
		void updateCheckboxes();
		// Zooms
		void webZoom(int);
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
		virtual void addResultsPage(Page *page, const QList<QSharedPointer<Image>> &imgs, bool merged, const QString &noResultsMessage = nullptr);
		void setMergedLabelText(QLabel *txt, const QList<QSharedPointer<Image>> &imgs);
		virtual void setPageLabelText(QLabel *txt, Page *page, const QList<QSharedPointer<Image>> &imgs, const QString &noResultsMessage = nullptr);
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
		void postLoading(Page *page, const QList<QSharedPointer<Image>> &imgs);
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

	protected:
		Profile *m_profile;
		int m_lastPage;
		qulonglong m_lastPageMaxId, m_lastPageMinId;
		const QMap<QString, Site*> &m_sites;
		QMap<Image*, QBouton*> m_boutons;
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

		QStringList m_completion;
		QMap<QNetworkReply*, QSharedPointer<Image>> m_thumbnailsLoading;
		QList<QSharedPointer<Image>> m_images;
		QMap<QString, QList<QSharedPointer<Page>>> m_pages;
		QMap<QString, QSharedPointer<Page>> m_lastPages;
		QMap<Site*, QLabel*> m_siteLabels;
		QMap<Site*, QVBoxLayout*> m_siteLayouts;
		QMap<Page*, FixedSizeGridLayout*> m_layouts;
		int m_page;
		int m_pagemax;
		bool m_stop;
		int m_lastToggle;
		bool m_endlessLoadingEnabled, m_endlessLoadingEnabledPast;
		int m_endlessLoadOffset;
		bool m_pageMergedMode;
		QPointer<ZoomWindow> m_lastZoomWindow;

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
