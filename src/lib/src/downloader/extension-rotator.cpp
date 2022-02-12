#include "downloader/extension-rotator.h"
#include <QObject>


ExtensionRotator::ExtensionRotator(const ExtensionRotator &other)
	: QObject(other.parent())
{
	m_initialExtensionIndex = other.m_initialExtensionIndex;
	m_extensions = other.m_extensions;
	m_next = other.m_next;
}

ExtensionRotator::ExtensionRotator(const QString &initialExtension, const QStringList &extensions, QObject *parent)
	: QObject(parent), m_initialExtensionIndex(extensions.indexOf(initialExtension)), m_extensions(extensions)
{}

QString ExtensionRotator::next()
{
	// Always return an empty string for empty lists
	if (m_extensions.isEmpty()) {
		return QString();
	}

	// Skip the initial extension
	if (m_next == m_initialExtensionIndex) {
		m_next++;
	}

	// If we did a full loop, that means we finished
	if (m_next >= m_extensions.size()) {
		return QString();
	}

	// Return the next extension in the list
	QString next = m_extensions[m_next % m_extensions.length()];
	m_next++;

	return next;
}
