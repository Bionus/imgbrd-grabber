#ifndef SEARCH_LOADER_H
#define SEARCH_LOADER_H

#include "loader.h"
#include <QList>
#include <QString>
#include "models/search-query/search-query.h"

Q_MOC_INCLUDE("models/profile.h")
Q_MOC_INCLUDE("models/qml-image.h")


class Page;
class Profile;
class QmlImage;

class SearchLoader : public Loader
{
	Q_OBJECT

	Q_PROPERTY(QString site READ site WRITE setSite NOTIFY siteChanged)
	Q_PROPERTY(int page READ page WRITE setPage NOTIFY pageChanged)
	Q_PROPERTY(int perPage READ perPage WRITE setPerPage NOTIFY perPageChanged)
	Q_PROPERTY(QString postFilter READ postFilter WRITE setPostFilter NOTIFY postFilterChanged)
	Q_PROPERTY(Profile * profile READ profile WRITE setProfile NOTIFY profileChanged)

	Q_PROPERTY(QList<QmlImage*> results READ results NOTIFY resultsChanged)
	Q_PROPERTY(bool hasPrev READ hasPrev NOTIFY hasPrevChanged)
	Q_PROPERTY(bool hasNext READ hasNext NOTIFY hasNextChanged)

	public:
		explicit SearchLoader(QObject *parent = nullptr);

		const QString &site() const { return m_site; }
		void setSite(const QString &site) { m_site = site; emit siteChanged(); }

		int page() const { return m_page; }
		void setPage(int page) { m_page = page; emit pageChanged(); }

		int perPage() const { return m_perPage; }
		void setPerPage(int perPage) { m_perPage = perPage; emit perPageChanged(); }

		const QString &postFilter() const { return m_postFilter; }
		void setPostFilter(const QString &postFilter) { m_postFilter = postFilter; emit postFilterChanged(); }

		Profile *profile() const { return m_profile; }
		void setProfile(Profile *profile) { m_profile = profile; emit profileChanged(); }

		const QList<QmlImage*> &results() const { return m_results; }
		bool hasPrev() const { return m_hasPrev; }
		bool hasNext() const { return m_hasNext; }

	protected slots:
		void search(SearchQuery query);

	private slots:
		void searchFinished(Page *page);

	signals:
		void siteChanged();
		void pageChanged();
		void perPageChanged();
		void postFilterChanged();
		void resultsChanged();
		void profileChanged();
		void hasPrevChanged();
		void hasNextChanged();

	private:
		QString m_site;
		int m_page;
		int m_perPage;
		QString m_postFilter;
		Profile *m_profile;

		QList<QmlImage*> m_results;
		bool m_hasPrev;
		bool m_hasNext;
};

#endif // SEARCH_LOADER_H
