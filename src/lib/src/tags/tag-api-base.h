#ifndef TAG_API_BASE_H
#define TAG_API_BASE_H

#include <QObject>
#include <QUrl>


class Api;
class NetworkReply;
class Profile;
class Site;

class TagApiBase : public QObject
{
	Q_OBJECT
	Q_ENUMS(LoadResult)

	public:
		enum LoadResult
		{
			Ok,
			Error
		};

		explicit TagApiBase(Profile *profile, Site *site, Api *api, QObject *parent = nullptr);
		~TagApiBase() override;
		void load(bool rateLimit = false);

	protected:
		void setUrl(QUrl url);
		virtual void parse(const QString &source, int statusCode, Site *site) = 0;

	public slots:
		void abort();

	protected slots:
		void parseInternal();

	signals:
		void finishedLoading(TagApiBase *api, TagApiBase::LoadResult status);

	protected:
		Profile *m_profile;
		Site *m_site;
		Api *m_api;

	private:
		QUrl m_url;
		NetworkReply *m_reply;
};

Q_DECLARE_METATYPE(TagApiBase::LoadResult)

#endif // TAG_API_BASE_H
