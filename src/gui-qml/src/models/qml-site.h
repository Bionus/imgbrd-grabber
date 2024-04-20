#ifndef QML_SITE_H
#define QML_SITE_H

#include <QObject>
#include "models/qml-auth.h"
#include "models/site.h"
#include "models/source.h"
#include "models/source-engine.h"
#include "settings.h"


class QmlSite : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString url READ url CONSTANT)
	Q_PROPERTY(QString name READ name CONSTANT)
	Q_PROPERTY(QString icon READ icon CONSTANT)
	Q_PROPERTY(Settings * settings READ settings CONSTANT)
	Q_PROPERTY(QList<QmlAuth*> authFields READ authFields CONSTANT)

	public:
		explicit QmlSite(Site *site, Source *source, QObject *parent = nullptr)
			: QObject(parent), m_site(site), m_source(source), m_settings(new Settings(site->settings(), this))
		{
			auto auths = m_site->getSourceEngine()->getAuths();
			for (auto it = auths.constBegin(); it != auths.constEnd(); ++it) {
				m_fields.append(new QmlAuth(it.key(), it.value(), this));
			}
		}

		QString url() const { return m_site->url(); }
		QString name() const { return m_site->name(); }
		QString icon() const { return m_source->getPath().readPath("icon.png"); }
		Settings *settings() const { return m_settings; }
		QList<QmlAuth*> authFields() const { return m_fields; }

		Site *rawSite() const { return m_site; }

	private:
		Site *m_site;
		Source *m_source;
		Settings *m_settings;
		QList<QmlAuth*> m_fields;
};

#endif // QML_SITE_H
