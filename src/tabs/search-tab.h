#ifndef SEARCH_TAB_H
#define SEARCH_TAB_H

#include <QWidget>
#include <QList>
#include <QCheckBox>
#include <QSpinBox>
#include <QMap>
#include <QPushButton>
#include "ui/QBouton.h"
#include "models/image.h"
#include "models/profile.h"



class mainWindow;

class searchTab : public QWidget
{
	Q_OBJECT

	public:
		searchTab(int id, QMap<QString, Site*> *sites, Profile *profile, mainWindow *parent);
		~searchTab();
		void mouseReleaseEvent(QMouseEvent *e);
		virtual QList<bool> sources();
		virtual QString tags() = 0;
		QList<Tag> results();
		virtual QString wiki() = 0;
		virtual int imagesPerPage() = 0;
		virtual int columns() = 0;
		virtual QString postFilter() = 0;
		virtual void optionsChanged() = 0;
		virtual void setTags(QString) = 0;
		void selectImage(Image*);
		void unselectImage(Image*);
		void toggleImage(Image*);
		int id();
		QStringList selectedImages();
		void setSources(QList<bool> sources);
		virtual void setImagesPerPage(int ipp) = 0;
		virtual void setColumns(int columns) = 0;
		virtual void setPostFilter(QString postfilter) = 0;

	protected:
		void setSelectedSources(QSettings *settings);
		void setTagsFromPages(const QMap<QString, Page*> &pages);
		void addHistory(QString tags, int page, int ipp, int cols);

	public slots:
		// Sources
		void openSourcesWindow();
		void saveSources(QList<bool>);
		void updateCheckboxes();
		// Zooms
		void webZoom(int);
		// Favorites
		void favorite();
		void unfavorite();
		// Pagination
		virtual void firstPage() = 0;
		virtual void previousPage() = 0;
		virtual void nextPage() = 0;
		virtual void lastPage() = 0;
		// Focus search field
		virtual void focusSearch() = 0;
		// Batch
		void getSel();
		// History
		void historyBack();
		void historyNext();

	signals:
		// Tab events
		void titleChanged(searchTab*);
		void changed(searchTab*);
		void closed(searchTab*);
		void deleted(int);

		// Batch
		void batchAddGroup(QStringList);
		void batchAddUnique(QMap<QString,QString>);

	protected:
		Profile				*m_profile;
		int					m_id, m_lastPage, m_lastPageMaxId, m_lastPageMinId;
		QMap<QString,Site*>	*m_sites;
		QList<QBouton*>		m_boutons;
		QStringList			m_selectedImages;
		QList<Image*>		m_selectedImagesPtrs;
		QList<bool>			m_selectedSources;
		QList<QCheckBox*>	m_checkboxes;
		QList<Favorite>		m_favorites;
		QString				m_link;
		QList<Tag>			m_tags;
		mainWindow			*m_parent;
		QSettings			*m_settings;

		QStringList m_completion;
		QList<Image*> m_images;

		// History
		bool m_from_history;
		int m_history_cursor;
		QList<QMap<QString,QString>> m_history;

		// UI stuff
		QSpinBox *ui_spinPage;
		QSpinBox *ui_spinImagesPerPage;
		QSpinBox *ui_spinColumns;
		QLayout *ui_layoutSourcesList;
		QPushButton *ui_buttonHistoryBack;
		QPushButton *ui_buttonHistoryNext;
};

#endif // SEARCH_TAB_H
