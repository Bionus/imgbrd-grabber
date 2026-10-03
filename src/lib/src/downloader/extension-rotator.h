#ifndef EXTENSION_ROTATOR_H
#define EXTENSION_ROTATOR_H

#include <QObject>
#include <QString>
#include <QStringList>


class ExtensionRotator : public QObject
{
	Q_OBJECT

	public:
		ExtensionRotator() = default;
		ExtensionRotator(const ExtensionRotator &other);
		explicit ExtensionRotator(const QString &initialExtension, const QStringList &extensions, QObject *parent = nullptr);
		QString next();

		// Rotating a failed video download into a still image extension makes the
		// retry succeed with the thumbnail instead of the video, because sites keep
		// serving the thumbnail on a 404 for the video URL. Removing the still
		// extensions keeps non-image formats the user configured (mkv, mov, ...).
		static QStringList withoutStillImages(const QStringList &extensions);

	private:
		int m_initialExtensionIndex;
		QStringList m_extensions;
		int m_next = 0;
};

#endif // EXTENSION_ROTATOR_H
