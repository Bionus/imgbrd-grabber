#ifndef QML_SITE_H
#define QML_SITE_H

#include <QObject>
#include "models/api/api.h"
#include "models/api/api-endpoint.h"
#include "models/qml-api-endpoint.h"
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
	Q_PROPERTY(QList<QmlApiEndpoint*> endpoints READ endpoints CONSTANT)

	public:
		explicit QmlSite(Site *site, Source *source, QObject *parent = nullptr)
			: QObject(parent), m_site(site), m_source(source), m_settings(new Settings(site->settings(), this))
		{
			auto auths = m_site->getSourceEngine()->getAuths();
			for (auto it = auths.constBegin(); it != auths.constEnd(); ++it) {
				m_fields.append(new QmlAuth(it.key(), it.value(), this));
			}

			QSet<QString> added;
			for (Api *api : m_site->getApis()) {
				const auto endpoints = api->endpoints();
				for (auto it = endpoints.constBegin(); it != endpoints.constEnd(); ++it) {
					const QString &id = it.key();
					if (!it.value()->name().isEmpty() && !added.contains(id)) {
						m_endpoints.append(new QmlApiEndpoint(id, it.value()->name(), this));
						added.insert(id);
					}
				}
			}
			if (!added.contains("search")) {
				m_endpoints.prepend(new QmlApiEndpoint("", "Search", this));
			}
		}

		QString url() const { return m_site->url(); }
		QString name() const { return m_site->name(); }
		QString icon() const;
		Settings *settings() const { return m_settings; }
		QList<QmlAuth*> authFields() const { return m_fields; }
		QList<QmlApiEndpoint*> endpoints() const { return m_endpoints; }

		Site *rawSite() const { return m_site; }

	private:
		Site *m_site;
		Source *m_source;
		Settings *m_settings;
		QList<QmlAuth*> m_fields;
		QList<QmlApiEndpoint*> m_endpoints;
};

#endif // QML_SITE_H
