#ifndef TAG_TYPE_API_H
#define TAG_TYPE_API_H

#include <QList>
#include "tags/tag-api-base.h"
#include "tags/tag-type-with-id.h"


class Api;
class Profile;
class QObject;
class Site;

class TagTypeApi : public TagApiBase
{
	Q_OBJECT

	public:
		explicit TagTypeApi(Profile *profile, Site *site, Api *api, QObject *parent = nullptr);
		const QList<TagTypeWithId> &tagTypes() const;

	protected:
		void parse(const QString &source, int statusCode, Site *site) override;

	private:
		QList<TagTypeWithId> m_tagTypes;
};

#endif // TAG_TYPE_API_H
