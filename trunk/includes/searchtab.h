#ifndef SEARCHTAB_H
#define SEARCHTAB_H

#include <QWidget>



class searchTab : public QWidget
{
    Q_OBJECT

    public:
        searchTab(QWidget *parent = 0);
        virtual QList<bool> sources();
        virtual QString tags();
        virtual QString results();
        virtual QString wiki();
        virtual void optionsChanged();
        virtual void updateCheckboxes();
        virtual void setTags(QString);

    signals:
        void titleChanged(searchTab*);
        void changed(searchTab*);
        void closed(searchTab*);
};

#endif // SEARCHTAB_H
