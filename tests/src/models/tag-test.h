#ifndef TAG_TEST_H
#define TAG_TEST_H

#include "test-suite.h"
#include "models/tag.h"
#include <QSettings>


class TagTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void init();
		void cleanup();

		void testDefaultConstructor();
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
		void testSetTypeShortType();
		void testStylishedBasic();
		void testStylishedIgnored();
		void testStylishedBlacklisted();
		void testStylishedFavorite();
		void testStylishedWithCount();
		void testTypedTextGeneral();
		void testTypedTextArtist();
		void testTypedTextSpecies();
		void testCompare();
		void testSortByFrequency();
		void testTypeSpaced();
		void testGetType();

	private:
		QSettings *m_settings;
		Profile *m_profile;
};

#endif // TAG_TEST_H
