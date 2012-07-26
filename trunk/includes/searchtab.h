#ifndef SEARCHTAB_H
#define SEARCHTAB_H

#include <QWidget>



class searchTab : public QWidget
{
    Q_OBJECT

    public:
        searchTab(QWidget *parent = 0);
		virtual QList<bool> sources() = 0;
		virtual QString tags() = 0;
		virtual QString results() = 0;
		virtual QString wiki() = 0;
		virtual void optionsChanged() = 0;
		virtual void updateCheckboxes() = 0;
		virtual void setTags(QString) = 0;

    signals:
        void titleChanged(searchTab*);
        void changed(searchTab*);
        void closed(searchTab*);
};

#endif // SEARCHTAB_H
