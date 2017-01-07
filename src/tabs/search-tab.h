#ifndef SEARCH_TAB_H
#define SEARCH_TAB_H

#include <QWidget>
#include <QList>
#include <QCheckBox>
#include <QSpinBox>
#include <QMap>
#include <QPushButton>
#include <QLayout>
#include <QGridLayout>
#include <QLabel>
#include <QProgressBar>
#include "ui/QBouton.h"
#include "ui/textedit.h"
#include "models/image.h"
#include "models/profile.h"



class mainWindow;

class searchTab : public QWidget
{
	Q_OBJECT

	public:
		searchTab(QMap<QString, Site*> *sites, Profile *profile, mainWindow *parent);
		void mouseReleaseEvent(QMouseEvent *e);
		virtual QList<bool> sources();
		virtual QString tags() = 0;
		QList<Tag> results();
		QString wiki();
		int imagesPerPage();
		int columns();
		QString postFilter();
		virtual void setTags(QString) = 0;
		virtual bool validateImage(QSharedPointer<Image> img) = 0;
		QStringList selectedImages();
		void setSources(QList<bool> sources);
		void setImagesPerPage(int ipp);
		void setColumns(int columns);
		void setPostFilter(QString postfilter);
		virtual QList<Site*> loadSites() = 0;
		virtual void onLoad();

	protected:
		void setSelectedSources(QSettings *settings);
		void setTagsFromPages(const QMap<QString, Page*> &pages);
		void addHistory(QString tags, int page, int ipp, int cols);
		QStringList reasonsToFail(Page *page, QStringList complete = QStringList(), QString *meant = nullptr);
		QColor imageColor(QSharedPointer<Image> img) const;
		void clear();
		TextEdit *createAutocomplete();
		void loadImageThumbnails(Page *page, const QList<QSharedPointer<Image>> &imgs);
		QBouton *createImageThumbnail(int position, QSharedPointer<Image> img);
		int getActualImagesPerPage(Page *page, bool merge);
		QGridLayout *createImagesLayout(QSettings *settings);

	public slots:
		// Sources
		void openSourcesWindow();
		void saveSources(QList<bool>);
		void updateCheckboxes();
		// Zooms
		void webZoom(int);
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
		void loadTags(QStringList tags);
		bool waitForMergedResults(QList<QSharedPointer<Image>> results, QList<QSharedPointer<Image>> &imgs);
		virtual void addResultsPage(Page *page, const QList<QSharedPointer<Image>> &imgs, QString noResultsMessage = nullptr);
		void setMergedLabelText(QLabel *txt, const QList<QSharedPointer<Image>> &imgs);
		virtual void setPageLabelText(QLabel *txt, Page *page, const QList<QSharedPointer<Image>> &imgs, QString noResultsMessage = nullptr);
		void addResultsImage(QSharedPointer<Image> img, bool merge = false);
		void finishedLoadingPreview();
		// Loading
		void finishedLoading(Page *page);
		void failedLoading(Page *page);
		void postLoading(Page *page, QList<QSharedPointer<Image>> source);
		void finishedLoadingTags(Page *page);
		// Image selection
		void selectImage(QSharedPointer<Image> img);
		void unselectImage(QSharedPointer<Image> img);
		void toggleImage(QSharedPointer<Image> img);
		void toggleImage(int id, bool toggle, bool range);
		// Others
		void optionsChanged();

	signals:
		// Tab events
		void titleChanged(searchTab*);
		void changed(searchTab*);
		void closed(searchTab*);

		// Batch
		void batchAddGroup(QStringList);
		void batchAddUnique(QMap<QString,QString>);

	protected:
		Profile				*m_profile;
		int					m_lastPage, m_lastPageMaxId, m_lastPageMinId;
		QMap<QString,Site*>	*m_sites;
		QMap<QSharedPointer<Image>, QBouton*>	m_boutons;
		QStringList			m_selectedImages;
		QList<QSharedPointer<Image>>	m_selectedImagesPtrs;
		QList<bool>			m_selectedSources;
		QList<QCheckBox*>	m_checkboxes;
		QList<Favorite>		&m_favorites;
		QList<Tag>			m_tags;
		mainWindow			*m_parent;
		QSettings			*m_settings;
		QString				m_wiki;

		QStringList m_completion;
		QList<QSharedPointer<Image>> m_images;
		QMap<QString, Page*> m_pages;
		QMap<Page*, QLabel*> m_pageLabels;
		QMap<Site*, QGridLayout*> m_layouts;
		int m_page;
		int m_pagemax;
		bool m_stop;
		int m_lastToggle;

		// History
		bool m_from_history;
		int m_history_cursor;
		QList<QMap<QString,QString>> m_history;
		QString m_lastTags;

		// UI stuff
		TextEdit *m_postFiltering;
		QCheckBox *ui_checkMergeResults;
		QProgressBar *ui_progressMergeResults;
		QSpinBox *ui_spinPage;
		QSpinBox *ui_spinImagesPerPage;
		QSpinBox *ui_spinColumns;
		QWidget *ui_widgetMeant;
		QLabel *ui_labelMeant;
		QGridLayout *ui_layoutResults;
		QLayout *ui_layoutSourcesList;
		QPushButton *ui_buttonHistoryBack;
		QPushButton *ui_buttonHistoryNext;
		QPushButton *ui_buttonNextPage;
		QPushButton *ui_buttonLastPage;
		QPushButton *ui_buttonGetAll;
		QPushButton *ui_buttonGetPage;
		QPushButton *ui_buttonGetSel;
		QPushButton *ui_buttonFirstPage;
		QPushButton *ui_buttonPreviousPage;
};

#endif // SEARCH_TAB_H
