#ifndef TAG_TEST_H
#define TAG_TEST_H

#include "models/tag.h"
#include <QSettings>


class TagTest : public QObject
{
	Q_OBJECT

	public:
        TagTest();

	private slots:
		void init();
		void cleanup();

		void testText();
		void testCount();
		void testRelated();
		void testShortType();
		void testStylishedFavorite();
		void testStylishedNotFavorite();
		void testType();
		void testTypedTextGeneral();
		void testTypedTextArtist();

	private:
		QSettings *m_settings;
};

#endif // TAG_TEST_H
