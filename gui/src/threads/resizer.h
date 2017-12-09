#ifndef RESIZER_H
#define RESIZER_H

#include <QImage>
#include <QSize>
#include <QString>


class Resizer : public QObject
{
	Q_OBJECT

	public:
		explicit Resizer(QObject *parent = Q_NULLPTR);

	public slots:
		void setSize(const QSize &size);
		void setAspectRatioMode(Qt::AspectRatioMode mode);
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
