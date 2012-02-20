#ifndef HEADER_TEXTEDIT
#define HEADER_TEXTEDIT

#include <QtGui>
#include <QCompleter>

class TextEdit : public QTextEdit
{
	Q_OBJECT

	public:
		TextEdit(QStringList, QWidget *parent = 0);
		~TextEdit();
		void setCompleter(QCompleter *c);
		QCompleter *completer() const;
		QSize sizeHint() const;
		void doColor();
		void setText(const QString &text);

	protected:
		void keyPressEvent(QKeyEvent *e);
		void focusInEvent(QFocusEvent *e);
		void wheelEvent(QWheelEvent *e);

	private:
		QString textUnderCursor() const;

	private slots:
		void insertCompletion(const QString &completion);
		void insertFav(QAction *act);
		void customContextMenuRequested(const QPoint &pos);

	signals:
		void returnPressed();

	private:
		QCompleter *c;
		QStringList m_favorites;
};

#endif
