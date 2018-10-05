#include "image-size.h"
#include <QFile>
#include <QFileInfo>


ImageSize::~ImageSize()
{
	if (!m_temporaryPath.isEmpty())
		QFile::remove(m_temporaryPath);
}


QString ImageSize::savePath() const
{
	return m_savePath;
}


bool ImageSize::setTemporaryPath(const QString &path)
{
	setSavePath(path);

	if (m_temporaryPath == path)
		return false;

	if (!m_temporaryPath.isEmpty())
		QFile::remove(m_temporaryPath);

	m_temporaryPath = path;

	if (fileSize <= 0)
	{
		fileSize = QFileInfo(m_temporaryPath).size();
		return true;
	}

	return false;
}
bool ImageSize::setSavePath(const QString &path)
{
	if (path != m_savePath)
	{
		m_savePath = path;

		if (fileSize <= 0)
		{ fileSize = QFileInfo(m_savePath).size(); }

		return true;
	}

	return false;
}
