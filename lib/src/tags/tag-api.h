#ifndef TAG_API_H
#define TAG_API_H

#include "tags/tag-api-base.h"
#include <QObject>
#include <QUrl>


class Api;
class Profile;
class Site;
class Tag;

class TagApi : public TagApiBase
{
	Q_OBJECT

	public:
		explicit TagApi(Profile *profile, Site *site, Api *api, int page = 1, int limit = 1000, QObject *parent = nullptr);
		const QList<Tag> &tags() const;

	protected:
		void parse(const QString &source, int statusCode, Site *site) override;

	private:
		int m_page;
		int m_limit;
		QList<Tag> m_tags;
};

#endif // TAG_API_H
