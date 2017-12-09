#include "threads/resizer.h"


Resizer::Resizer(QObject *parent)
	: QObject(parent), m_aspectMode(Qt::KeepAspectRatio)
{ }

void Resizer::run()
{
	if  (!m_inputFilename.isEmpty())
	{
		m_input.load(m_inputFilename);
	}

	if (m_input.isNull())
	{
		emit error();
		return;
	}

	QImage output = m_input.scaled(m_size, m_aspectMode, Qt::SmoothTransformation);
	emit finished(output);
}

void Resizer::setSize(const QSize &size)
{
	m_size = size;
}


void Resizer::setAspectRatioMode(Qt::AspectRatioMode mode)
{
	m_aspectMode = mode;
}

void Resizer::setInput(const QImage &input)
{
	m_input = input;
}

void Resizer::setInput(const QString &filename)
{
	m_inputFilename = filename;
}
