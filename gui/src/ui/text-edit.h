#ifndef TEXT_EDIT_H
#define TEXT_EDIT_H

#include <QString>
#include <QStringList>
#include <QTextEdit>


class Favorite;
class Profile;
class QCompleter;
class QWidget;

class TextEdit : public QTextEdit
{
	Q_OBJECT

	public:
		explicit TextEdit(Profile *profile, QWidget *parent = nullptr);
		void setCompleter(QCompleter *completer);
		QCompleter *completer() const;
		QSize sizeHint() const override;
		void doColor();
		void setText(const QString &text);

	protected:
		void keyPressEvent(QKeyEvent *e) override;
		void focusInEvent(QFocusEvent *e) override;
		void wheelEvent(QWheelEvent *e) override;

	private:
		QString textUnderCursor() const;

	private slots:
		void insertCompletion(const QString &completion);
		void insertFav(QAction *act);
		void customContextMenuRequested(const QPoint &pos);
		void setFavorite();
		void unsetFavorite();
		void setKfl();
		void unsetKfl();

	signals:
		void returnPressed();
		void addedFavorite(const QString &name);

	private:
		QCompleter *c;
		Profile *m_profile;
		QList<Favorite> &m_favorites;
		QStringList &m_viewItLater;
};

#endif // TEXT_EDIT_H
