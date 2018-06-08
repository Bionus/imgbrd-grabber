#ifndef TAG_TEST_H
#define TAG_TEST_H

#include <QSettings>
#include "tags/tag.h"
#include "test-suite.h"


class TagTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void init();
		void cleanup();

		void testDefaultConstructor();
		void testId();
		void testIdDefault();
		void testSetId();
		void testText();
		void testSetText();
		void testCount();
		void testSetCount();
		void testRelated();
		void testSetRelated();
		void testType();
		void testTypeArtistEnding();
		void testTypePrefix();
		void testTypePrefixSpecies();
		void testSetType();
		void testShortType();
		void testCompare();
		void testSortTagsByType();
		void testSortTagsByName();
		void testSortTagsByCount();
		void testTypeSpaced();
		void testGetType();

	private:
		QSettings *m_settings;
		Profile *m_profile;
};

#endif // TAG_TEST_H
