#include "tag-list-model.h"
#include "models/image.h"


TagListModel::TagListModel(const QSharedPointer<Image> &image, QObject *parent)
	: QAbstractListModel(parent)
{
	setTags(image->tags());
	connect(image.data(), &Image::finishedLoadingTags, [=]() {
		setTags(image->tags());
		refresh();
	});
}

void TagListModel::setTags(const QList<Tag> &tags)
{
	m_tags = tags;
	std::sort(m_tags.begin(), m_tags.end(), sortTagsByType);
}


QHash<int, QByteArray> TagListModel::roleNames() const
{
	return {
		{ NameRole, "name" },
		{ TypeRole, "type" },
	};
}

int TagListModel::rowCount(const QModelIndex &parent) const
{
	return m_tags.count();
}

QVariant TagListModel::data(const QModelIndex &index, int role) const
{
	const Tag &tag = m_tags.at(index.row());
	if (role == NameRole) {
		return tag.text();
	}
	if (role == TypeRole) {
		return tag.type().name();
	}
	return {};
}


void TagListModel::refresh()
{
	emit dataChanged(index(0), index(rowCount() - 1));
}
