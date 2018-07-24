#ifndef TAG_API_H
#define TAG_API_H

#include <QNetworkReply>
#include "tags/tag.h"


class Profile;
class Api;
class Site;

class TagApi : public QObject
{
	Q_OBJECT
	Q_ENUMS(LoadResult)

	public:
		enum LoadResult
		{
			Ok,
			Error
		};

		explicit TagApi(Profile *profile, Site *site, Api *api, int page = 1, int limit = 1000, QObject *parent = nullptr);
		~TagApi() override;
		void load(bool rateLimit = false);
		const QList<Tag> &tags() const;

	public slots:
		void loadNow();
		void abort();

	protected slots:
		void parse();

	signals:
		void finishedLoading(TagApi *api, TagApi::LoadResult status);

	private:
		Profile *m_profile;
		Site *m_site;
		Api *m_api;
		int m_page;
		int m_limit;
		QUrl m_url;
		QNetworkReply *m_reply;
		QList<Tag> m_tags;
};

Q_DECLARE_METATYPE(TagApi::LoadResult)

#endif // TAG_API_H
