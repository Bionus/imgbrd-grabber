#ifndef IMAGE_SAVE_RESULT_H
#define IMAGE_SAVE_RESULT_H

#include <QMetaType>
#include <QString>
#include "models/image.h"


struct ImageSaveResult
{
	QString path;
	Image::Size size;
	Image::SaveResult result;
};

bool operator==(const ImageSaveResult &lhs, const ImageSaveResult &rhs);
bool operator!=(const ImageSaveResult &lhs, const ImageSaveResult &rhs);

Q_DECLARE_METATYPE(ImageSaveResult)

#endif // IMAGE_SAVE_RESULT_H
