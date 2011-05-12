#ifndef HEADER_TEXTEDIT
#define HEADER_TEXTEDIT

#include <QtGui>
#include <QCompleter>

 class TextEdit : public QTextEdit
 {
	Q_OBJECT

	public:
		TextEdit(QStringList favorites, QWidget *parent = 0);
		~TextEdit();
		void setCompleter(QCompleter *c);
		QCompleter *completer() const;
		QSize sizeHint() const;
		void doColor();
		void setFavorites(QStringList favorites);

	signals:
		void returnPressed();

	protected:
		void keyPressEvent(QKeyEvent *e);
		void focusInEvent(QFocusEvent *e);
		void wheelEvent(QWheelEvent *e);

	private slots:
		void insertCompletion(const QString &completion);
		void customContextMenuRequested(QPoint);
		void insertFav(QAction *);

	private:
		QString textUnderCursor() const;
		QStringList favorites;
		QCompleter *c;
 };

#endif
