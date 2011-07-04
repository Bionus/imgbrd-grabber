#ifndef PAGE_H
#define PAGE_H

#include <QtGui>
#include "image.h"



class Page
{
	public:
		Page(QString url, QMap<QString,QMap<QString,QString> > site);

	private:
		QString m_url, m_format;
		QList<Image*> m_images;
};

#endif // PAGE_H
