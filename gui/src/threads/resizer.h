#ifndef RESIZER_H
#define RESIZER_H

#include <QSize>
#include <QImage>
#include <QString>


class Resizer : public QObject
{
	Q_OBJECT

	public:
		Resizer(QObject *parent = Q_NULLPTR);

	public slots:
		void setSize(const QSize &size);
		void setAspectRatioMode(const Qt::AspectRatioMode mode);
		void setInput(const QImage &input);
		void setInput(const QString &filename);
		void run();

	signals:
		void error();
		void finished(const QImage &output);

	private:
		QSize m_size;
		Qt::AspectRatioMode m_aspectMode;
		QImage m_input;
		QString m_inputFilename;
};

#endif // IMAGE_THREAD_H
