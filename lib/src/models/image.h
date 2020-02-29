#ifndef IMAGE_H
#define IMAGE_H

#include <QDateTime>
#include <QMap>
#include <QMetaType>
#include <QObject>
#include <QPixmap>
#include <QString>
#include "image-size.h"
#include "loader/downloadable.h"
#include "loader/token.h"
#include "models/pool.h"
#include "tags/tag.h"


class ExtensionRotator;
class NetworkReply;
class Page;
class Profile;
class QSettings;
class Site;

class Image : public QObject, public Downloadable
{
	Q_OBJECT

	public:
		Image();
		explicit Image(Profile *profile);
		Image(Site *site, QMap<QString, QString> details, Profile *profile, Page *parent = nullptr);
		Image(Site *site, QMap<QString, QString> details, QVariantMap data, Profile *profile, Page *parent = nullptr);
		Image(const Image &other);

		// Serialization
		void write(QJsonObject &json) const;
		bool read(const QJsonObject &json, const QMap<QString, Site*> &sites);

		// TODO(Bionus): remove these two methods
		QMap<QString, Image::SaveResult> save(const QString &filename, const QString &path, bool addMd5 = true, bool startCommands = false, int count = 1, Size size = Size::Full);

		int value() const;
		QString md5() const;
		const QList<Tag> &tags() const;
		QStringList tagsString() const;
		const QList<Pool> &pools() const;
		qulonglong id() const;
		int fileSize() const;
		int width() const;
		int height() const;
		const QStringList &search() const;
		const QDateTime &createdAt() const;
		const QUrl &pageUrl() const;
		const QUrl &fileUrl() const;
		QSize size(Size size = Size::Full) const;
		const QString &name() const;
		Page *page() const;
		const QUrl &parentUrl() const;
		Site *parentSite() const;
		ExtensionRotator *extensionRotator() const;
		bool hasTag(QString tag) const;
		bool hasUnknownTag() const;
		void setUrl(const QUrl &url);
		void setSize(QSize size, Size s);
		void setFileSize(int fileSize, Size s);
		void setFileExtension(const QString &ext);
		void setTemporaryPath(const QString &path, Size size = Size::Full);
		void setSavePath(const QString &path, Size size = Size::Full);
		QString savePath(Size size = Size::Full) const;
		Size preferredDisplaySize() const;
		bool isVideo() const;
		QString isAnimated() const;
		void setTags(const QList<Tag> &tags);
		bool isGallery() const;
		QString extension() const;
		void setParentGallery(const QSharedPointer<Image> &parentGallery);
		void setPromoteDetailParsWarn(bool);

		// Preview pixmap store
		QPixmap previewImage() const;
		const QPixmap &previewImage();
		void setPreviewImage(const QPixmap &preview);

		// Displayable
		QColor color() const override;
		QString tooltip() const override;
		QString counter() const override;
		QList<QStrP> detailsData() const override;

		// Downloadable
		QUrl url(Size size = Size::Full) const override;
		void preload(const Filename &filename) override;
		QStringList paths(const QString &filename, const QString &folder, int count) const;
		QStringList paths(const Filename &filename, const QString &folder, int count) const override;
		QMap<QString, Token> generateTokens(Profile *profile) const override;
		SaveResult preSave(const QString &path, Size size) override;
		void postSave(const QString &path, Size size, SaveResult result, bool addMd5, bool startCommands, int count) override;

		// Tokens
		template <typename T>
		T token(const QString &name) const
		{
			return tokens(m_profile).value(name).value<T>();
		}

	protected:
		void init();
		QList<Tag> filteredTags(const QStringList &remove) const;
		QString md5forced() const;

		// Saving
		SaveResult save(const QString &path, Size size, bool force = false, bool basic = false, bool addMd5 = true, bool startCommands = false, int count = 1, bool postSave = true);
		void postSaving(const QString &path, Size size, bool addMd5 = true, bool startCommands = false, int count = 1, bool basic = false);
		QMap<QString, Image::SaveResult> save(const QStringList &paths, bool addMd5 = true, bool startCommands = false, int count = 1, bool force = false, Size size = Size::Full);

	public slots:
		void loadDetails(bool rateLimit = false);
		void abortTags();
		void parseDetails();

	signals:
		void finishedLoadingPreview();
		void finishedLoadingTags();
		void urlChanged(const QUrl &before, const QUrl &after);

	private:
		Profile *m_profile;
		Page *m_parent = nullptr;
		qulonglong m_id;
		QString m_score;
		int m_parentId, m_authorId;
		bool m_hasChildren, m_hasNote, m_hasComments, m_hasScore;
		QUrl m_url;
		QString mutable m_md5;
		QString m_author, m_name, m_status, m_rating;
		QStringList m_sources;
		QUrl m_pageUrl;
		QUrl m_parentUrl;
		QDateTime m_createdAt;
		NetworkReply *m_loadDetails = nullptr;
		QList<Tag> m_tags;
		QList<Pool> m_pools;
		QSettings *m_settings;
		QStringList m_search;
		Site *m_parentSite;
		ExtensionRotator *m_extensionRotator;
		bool m_loadingDetails = false;
		bool m_loadedDetails = false;
		bool m_isGallery = false;
		int m_galleryCount;
		int m_position;
		bool m_detailsParsWarnAsErr = false;
		QSharedPointer<Image> m_parentGallery;
		QMap<Image::Size, QSharedPointer<ImageSize>> m_sizes;
		QVariantMap m_data;
};

Q_DECLARE_METATYPE(Image)
Q_DECLARE_METATYPE(Image::SaveResult)

#endif // IMAGE_H
