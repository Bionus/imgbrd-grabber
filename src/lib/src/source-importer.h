#ifndef SOURCE_IMPORTER_H
#define SOURCE_IMPORTER_H

#include <QObject>
#include <QNetworkAccessManager>


class Profile;
class QNetworkReply;

class SourceImporter : public QObject
{
	Q_OBJECT

	public:
		enum ImportResult
		{
			NetworkError,
			ZipError,
			Success,
		};

		explicit SourceImporter(Profile *profile, QObject *parent = nullptr);

	public slots:
		void load(const QUrl & url) const;

	protected slots:
		void finishedLoading(QNetworkReply *reply);

	signals:
		void finished(ImportResult result);

	private:
		Profile *m_profile;
		QNetworkAccessManager *m_manager;
};

#endif // SOURCE_IMPORTER_H
