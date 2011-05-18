#include <QApplication>
#include "textedit.h"

TextEdit::TextEdit(QStringList favorites, QWidget *parent) : QTextEdit(parent), favorites(favorites), c(0)
{
	setAcceptRichText(false);
	setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
	setWordWrapMode(QTextOption::NoWrap);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setTabChangesFocus(true);
	setMaximumHeight(21);
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenuRequested(QPoint)));
}

TextEdit::~TextEdit()
{
}

void TextEdit::setCompleter(QCompleter *completer)
{
	 if (c)
		 QObject::disconnect(c, 0, this, 0);

	 c = completer;

	 if (!c)
		 return;

	 c->setWidget(this);
	 c->setCompletionMode(QCompleter::PopupCompletion);
	 c->setCaseSensitivity(Qt::CaseInsensitive);
	 QObject::connect(c, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));
}

void TextEdit::setFavorites(QStringList favorites)
{ this->favorites = favorites; }

QCompleter *TextEdit::completer() const
{
	return c;
}

void TextEdit::insertCompletion(const QString& completion)
{
	if (c->widget() != this)
	{ return; }
	QTextCursor tc = textCursor();
	int extra = completion.length() - c->completionPrefix().length();
	tc.movePosition(QTextCursor::Left);
	tc.movePosition(QTextCursor::EndOfWord);
	tc.insertText(completion.right(extra));
	setTextCursor(tc);
	doColor();
}

 QString TextEdit::textUnderCursor() const
{
	int size = 0, pos = textCursor().position();
	QString topos = this->toPlainText().left(pos), plain = " "+this->toPlainText()+" ";
	pos = pos-topos.count("-");
	QRegExp reg(" [~-]([^ ]*) ");
	plain.replace(" ", "  ").replace(reg, " \\1 ");
	plain = plain.replace("  ", " ").trimmed();
	QStringList words = plain.split(" ", QString::SkipEmptyParts);
	QString word;
	for (int i = 0; i < words.count(); i++)
	{
		if (size <= pos && pos <= size+words.at(i).length())
		{ word = words.at(i); }
		size += words.at(i).length()+1;
	}
	return word;
}

void TextEdit::focusInEvent(QFocusEvent *e)
{
	if (c)
	{ c->setWidget(this); }
	QTextEdit::focusInEvent(e);
}

void TextEdit::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
	{
		e->ignore();
		if (!c || !c->popup()->isVisible())
		{
			this->doColor();
			emit this->returnPressed();
		}
		return;
	}
	if (c && c->popup()->isVisible())
	{
		// The following keys are forwarded by the completer to the widget
		switch (e->key())
		{
			case Qt::Key_Enter:
			case Qt::Key_Return:
			case Qt::Key_Escape:
			case Qt::Key_Tab:
			case Qt::Key_Backtab:
				e->ignore();
				this->doColor();
				return; // let the completer do default behavior
			default:
				break;
		}
	}

	bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_Space); // CTRL+Space
	if (!c || !isShortcut) // dont process the shortcut when we have a completer
	{ QTextEdit::keyPressEvent(e); }
	const bool ctrlOrShift = (e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier));
	if (!c || (ctrlOrShift && e->text().isEmpty()))
	{ this->doColor(); return; }

	static QString eow("- "); // end of word
	bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
	QString completionPrefix = textUnderCursor();

	if (!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 3 || eow.contains(e->text().right(1))))
	{
		c->popup()->hide();
		this->doColor();
		return;
	}

	if (completionPrefix != c->completionPrefix())
	{
		c->setCompletionPrefix(completionPrefix);
		c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
	}
	QRect cr = cursorRect();
	cr.setWidth(c->popup()->sizeHintForColumn(0) + c->popup()->verticalScrollBar()->sizeHint().width());
	c->complete(cr); // popup it up!
	this->doColor();
}

void TextEdit::wheelEvent(QWheelEvent *e)
{ e->ignore(); }

QSize TextEdit::sizeHint() const
{
	QFontMetrics fm(font());
	int h = fm.height();
	int w = fm.width(QLatin1Char('x'));
	return QSize(w, h);
}

void TextEdit::doColor()
{
	if (!c || !c->popup()->isVisible())
	{
		QString txt = " "+this->toPlainText()+" ";
		for (int i = 0; i < favorites.size(); i++)
		{ txt.replace(" "+favorites.at(i)+" ", " <span style=\"color:pink\">"+favorites.at(i)+"</span> "); }
		txt.replace(QRegExp(" ~([^ ]+)"), " <span style=\"color:green\">~\\1</span>");
		txt.replace(QRegExp(" -([^ ]+)"), " <span style=\"color:red\">-\\1</span>");
		txt.replace(QRegExp(" (user|fav|md5|rating|source|status|approver|unlocked|sub|id|width|height|score|mpixels|filesize|date|gentags|arttags|chartags|copytags|status|status|approver|order|parent):([^ ]*)"), " <span style=\"color:brown\">\\1:\\2</span>");
		QTextCursor crsr = textCursor();
		int pos = crsr.columnNumber(), lengh = crsr.selectionEnd()-crsr.selectionStart();
		setHtml(txt.mid(1, txt.length()-2));
		crsr.setPosition(pos-lengh, QTextCursor::MoveAnchor);
		crsr.setPosition(pos, QTextCursor::KeepAnchor);
		setTextCursor(crsr);
	}
}

void TextEdit::customContextMenuRequested(QPoint)
{
	QMenu *menu = new QMenu(this);
		QMenu *favs = new QMenu(tr("Favoris"), menu);
			QActionGroup* favsGroup = new QActionGroup(favs);
				favsGroup->setExclusive(true);
				connect(favsGroup, SIGNAL(triggered(QAction *)), this, SLOT(insertFav(QAction *)));
				for (int i = 0; i < favorites.count(); i++)
				{ favsGroup->addAction(favorites.at(i)); }
				favs->addActions(favsGroup->actions());
				favs->setIcon(QIcon(":/images/icons/favorite.png"));
			menu->addMenu(favs);
		QMenu *ratings = new QMenu(tr("Classes"), menu);
			QActionGroup* ratingsGroup = new QActionGroup(favs);
				ratingsGroup->setExclusive(true);
				connect(ratingsGroup, SIGNAL(triggered(QAction *)), this, SLOT(insertFav(QAction *)));
					ratingsGroup->addAction(QIcon(":/images/ratings/safe.png"), "rating:safe");
					ratingsGroup->addAction(QIcon(":/images/ratings/questionable.png"), "rating:questionable");
					ratingsGroup->addAction(QIcon(":/images/ratings/explicit.png"), "rating:explicit");
				ratings->addActions(ratingsGroup->actions());
				ratings->setIcon(QIcon(":/images/ratings/none.png"));
			menu->addMenu(ratings);
	menu->exec(QCursor::pos());
}
void TextEdit::insertFav(QAction *act)
{
	QString fav = act->text();
	QTextCursor cursor = this->textCursor();
	int pos = cursor.columnNumber();
	QString txt = this->toPlainText();
	if(!cursor.hasSelection())
	{ this->setPlainText(txt.mid(0, pos)+fav+txt.mid(pos)); }
	else
	{ this->setPlainText(txt.mid(0, cursor.selectionStart())+fav+txt.mid(cursor.selectionEnd())); }
	cursor.clearSelection();
	cursor.setPosition(pos+fav.length(), QTextCursor::KeepAnchor);
	this->setTextCursor(cursor);
	this->doColor();
}
