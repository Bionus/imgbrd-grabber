#ifndef TAG_TYPE_FACTORY_H
#define TAG_TYPE_FACTORY_H

#include <QString>
#include "flyweight-cache.h"


class TagType;

class TagTypeFactory : public FlyweightCache<QString, TagType>
{};

#endif // TAG_TYPE_FACTORY_H
