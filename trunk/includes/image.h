#ifndef IMAGE_H
#define IMAGE_H

#include <QtGui>
#include <QtNetwork>
#include "page.h"

class Page;



class Image : public QObject
{
	Q_OBJECT

	public:
		Image(QMap<QString,QString> details, int timezonedecay = 0, Page *parent = NULL);
		~Image();
		void loadPreview();
		QString filter(QStringList);
		QString		url();
		QString		md5();
		QString		author();
		QString		rating();
		QString		source();
		QString		site();
		QStringList	tags();
		int			id();
		int			score();
		int			parentId();
		int			fileSize();
		int			width();
		int			height();
		int			authorId();
		QDateTime	createdAt();
		bool		hasChildren();
		bool		hasNote();
		bool		hasComments();
		QUrl		pageUrl();
		QUrl		fileUrl();
		QUrl		sampleUrl();
		QUrl		previewUrl();
		QSize		size();
		QPixmap		previewImage();
		Page		*page();

	public slots:
		void parse(QNetworkReply *);

	signals:
		void finishedLoadingPreview(Image*);

	private:
		QString		m_url, m_md5, m_author, m_rating, m_source, m_site;
		QStringList	m_tags;
		int			m_id, m_score, m_parentId, m_fileSize, m_authorId;
		QDateTime	m_createdAt;
		bool		m_hasChildren, m_hasNote, m_hasComments;
		QUrl		m_pageUrl, m_fileUrl, m_sampleUrl, m_previewUrl;
		QSize		m_size;
		QPixmap		m_imagePreview;
		Page		*m_parent;
};

#endif // IMAGE_H
