#ifndef IMAGE_SIZE_TEST_H
#define IMAGE_SIZE_TEST_H

#include "test-suite.h"


class ImageSizeTest : public TestSuite
{
	Q_OBJECT

	private slots:
		void testTemporaryPath();
		void testSavePath();
		void testSaveDefault();
		void testSaveMove();
		void testSaveCopy();
		void testPixmap();
		void testPixmapRect();
		void testSerialization();
};

#endif // IMAGE_SIZE_TEST_H
