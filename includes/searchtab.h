#ifndef SEARCHTAB_H
#define SEARCHTAB_H

#include <QWidget>
#include <QList>
#include <QCheckBox>
#include <QMap>
#include "QBouton.h"
#include "image.h"



class searchTab : public QWidget
{
    Q_OBJECT

    public:
		searchTab(int id, QMap<QString,Site*> *sites, QWidget *parent = 0);
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
		virtual void updateCheckboxes() = 0;
		void selectImage(Image*);
		void unselectImage(Image*);
		void toggleImage(Image*);
		int id();
		QStringList selectedImages();
		void setSources(QList<bool> sources);
		virtual void setImagesPerPage(int ipp) = 0;
		virtual void setColumns(int columns) = 0;
		virtual void setPostFilter(QString postfilter) = 0;

	public slots:
		// Sources
		void openSourcesWindow();
		void saveSources(QList<bool>);
		// Favorites
		void favorite();
		void unfavorite();
		// Pagination
		virtual void firstPage() = 0;
		virtual void previousPage() = 0;
		virtual void nextPage() = 0;
		virtual void lastPage() = 0;

    signals:
        void titleChanged(searchTab*);
        void changed(searchTab*);
        void closed(searchTab*);
		void deleted(int);

	protected:
		int					m_id;
		QList<QBouton*>		m_boutons;
		QStringList			m_selectedImages;
		QList<Image*>		m_selectedImagesPtrs;
		QList<bool>			m_selectedSources;
		QMap<QString,Site*>	*m_sites;
		QList<QCheckBox*>	m_checkboxes;
		QList<Favorite>		m_favorites;
		QString				m_link;
		QList<Tag>			m_tags;
};

#endif // SEARCHTAB_H
