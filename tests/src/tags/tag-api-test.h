#ifndef TAG_API_TEST_H
#define TAG_API_TEST_H

#include "test-suite.h"
#include "tags/tag-api.h"
#include "models/site.h"
#include <QMap>


class TagApiTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void init();
		void cleanup();

		void testBasic();

	private:
		Site* m_site;
};

#endif // TAG_API_TEST_H
