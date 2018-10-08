#include "image-save-result.h"


bool operator==(const ImageSaveResult &lhs, const ImageSaveResult &rhs)
{
	return lhs.path == rhs.path
		&& lhs.size == rhs.size
		&& lhs.result == rhs.result;
}
