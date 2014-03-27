#ifndef IMAGE_H
#define IMAGE_H

#include <QMap>
#include <QNetworkReply>
#include <QDateTime>
#include <QPixmap>
#include <QSslError>
#include <QSettings>
#include "page.h"
#include "tag.h"
#include "pool.h"
#include "site.h"



class Page;
class Site;

class Image : public QObject
{
	Q_OBJECT

	public:
		Image(QMap<QString,QString> details, int timezonedecay = 0, Page *parent = NULL);
		~Image();
		int			value();
		QString		filter(QStringList);
		QString		path(QString fn = "", QString pth = "", int counter = 0, bool complex = true);
		QStringList blacklisted(QStringList);
		QStringList	stylishedTags(QStringList ignored);
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
		void		setFileSize(int);
		QSettings	*settings();
		QNetworkReply	*imageReply();
		QNetworkReply	*tagsReply();

	public slots:
		void loadPreview();
		void loadDetails();
		void loadImage();
		void abortPreview();
		void abortTags();
		void abortImage();
		void parsePreview();
		void parseDetails();

	private slots:
		void finishedImageS();
		void downloadProgressImageS(qint64, qint64);

	signals:
		void finishedLoadingPreview(Image*);
		void finishedLoadingTags(Image*);
		void finishedImage(Image*);
		void downloadProgressImage(Image*, qint64, qint64);
		void urlChanged(QString, QString);

	private:
		Page			*m_parent;
		int				m_id, m_score, m_parentId, m_fileSize, m_authorId, m_previewTry;
		bool			m_hasChildren, m_hasNote, m_hasComments, m_hasScore;
		QString			m_url, m_md5, m_author, m_status, m_rating, m_source, m_site, m_filename, m_folder;
		QUrl			m_pageUrl, m_fileUrl, m_sampleUrl, m_previewUrl;
		QSize			m_size;
		QPixmap			m_imagePreview;
		QDateTime		m_createdAt;
		QByteArray		m_data;
		QNetworkReply	*m_loadPreview, *m_loadDetails, *m_loadImage;
		QList<Tag>		m_tags;
		QList<Pool*>	m_pools;
		QTime			m_timer;
		QSettings		*m_settings;
		Site			*m_parentSite;
};

#endif // IMAGE_H
