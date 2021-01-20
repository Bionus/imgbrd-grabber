#ifndef TAG_LIST_LOADER_H
#define TAG_LIST_LOADER_H

#include <QObject>
#include <QList>


class Api;
class Profile;
class Site;
class Tag;
class TagApi;

class TagListLoader : public QObject
{
	Q_OBJECT

	public:
		explicit TagListLoader(Profile *profile, Site *site, int minTagCount = 20, QObject *parent = nullptr);
		static bool canLoadTags(Site *site);

		const QString &error() const { return m_error; }
		const QList<Tag> &results() const { return m_results; }

	public slots:
		void start();
		void cancel();

	protected slots:
		void loadTagTypes(Api *apiTypes);
		void loadTags();
		void loadNextPage();
		void tagsLoaded();

	protected:
		static QList<Api*> getApisToLoadTagTypes(Site *site);
		static QList<Api*> getApisToLoadTags(Site *site, bool needTagTypes);

	signals:
		void progress(const QString &status);
		void finished();

	private:
		Profile *m_profile;
		Site *m_site;
		int m_minTagCount;

		QString m_error;
		bool m_cancelled = false;
		bool m_needTagTypes = false;
		int m_oldCount = -1;
		int m_currentPage = 1;
		Api *m_api = nullptr;
		TagApi *m_currentTagApi = nullptr;
		QList<Tag> m_results;
};

#endif // TAG_LIST_LOADER_H
