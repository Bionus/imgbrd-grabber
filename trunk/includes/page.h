#ifndef PAGE_H
#define PAGE_H

#include <QtGui>
#include <QtNetwork>
#include "tag.h"
#include "image.h"
#include "functions.h"

class Image;



class Page : public QObject
{
	Q_OBJECT

	public:
		explicit Page(QMap<QString,QMap<QString,QString> > *sites, QString site, QStringList tags = QStringList(), int page = 1, int limit = 25, QStringList postFiltering = QStringList(), QObject *parent = 0);
		~Page();
		void					load();
		QList<Image*>			images();
		QMap<QString,QString>	site();
		int						imagesCount();
		QUrl					url();
		QString					source();
		QList<Tag*>				tags();

	public slots:
		void parse(QNetworkReply*);

	signals:
		void finishedLoading(Page*);

	private:
		QMap<QString,QString>	m_site;
		QString					m_format, m_source;
		QStringList				m_postFiltering;
		QUrl					m_url;
		QList<Image*>			m_images;
		int						m_imagesCount, m_imagesPerPage;
		QList<Tag*>				m_tags;
};

#endif // PAGE_H
