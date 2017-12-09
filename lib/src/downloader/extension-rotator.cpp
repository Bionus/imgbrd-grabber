#include "downloader/extension-rotator.h"


ExtensionRotator::ExtensionRotator(const ExtensionRotator &other)
{
	m_initialExtension = other.m_initialExtension;
	m_extensions = other.m_extensions;
	m_next = other.m_next;
}

ExtensionRotator::ExtensionRotator(QString initialExtension, QStringList extensions, QObject *parent)
	: QObject(parent), m_initialExtension(initialExtension), m_extensions(extensions)
{
	int index = extensions.indexOf(initialExtension);

	// If the initial extension is not in the list, we return the first one
	if (index < 0)
		m_next = 0;
	else
		m_next = index + 1;
}

ExtensionRotator::~ExtensionRotator()
{
	m_extensions.clear();
}

QString ExtensionRotator::next()
{
	// Always return an empty string for empty lists
	if (m_extensions.isEmpty())
		return QString();

	QString next = m_extensions[m_next % m_extensions.length()];

	// If we did a full loop, that means we finished
	bool isLast = m_next == m_extensions.size();
	bool isNotFound = m_extensions.indexOf(m_initialExtension) < 0;
	if (next == m_initialExtension || (isLast && isNotFound))
		return QString();

	m_next++;
	return next;
}
