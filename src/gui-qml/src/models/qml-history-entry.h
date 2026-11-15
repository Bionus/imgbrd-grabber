#ifndef QML_HISTORY_ENTRY_H
#define QML_HISTORY_ENTRY_H

#include <QObject>
#include "models/site.h"
#include "models/history/history-entry.h"


class QmlHistoryEntry : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString query READ query CONSTANT)
	Q_PROPERTY(QStringList sites READ sites CONSTANT)

	public:
		explicit QmlHistoryEntry(const QSharedPointer<HistoryEntry> &entry, QObject *parent = nullptr)
			: QObject(parent), m_entry(entry)
		{
			for (const auto &site : m_entry->sites) {
				m_sites.append(site->url());
			}
		}

		QString query() const { return m_entry->query.tags.join(' '); }
		QStringList sites() const { return m_sites; }

	private:
		QSharedPointer<HistoryEntry> m_entry;
		QStringList m_sites;
};

#endif // QML_HISTORY_ENTRY_H
