#ifndef IMAGE_H
#define IMAGE_H

#include <QMap>
#include <QNetworkReply>
#include <QDateTime>
#include <QPixmap>
#include <QSettings>
#include "tags/tag.h"
#include "pool.h"
#include "downloader/extension-rotator.h"
#include "loader/downloadable.h"


class Page;
class Site;
class Profile;

class Image : public QObject, public Downloadable
{
	Q_OBJECT

	public:
		Image();
		Image(Site *site, QMap<QString,QString> details, Profile *profile, Page *parent = Q_NULLPTR);
		Image(const Image &other);
		int			value() const;
		QString		match(QString filter, bool invert = false) const;
		QStringList	filter(const QStringList &filters) const;
		QStringList	path(QString fn = "", QString pth = "", int counter = 0, bool complex = true, bool simple = false, bool maxLength = true, bool shouldFixFilename = true, bool getFull = false) const;
		QStringList blacklisted(const QStringList &blacklistedTags, bool invert = true) const;
		QStringList	stylishedTags(Profile *profile) const;
		SaveResult  save(const QString &path, bool force = false, bool basic = false, bool addMd5 = true, bool startCommands = false, int count = 1, bool loadIfNecessary = false, bool postSave = true);
		void		postSaving(const QString &path, bool addMd5 = true, bool startCommands = false, int count = 1, bool basic = false);
		QMap<QString, Image::SaveResult> save(const QStringList &paths, bool addMd5 = true, bool startCommands = false, int count = 1, bool force = false, bool loadIfNecessary = false);
		QMap<QString, Image::SaveResult> save(const QString &filename, const QString &path, bool addMd5 = true, bool startCommands = false, int count = 1, bool loadIfNecessary = false);
		QString		md5() const;
		QString		author() const;
		QString		status() const;
		QString		rating() const;
		QString		source() const;
		QString		site() const;
		QString		filename() const;
		QString		folder() const;
		QList<Tag>	tags() const;
		QList<Tag>	filteredTags(const QStringList &remove) const;
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
		const QPixmap &previewImage();
		Page		*page() const;
		const QByteArray &data() const;
		QSettings	*settings() const;
		Site		*parentSite() const;
		QNetworkReply	*imageReply() const;
		QNetworkReply	*tagsReply() const;
		bool		hasTag(QString tag) const;
		bool		hasAnyTag(QStringList tags) const;
		bool		hasAllTags(QStringList tags) const;
		QMap<QString,QString>   details() const;
		void		setUrl(QString);
		void		setData(const QByteArray &data);
		void		setSize(QSize size);
		void		setFileSize(int size);
		void		setSavePath(const QString &savePath);
		void		setRating(QString rating);
		void		setFileExtension(QString ext);
		bool		shouldDisplaySample() const;
		QUrl		getDisplayableUrl() const;
		bool		isVideo() const;
		QString		isAnimated() const;
		void		setTags(const QList<Tag> &tags);

		// Downloadable
		virtual QString url() const override;
		virtual void preload(const Filename &filename) override;
		virtual QStringList paths(const Filename &filename, const QString &folder, int count) const override;
		virtual QMap<QString, Token> tokens(Profile *profile) const;
		virtual SaveResult preSave(const QString &path) override;
		virtual void postSave(QMap<QString, SaveResult> result, bool addMd5, bool startCommands, int count) override;

	public slots:
		void loadPreview();
		void loadDetails(bool rateLimit = false);
		void loadImage(bool inMemory = true);
		void abortPreview();
		void abortTags();
		void abortImage();
		void parsePreview();
		void parseDetails();
		void unload();

	private slots:
		void finishedImageBasic();
		void finishedImageInMemory();
		void finishedImageS(bool inMemory);
		void downloadProgressImageBasic(qint64, qint64);
		void downloadProgressImageInMemory(qint64, qint64);
		void downloadProgressImageS(qint64, qint64, bool inMemory);

	signals:
		void finishedLoadingPreview();
		void finishedLoadingTags();
		void finishedImage(QNetworkReply::NetworkError, QString);
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
		QNetworkReply::NetworkError m_loadImageError;
		ExtensionRotator m_extensionRotator;
		bool			m_loadingPreview, m_loadingDetails, m_loadingImage, m_tryingSample, m_loadedDetails, m_loadedImage;
};

Q_DECLARE_METATYPE(Image)

#endif // IMAGE_H
