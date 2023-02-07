#ifndef TAG_LIST_MODEL_H
#define TAG_LIST_MODEL_H

#include <QAbstractListModel>
#include <QList>
#include "tags/tag.h"


class Image;

class TagListModel : public QAbstractListModel
{
	Q_OBJECT

	public:
		enum MyRoles {
			NameRole = Qt::UserRole + 1,
			TypeRole
		};

		explicit TagListModel(const QSharedPointer<Image> &image, QObject *parent = nullptr);

		QHash<int, QByteArray> roleNames() const override;
		int rowCount(const QModelIndex &parent = QModelIndex()) const override;
		QVariant data(const QModelIndex &index, int role = 0) const override;

	public slots:
		void refresh();

	protected:
		void setTags(const QList<Tag> &tags);

	private:
		QList<Tag> m_tags;
};

#endif // TAG_LIST_MODEL_H
