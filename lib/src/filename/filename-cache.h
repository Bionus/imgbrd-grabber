#ifndef FILENAME_CACHE_H
#define FILENAME_CACHE_H

#include "flyweight-cache.h"
#include "filename/ast-filename.h"


class FilenameCache : public FlyweightCache<QString, AstFilename>
{
};

#endif // FILENAME_CACHE_H
