#ifndef POST_FILTER_TEST_H
#define POST_FILTER_TEST_H

#include <QMap>
#include <QString>
#include "test-suite.h"


class Image;
class Profile;
class QSettings;
class Site;
class Source;

class PostFilterTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void init();
		void cleanup();

		void testCount();
		void testFilterNumeric();
		void testFilterSpecial();
		void testFilterInvert();

	private:
		Profile *m_profile;
		QSettings *m_settings;
		Source *m_source;
		Site *m_site;
		Image *m_img;
		QMap<QString, QString> m_details;
};

#endif // POST_FILTER_TEST_H
