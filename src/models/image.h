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
#include "profile.h"



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
		Image();
		Image(Site *site, QMap<QString,QString> details, Profile *profile, Page *parent = NULL);
		Image(const Image &other);
		~Image();
		int			value() const;
		QString		match(QString filter, bool invert = false) const;
		QStringList	filter(QStringList filters) const;
		QStringList	path(QString fn = "", QString pth = "", int counter = 0, bool complex = true, bool simple = false, bool maxlength = true, bool shouldFixFilename = true, bool getFull = false) const;
		QStringList blacklisted(QStringList, bool invert = true) const;
		QStringList	stylishedTags(Profile *profile) const;
		SaveResult  save(QString path, bool force = false, bool basic = false);
		QMap<QString, Image::SaveResult> save(QStringList paths);
		QMap<QString, Image::SaveResult> save(QString filename, QString path);
		QString		url() const;
		QString		md5() const;
		QString		author() const;
		QString		status() const;
		QString		rating() const;
		QString		source() const;
		QString		site() const;
		QString		filename() const;
		QString		folder() const;
		QList<Tag>	tags() const;
		QList<Tag>	filteredTags(QStringList remove) const;
		QStringList tagsString() const;
		QStringList search() const;
		QList<Pool>	pools() const;
		int			id() const;
		int			score() const;
		int			parentId() const;
		int			fileSize() const;
		int			width() const;
		int			height() const;
		int			authorId() const;
		QDateTime	createdAt() const;
		bool		hasChildren() const;
		bool		hasNote() const;
		bool		hasComments() const;
		bool		hasScore() const;
		QUrl		pageUrl() const;
		QUrl		fileUrl() const;
		QUrl		sampleUrl() const;
		QUrl		previewUrl() const;
		QSize		size() const;
		QPixmap		previewImage() const;
		Page		*page() const;
		QByteArray	data() const;
		QSettings	*settings() const;
		Site		*parentSite() const;
		QNetworkReply	*imageReply() const;
		QNetworkReply	*tagsReply() const;
		bool		hasTag(QString tag) const;
		bool		hasAnyTag(QStringList tags) const;
		bool		hasAllTags(QStringList tags) const;
		QMap<QString,QString>   details() const;
		void		setUrl(QString);
		void		setData(QByteArray);
		void		setFileSize(int);
		void		setSavePath(QString);
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
		void finishedLoadingPreview();
		void finishedLoadingTags();
		void finishedImage();
		void downloadProgressImage(qint64, qint64);
		void urlChanged(QString, QString);

	private:
		Page			*m_parent;
		int				m_id, m_score, m_parentId, m_fileSize, m_authorId, m_previewTry;
		bool			m_hasChildren, m_hasNote, m_hasComments, m_hasScore;
		QString			m_url;
		QString	mutable m_md5;
		QString			m_author, m_status, m_rating, m_source, m_site, m_filename, m_folder, m_savePath;
		QUrl			m_pageUrl, m_fileUrl, m_sampleUrl, m_previewUrl;
		QSize			m_size;
		QPixmap			m_imagePreview;
		QDateTime		m_createdAt;
		QByteArray		m_data;
		QNetworkReply	*m_loadPreview, *m_loadDetails, *m_loadImage;
		QList<Tag>		m_tags;
		QList<Pool>		m_pools;
		QTime			m_timer;
		Profile			*m_profile;
		QSettings		*m_settings;
		QStringList		m_search;
		Site			*m_parentSite;
		QMap<QString, QString>   m_details;
		bool			m_loadingPreview, m_loadingDetails, m_loadingImage, m_tryingSample;
};

Q_DECLARE_METATYPE(Image)

#endif // IMAGE_H
