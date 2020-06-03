#ifndef FILENAME_NODE_H
#define FILENAME_NODE_H

class FilenameVisitor;


struct FilenameNode
{
	virtual ~FilenameNode() = default;
	virtual void accept(FilenameVisitor &v) const = 0;
};

#endif // FILENAME_NODE_H
