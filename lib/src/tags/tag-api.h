#ifndef TAG_API_H
#define TAG_API_H

#include <QList>
#include "tags/tag.h"
#include "tags/tag-api-base.h"


class Api;
class Profile;
class QObject;
class QString;
class Site;
class Tag;

class TagApi : public TagApiBase
{
	Q_OBJECT

	public:
		explicit TagApi(Profile *profile, Site *site, Api *api, int page = 1, int limit = 1000, const QString &order = "count", QObject *parent = nullptr);
		const QList<Tag> &tags() const;

	protected:
		void parse(const QString &source, int statusCode, Site *site) override;

	private:
		QList<Tag> m_tags;
};

#endif // TAG_API_H
