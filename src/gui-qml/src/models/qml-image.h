#ifndef QML_IMAGE_H
#define QML_IMAGE_H

#include <QColor>
#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <utility>
#include "functions.h"
#include "models/image.h"
#include "models/site.h"
#include "tag-list-model.h"
#include "tags/tag-stylist.h"


class Profile;
class Settings;

class QmlImage : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString previewUrl READ previewUrl CONSTANT)
	Q_PROPERTY(QString previewRect READ previewRect CONSTANT)
	Q_PROPERTY(QString sampleUrl READ sampleUrl CONSTANT)
	Q_PROPERTY(QString fileUrl READ fileUrl CONSTANT)
	Q_PROPERTY(QString siteUrl READ siteUrl CONSTANT)
	Q_PROPERTY(TagListModel *tags READ tags CONSTANT)
	Q_PROPERTY(QString badge READ badge CONSTANT)
	Q_PROPERTY(QColor color READ color CONSTANT)
	Q_PROPERTY(QSharedPointer<Image> image READ image CONSTANT)
	Q_PROPERTY(bool isAnimated READ isAnimated CONSTANT)
	Q_PROPERTY(bool isVideo READ isVideo CONSTANT)
	Q_PROPERTY(bool isGallery READ isGallery CONSTANT)

	public:
		QmlImage(QSharedPointer<Image> image, Profile *profile, QObject *parent = nullptr)
			: QObject(parent), m_image(std::move(image)), m_profile(profile)
		{
			m_tags = new TagListModel(m_image);
		}

		QSharedPointer<Image> image() const { return m_image; }
		QString previewUrl() const { return m_image->url(Image::Size::Thumbnail).toString(); }
		QString previewRect() const { return rectToString(m_image->rect(Image::Size::Thumbnail)); }
		QString sampleUrl() const { return m_image->url(Image::Size::Sample).toString(); }
		QString fileUrl() const { return m_image->url(Image::Size::Full).toString(); }
		QString siteUrl() const { return m_image->parentSite()->url(); }
		TagListModel *tags() const { return m_tags; }
		QString badge() const { return m_image->counter(); }
		QColor color() const { return m_image->color().isValid() ? m_image->color() : QColor(0, 0, 0, 0); }
		bool isAnimated() const { return !m_image->isAnimated().isEmpty(); }
		bool isVideo() const { return m_image->isVideo(); }
		bool isGallery() const { return m_image->isGallery(); }

	public slots:
		void loadTags() { m_image->loadDetails(); };

	private:
		QSharedPointer<Image> m_image;
		Profile *m_profile;
		TagListModel *m_tags;
};

#endif // QML_IMAGE_H
