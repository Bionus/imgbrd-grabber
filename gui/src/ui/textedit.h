#ifndef HEADER_TEXTEDIT
#define HEADER_TEXTEDIT

#include <QCompleter>
#include <QTextEdit>


class Profile;
class Favorite;

class TextEdit : public QTextEdit
{
	Q_OBJECT

	public:
		explicit TextEdit(Profile *profile, QWidget *parent = Q_NULLPTR);
		void setCompleter(QCompleter *c);
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
		void customContextMenuRequested(QPoint);
		void setFavorite();
		void unsetFavorite();
		void setKfl();
		void unsetKfl();

	signals:
		void returnPressed();
		void addedFavorite(const QString &name);

	private:
		QCompleter		*c;
		Profile			*m_profile;
		QList<Favorite>	&m_favorites;
		QStringList		&m_viewItLater;
};

#endif
