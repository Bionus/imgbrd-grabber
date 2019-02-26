#ifndef FILENAME_CACHE_H
#define FILENAME_CACHE_H

#include "filename/ast-filename.h"
#include "flyweight-cache.h"


class FilenameCache : public FlyweightCache<QString, AstFilename>
{
};

#endif // FILENAME_CACHE_H
