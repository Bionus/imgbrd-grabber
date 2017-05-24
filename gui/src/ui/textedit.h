#ifndef HEADER_TEXTEDIT
#define HEADER_TEXTEDIT

#include <QTextEdit>
#include <QCompleter>


class Profile;
class Favorite;

class TextEdit : public QTextEdit
{
	Q_OBJECT

	public:
		TextEdit(Profile *profile, QWidget *parent = 0);
		~TextEdit();
		void setCompleter(QCompleter *c);
		QCompleter *completer() const;
		QSize sizeHint() const;
		void doColor();
		void setText(const QString &text);
		void setFavorites(QStringList);

	protected:
		void keyPressEvent(QKeyEvent *e);
		void focusInEvent(QFocusEvent *e);
		void wheelEvent(QWheelEvent *e);

	private:
		QString textUnderCursor() const;

	private slots:
		void insertCompletion(const QString &completion);
		void insertFav(QAction *act);
		void customContextMenuRequested(QPoint);
		void setFavorite();
		void unsetFavorite();
		void setKfl();
		void unsetKfl();

	signals:
		void returnPressed();

	private:
		QCompleter		*c;
		Profile			*m_profile;
		QList<Favorite>	&m_favorites;
		QStringList		&m_viewItLater;
};

#endif
