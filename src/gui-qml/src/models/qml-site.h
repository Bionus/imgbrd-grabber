#ifndef QML_SITE_H
#define QML_SITE_H

#include <QObject>
#include "models/site.h"
#include "settings.h"


class QmlSite : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString url READ url CONSTANT)
	Q_PROPERTY(QString name READ name CONSTANT)
	Q_PROPERTY(Settings* settings READ settings CONSTANT)

	public:
		explicit QmlSite(Site *site, QObject *parent = nullptr)
			: QObject(parent), m_site(site), m_settings(new Settings(site->settings(), this)) {}

		QString url() const { return m_site->url(); }
		QString name() const { return m_site->name(); }
		Settings *settings() const { return m_settings; }

	private:
		Site *m_site;
		Settings *m_settings;
};

#endif // QML_SITE_H
