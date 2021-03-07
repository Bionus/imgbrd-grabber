#ifndef QML_SITE_H
#define QML_SITE_H

#include <QObject>
#include "models/qml-auth.h"
#include "models/site.h"
#include "models/source.h"
#include "settings.h"


class QmlSite : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString url READ url CONSTANT)
	Q_PROPERTY(QString name READ name CONSTANT)
	Q_PROPERTY(Settings* settings READ settings CONSTANT)
	Q_PROPERTY(QList<QmlAuth*> authFields READ authFields CONSTANT)

	public:
		explicit QmlSite(Site *site, QObject *parent = nullptr)
			: QObject(parent), m_site(site), m_settings(new Settings(site->settings(), this))
		{
			auto auths = m_site->getSource()->getAuths();
			for (auto it = auths.constBegin(); it != auths.constEnd(); ++it) {
				m_fields.append(new QmlAuth(it.value(), this));
			}
		}

		QString url() const { return m_site->url(); }
		QString name() const { return m_site->name(); }
		Settings *settings() const { return m_settings; }
		QList<QmlAuth*> authFields() const { return m_fields; }

	private:
		Site *m_site;
		Settings *m_settings;
		QList<QmlAuth*> m_fields;
};

#endif // QML_SITE_H
