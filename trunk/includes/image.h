#ifndef IMAGE_H
#define IMAGE_H

#include <QtGui>
#include <QtNetwork>
#include "page.h"
#include "tag.h"
#include "pool.h"

class Page;



class Image : public QObject
{
	Q_OBJECT

	public:
		Image(QMap<QString,QString> details, int timezonedecay = 0, Page *parent = NULL);
		~Image();
		int value();
		QString filter(QStringList);
		QString path(QString fn = "", QString pth = "");
		QStringList blacklisted(QStringList);
		QString		url();
		QString		md5();
		QString		author();
		QString		status();
		QString		rating();
		QString		source();
		QString		site();
		QString		filename();
		QString		folder();
		QList<Tag>	tags();
		QList<Pool*>pools();
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
		bool		hasScore();
		QUrl		pageUrl();
		QUrl		fileUrl();
		QUrl		sampleUrl();
		QUrl		previewUrl();
		QSize		size();
		QPixmap		previewImage();
		Page		*page();
		QByteArray	data();
		void		setUrl(QString);
		void		setData(QByteArray data);
		QNetworkReply	*imageReply();

	public slots:
		void loadPreview();
		void loadTags();
		void loadImage();
		void abortPreview();
		void abortTags();
		void abortImage();
		void parsePreview(QNetworkReply*);
		void parseTags(QNetworkReply*);

	private slots:
		void finishedImageS();
		void downloadProgressImageS(qint64, qint64);

	signals:
		void finishedLoadingPreview(Image*);
		void finishedLoadingTags(Image*);
		void finishedImage(Image*);
		void downloadProgressImage(Image*, qint64, qint64);

	private:
		Page			*m_parent;
		int				m_id, m_score, m_parentId, m_fileSize, m_authorId, m_previewTry;
		bool			m_hasChildren, m_hasNote, m_hasComments, m_loadPreviewExists, m_loadTagsExists, m_hasScore, m_loadImageExists;
		QString			m_url, m_md5, m_author, m_status, m_rating, m_source, m_site, m_filename, m_folder;
		QUrl			m_pageUrl, m_fileUrl, m_sampleUrl, m_previewUrl;
		QSize			m_size;
		QPixmap			m_imagePreview;
		QDateTime		m_createdAt;
		QByteArray		m_data;
		QNetworkReply	*m_loadPreview, *m_loadTags, *m_loadImage;
		QList<Tag>		m_tags;
		QList<Pool*>	m_pools;
		QTime			m_timer;
};

#endif // IMAGE_H
