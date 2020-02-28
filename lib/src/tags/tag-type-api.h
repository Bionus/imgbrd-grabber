#ifndef TAG_TYPE_API_H
#define TAG_TYPE_API_H

#include "models/api/api.h"
#include "tags/tag-api-base.h"
#include <QObject>
#include <QUrl>


class Api;
class Profile;
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
