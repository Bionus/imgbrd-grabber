#ifndef POST_FILTER_TEST_H
#define POST_FILTER_TEST_H

#include "models/image.h"
#include "models/site.h"
#include "models/source.h"
#include "test-suite.h"


class PostFilterTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void init();
		void cleanup();

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
