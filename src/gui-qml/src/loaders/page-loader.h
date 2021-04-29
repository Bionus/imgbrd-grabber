#ifndef PAGE_LOADER_H
#define PAGE_LOADER_H

#include "loader.h"
#include <QList>
#include <QString>


class Page;
class Profile;
class QmlImage;

class PageLoader : public Loader
{
	Q_OBJECT

	Q_PROPERTY(QString site READ site WRITE setSite NOTIFY siteChanged)
	Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)
	Q_PROPERTY(int page READ page WRITE setPage NOTIFY pageChanged)
	Q_PROPERTY(int perPage READ perPage WRITE setPerPage NOTIFY perPageChanged)
	Q_PROPERTY(QString postFilter READ postFilter WRITE setPostFilter NOTIFY postFilterChanged)
	Q_PROPERTY(Profile* profile READ profile WRITE setProfile NOTIFY profileChanged)

	Q_PROPERTY(QList<QmlImage*> results READ results NOTIFY resultsChanged)

	public:
		explicit PageLoader(QObject *parent = nullptr);

		const QString &site() const { return m_site; }
		void setSite(const QString &site) { m_site = site; emit siteChanged(); }

		const QString &query() const { return m_query; }
		void setQuery(const QString &query) { m_query = query; emit queryChanged(); }

		int page() const { return m_page; }
		void setPage(int page) { m_page = page; emit pageChanged(); }

		int perPage() const { return m_perPage; }
		void setPerPage(int perPage) { m_perPage = perPage; emit perPageChanged(); }

		const QString &postFilter() const { return m_postFilter; }
		void setPostFilter(const QString &postFilter) { m_postFilter = postFilter; emit postFilterChanged(); }

		Profile *profile() const { return m_profile; }
		void setProfile(Profile *profile) { m_profile = profile; emit profileChanged(); }

		const QList<QmlImage*> &results() const { return m_results; }

	public slots:
		void load() override;

	protected slots:
		void searchFinished(Page *page);

	signals:
		void siteChanged();
		void queryChanged();
		void pageChanged();
		void perPageChanged();
		void postFilterChanged();
		void resultsChanged();
		void profileChanged();

	private:
		QString m_site;
		QString m_query;
		int m_page;
		int m_perPage;
		QString m_postFilter;
		Profile *m_profile;

		QList<QmlImage*> m_results;
};

#endif // PAGE_LOADER_H
