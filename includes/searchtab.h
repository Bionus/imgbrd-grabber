#ifndef SEARCHTAB_H
#define SEARCHTAB_H

#include <QWidget>
#include <QList>
#include "QBouton.h"
#include "image.h"



class searchTab : public QWidget
{
    Q_OBJECT

    public:
        searchTab(int id, QWidget *parent = 0);
        ~searchTab();
		virtual QList<bool> sources();
        virtual QString tags() = 0;
        virtual QString results() = 0;
        virtual QString wiki() = 0;
        virtual void optionsChanged() = 0;
        virtual void updateCheckboxes() = 0;
		virtual void setTags(QString) = 0;
		void selectImage(Image*);
		void unselectImage(Image*);
		void toggleImage(Image*);
		int id();
		QStringList selectedImages();
		void setSources(QList<bool> sources);

    signals:
        void titleChanged(searchTab*);
        void changed(searchTab*);
        void closed(searchTab*);
        void deleted(int);

	protected:
		int				m_id;
		QList<QBouton*>	m_boutons;
		QStringList		m_selectedImages;
		QList<Image*>	m_selectedImagesPtrs;
		QList<bool>		m_selectedSources;
};

#endif // SEARCHTAB_H
