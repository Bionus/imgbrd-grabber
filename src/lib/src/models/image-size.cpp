#include "image-size.h"
#include <QCryptographicHash>
#include <QFile>
#include <QFileInfo>
#include <QJsonObject>
#include "logger.h"


ImageSize::~ImageSize()
{
	if (!m_temporaryPath.isEmpty()) {
		log(QStringLiteral("Deleting temporary file `%1`").arg(m_temporaryPath));
		QFile::remove(m_temporaryPath);
	}
}


QString ImageSize::savePath() const
{
	return m_savePath;
}


QString ImageSize::save(const QString &path)
{
	// If we have a temporary path for this image, we move it to the destination
	if (!m_temporaryPath.isEmpty() && QFile::exists(m_temporaryPath)) {
		QString temp = m_temporaryPath;
		QFile file(m_temporaryPath);

		// Try to rename, otherwise fallback to a copy
		if (!file.rename(path)) {
			file.copy(path);
		} else {
			m_temporaryPath.clear();
		}

		m_savePath = path;
		return temp;
	}

	// If we already saved this image somewhere, simply make a copy of this file
	if (!m_savePath.isEmpty() && QFile::exists(m_savePath)) {
		QFile(m_savePath).copy(path);
		return m_savePath;
	}

	return QString();
}

bool ImageSize::setTemporaryPath(const QString &path)
{
	bool changed = setSavePath(path);

	if (m_temporaryPath == path) {
		return changed;
	}

	if (!m_temporaryPath.isEmpty()) {
		QFile::remove(m_temporaryPath);
	}

	m_temporaryPath = path;

	if (fileSize <= 0) {
		fileSize = QFileInfo(m_temporaryPath).size();
		return true;
	}

	return changed;
}

bool ImageSize::setSavePath(const QString &path)
{
	if (path != m_savePath) {
		m_savePath = path;

		if (fileSize <= 0) {
			fileSize = QFileInfo(m_savePath).size();
		}

		return true;
	}

	return false;
}


QPixmap ImageSize::pixmap() const
{
	return m_pixmap;
}

const QPixmap &ImageSize::pixmap()
{
	return m_pixmap;
}

void ImageSize::setPixmap(const QPixmap &pixmap)
{
	m_pixmap = !rect.isNull()
		? pixmap.copy(rect)
		: pixmap;
}


QString ImageSize::md5() const
{
	if (m_md5.isEmpty()) {
		const QString path = !m_savePath.isEmpty() ? m_savePath : m_temporaryPath;
		if (!path.isEmpty()) {
			QCryptographicHash hash(QCryptographicHash::Md5);

			QFile f(path);
			f.open(QFile::ReadOnly);
			hash.addData(&f);
			f.close();

			m_md5 = hash.result().toHex();
		}
	}

	return m_md5;
}


void ImageSize::read(const QJsonObject &json)
{
	if (json.contains("url")) {
		url = json["url"].toString();
	}

	if (json.contains("fileSize")) {
		fileSize = json["fileSize"].toInt();
	}

	if (json.contains("size") && json["size"].isObject()) {
		QJsonObject sz = json["size"].toObject();
		size = QSize(
			sz["width"].toInt(),
			sz["height"].toInt()
		);
	}

	if (json.contains("rect") && json["rect"].isObject()) {
		QJsonObject rct = json["rect"].toObject();
		rect = QRect(
			rct["left"].toInt(),
			rct["top"].toInt(),
			rct["width"].toInt(),
			rct["height"].toInt()
		);
	}
}

void ImageSize::write(QJsonObject &json) const
{
	if (!url.isEmpty()) {
		json["url"] = url.toString();
	}

	if (fileSize > 0) {
		json["fileSize"] = fileSize;
	}

	if (size.isValid()) {
		QJsonObject sz;
		sz["width"] = size.width();
		sz["height"] = size.height();
		json["size"] = sz;
	}

	if (rect.isValid()) {
		QJsonObject rct;
		rct["left"] = rect.left();
		rct["top"] = rect.top();
		rct["width"] = rect.width();
		rct["height"] = rect.height();
		json["rect"] = rct;
	}
}
