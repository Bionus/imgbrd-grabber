#ifndef SOURCE_IMPORTER_H
#define SOURCE_IMPORTER_H

#include <QObject>
#include <QNetworkAccessManager>


class Profile;
class QNetworkReply;
class Source;

class SourceImporter : public QObject
{
	Q_OBJECT

	public:
		enum ImportResult
		{
			NetworkError,
			ZipError,
			SourceError,
			Success,
		};

		explicit SourceImporter(Profile *profile, QObject *parent = nullptr);

	public slots:
		void load(const QUrl &url) const;

	protected slots:
		void finishedLoading(QNetworkReply *reply);
		Source *importSource(const QString &path);

	signals:
		void finished(ImportResult result, QList<Source*> sources);

	private:
		Profile *m_profile;
		QNetworkAccessManager *m_manager;
};

#endif // SOURCE_IMPORTER_H
