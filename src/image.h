#ifndef IMAGE_H
#define IMAGE_H

#include <QMap>
#include <QNetworkReply>
#include <QDateTime>
#include <QPixmap>
#include <QSslError>
#include <QSettings>
#include "tag.h"
#include "pool.h"



class Page;
class Site;

class Image : public QObject
{
    Q_OBJECT

	public:
		enum SaveResult
		{
			AlreadyExists,
			Ignored,
			Moved,
			Copied,
			Saved,
			Error
		};

		Image(QMap<QString,QString> details, Page *parent = NULL);
		~Image();
		int			value();
		QString		match(QString filter, bool invert = false);
		QStringList	filter(QStringList filters);
		QStringList	path(QString fn = "", QString pth = "", int counter = 0, bool complex = true, bool simple = false, bool maxlength = true, bool shouldFixFilename = true, bool getFull = false);
		QStringList blacklisted(QStringList, bool invert = true);
		QStringList	stylishedTags(QStringList ignored);
		SaveResult  save(QString path, bool force = false, bool basic = false);
		QMap<QString, Image::SaveResult> save(QStringList paths);
		QMap<QString, Image::SaveResult> save(QString filename, QString path);
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
		QStringList tagsString();
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
		void		setData(QByteArray);
		void		setFileSize(int);
		void		setSavePath(QString);
		QSettings	*settings();
		QNetworkReply	*imageReply();
		QNetworkReply	*tagsReply();
		bool		hasTag(QString tag);
		bool		hasTag(QStringList tags);
		QMap<QString,QString>   details();
		QString		detail(QString key);
		void		setRating(QString rating);
		void		setFileExtension(QString ext);

	public slots:
		void loadPreview();
		void loadDetails();
		void loadImage();
		void abortPreview();
		void abortTags();
		void abortImage();
		void parsePreview();
		void parseDetails();
		void unload();

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
		QString			m_url, m_md5, m_author, m_status, m_rating, m_source, m_site, m_filename, m_folder, m_savePath;
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
		QStringList		m_search;
		Site			*m_parentSite;
        QMap<QString,QString>   m_details;
		bool			m_loadingPreview, m_loadingDetails, m_loadingImage, m_tryingSample;
};

#endif // IMAGE_H
