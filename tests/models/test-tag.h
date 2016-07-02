#ifndef TST_TAG_H
#define TST_TAG_H

#include "models/tag.h"
#include <QSettings>


class TestTag : public QObject
{
	Q_OBJECT

	public:
		TestTag();

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

#endif // TST_TAG_H
