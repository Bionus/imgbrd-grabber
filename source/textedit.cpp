#include <QApplication>
#include "textedit.h"
#include "functions.h"



TextEdit::TextEdit(QStringList favs, QWidget *parent) : QTextEdit(parent), c(0), m_favorites(favs)
{
	setTabChangesFocus(true);
	setWordWrapMode(QTextOption::NoWrap);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	setFixedHeight(sizeHint().height());
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenuRequested(QPoint)));
}

TextEdit::~TextEdit()
{
}

QSize TextEdit::sizeHint() const
{
	QFontMetrics fm(font());
	int h = qMax(fm.height(), 14) + 4;
	int w = fm.width(QLatin1Char('x')) * 17 + 4;
	QStyleOptionFrameV2 opt;
	opt.initFrom(this);
	return (style()->sizeFromContents(QStyle::CT_LineEdit, &opt, QSize(w, h).expandedTo(QApplication::globalStrut()), this));
}

void TextEdit::wheelEvent(QWheelEvent *e)
{ e->ignore(); }

void TextEdit::doColor()
{
	QString txt = Qt::escape(" "+this->toPlainText()+" ");
	for (int i = 0; i < m_favorites.size(); i++)
	{ txt.replace(" "+m_favorites.at(i)+" ", " <span style=\"color:#ffc0cb\">"+m_favorites.at(i)+"</span> "); }
	QRegExp r1(" ~([^ ]+)"), r2(" -([^ ]+)"), r3(" (user|fav|md5|pool|rating|source|status|approver|unlocked|sub|id|width|height|score|mpixels|filesize|date|gentags|arttags|chartags|copytags|status|status|approver|order|parent):([^ ]*)");
	int pos = 0;
	while ((pos = r1.indexIn(txt, pos)) != -1)
	{
		QString rep = " <span style=\"color:green\">~"+r1.cap(1)+"</span>";
		txt.replace(r1.cap(0), rep);
		pos += rep.length();
	}
	pos = 0;
	while ((pos = r2.indexIn(txt, pos)) != -1)
	{
		QString rep = " <span style=\"color:red\">-"+r2.cap(1)+"</span>";
		txt.replace(r2.cap(0), rep);
		pos += rep.length();
	}
	pos = 0;
	while ((pos = r3.indexIn(txt, pos)) != -1)
	{
		QString rep = " <span style=\"color:brown\">"+r3.cap(1)+":"+r3.cap(2)+"</span>";
		txt.replace(r3.cap(0), rep);
		pos += rep.length();
	}
	QTextCursor crsr = textCursor();
	pos = crsr.columnNumber();
	int lengh = crsr.selectionEnd()-crsr.selectionStart();
	setHtml(txt.mid(1, txt.length()-2));
	crsr.setPosition(pos-lengh, QTextCursor::MoveAnchor);
	crsr.setPosition(pos, QTextCursor::KeepAnchor);
	setTextCursor(crsr);
}
void TextEdit::setText(const QString &text)
{
	QTextEdit::setText(text);
	doColor();
}

void TextEdit::setCompleter(QCompleter *completer)
{
	if (c)
	{ QObject::disconnect(c, 0, this, 0); }

	c = completer;

	if (!c)
	{ return; }

	c->setWidget(this);
	c->setCompletionMode(QCompleter::PopupCompletion);
	c->setCaseSensitivity(Qt::CaseInsensitive);
	QObject::connect(c, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));
}

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
}

QString TextEdit::textUnderCursor() const
{
	QTextCursor tc = textCursor();
	tc.select(QTextCursor::WordUnderCursor);
	return tc.selectedText();
}

void TextEdit::focusInEvent(QFocusEvent *e)
{
	if (c)
	{ c->setWidget(this); }
	QTextEdit::focusInEvent(e);
}

void TextEdit::keyPressEvent(QKeyEvent *e)
{
	if (c && c->popup()->isVisible())
	{
		// The following keys are forwarded by the completer to the widget
	   switch (e->key()) {
	   case Qt::Key_Enter:
	   case Qt::Key_Return:
	   case Qt::Key_Escape:
	   case Qt::Key_Tab:
	   case Qt::Key_Backtab:
			e->ignore();
			return; // let the completer do default behavior
	   default:
		   break;
	   }
	}

	bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
	if (!c || !isShortcut) // do not process the shortcut when we have a completer
	{
		if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
		{
			emit returnPressed();
			return;
		}
		QTextEdit::keyPressEvent(e);
	}
	doColor();

	const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
	if (!c || (ctrlOrShift && e->text().isEmpty()))
	{ return; }

	static QString eow(""); // end of word
	bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
	QString completionPrefix = textUnderCursor();

	if (!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 3 || eow.contains(e->text().right(1))))
	{
		c->popup()->hide();
		return;
	}

	if (completionPrefix != c->completionPrefix())
	{
		c->setCompletionPrefix(completionPrefix);
		c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
	}
	QRect cr = cursorRect();
	cr.setWidth(c->popup()->sizeHintForColumn(0) + c->popup()->verticalScrollBar()->sizeHint().width());
	c->complete(cr);
}

void TextEdit::customContextMenuRequested(const QPoint &pos)
{
	QMenu *menu = new QMenu(this);
		QMenu *favs = new QMenu(tr("Favoris"), menu);
			QActionGroup* favsGroup = new QActionGroup(favs);
				favsGroup->setExclusive(true);
				connect(favsGroup, SIGNAL(triggered(QAction *)), this, SLOT(insertFav(QAction *)));
				for (int i = 0; i < m_favorites.count(); i++)
				{ favsGroup->addAction(m_favorites.at(i)); }
				favs->addActions(favsGroup->actions());
				favs->addSeparator();
				if (m_favorites.contains(toPlainText()))
				{ favs->addAction(tr("Retirer"), this, SLOT(unsetFavorite())); }
				else
				{ favs->addAction(tr("Ajouter"), this, SLOT(setFavorite())); }
				favs->setIcon(QIcon(":/images/icons/favorite.png"));
			menu->addMenu(favs);
		QMenu *vils = new QMenu(tr("Gardés pour plus tard"), menu);
			QActionGroup* vilsGroup = new QActionGroup(vils);
				vilsGroup->setExclusive(true);
				connect(vilsGroup, SIGNAL(triggered(QAction *)), this, SLOT(insertFav(QAction *)));
				QStringList viewitlater = loadViewItLater();
				for (int i = 0; i < viewitlater.count(); i++)
				{ vilsGroup->addAction(viewitlater.at(i)); }
				vils->addActions(vilsGroup->actions());
				vils->addSeparator();
				if (viewitlater.contains(toPlainText()))
				{ vils->addAction(tr("Retirer"), this, SLOT(unsetKfl())); }
				else
				{ vils->addAction(tr("Ajouter"), this, SLOT(setKfl())); }
				vils->setIcon(QIcon(":/images/icons/book.png"));
			menu->addMenu(vils);
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
		QMenu *sortings = new QMenu(tr("Tris"), menu);
			QActionGroup* sortingsGroup = new QActionGroup(favs);
				sortingsGroup->setExclusive(true);
				connect(sortingsGroup, SIGNAL(triggered(QAction *)), this, SLOT(insertFav(QAction *)));
					sortingsGroup->addAction(QIcon(":/images/icons/favorite.png"), "order:favcount");
					sortingsGroup->addAction(QIcon(":/images/sortings/size.png"), "order:filesize");
					sortingsGroup->addAction(QIcon(":/images/sortings/id.png"), "order:id");
					sortingsGroup->addAction(QIcon(":/images/sortings/id.png"), "order:id_desc");
					sortingsGroup->addAction(QIcon(":/images/sortings/landscape.png"), "order:landscape");
					sortingsGroup->addAction(QIcon(":/images/sortings/pixels.png"), "order:mpixels");
					sortingsGroup->addAction(QIcon(":/images/sortings/pixels.png"), "order:mpixels_asc");
					sortingsGroup->addAction(QIcon(":/images/sortings/portrait.png"), "order:portrait");
					sortingsGroup->addAction(QIcon(":/images/icons/favorite.png"), "order:rank");
					sortingsGroup->addAction(QIcon(":/images/sortings/score.png"), "order:score");
					sortingsGroup->addAction(QIcon(":/images/sortings/score.png"), "order:score_asc");
				sortings->addActions(sortingsGroup->actions());
				sortings->setIcon(QIcon(":/images/sortings/sort.png"));
			menu->addMenu(sortings);
		menu->addSeparator();
			if (!textCursor().selection().isEmpty())
			{
				menu->addAction(tr("Copier"), this, SLOT(copy()), QKeySequence::Copy);
				menu->addAction(tr("Couper"), this, SLOT(cut()), QKeySequence::Cut);
			}
			menu->addAction(tr("Coller"), this, SLOT(paste()), QKeySequence::Paste);
	menu->exec(QCursor::pos());
}
void TextEdit::setFavorite()
{
	m_favorites.append(toPlainText());
	m_favorites.sort();

	QFile f(savePath("favorites.txt"));
		f.open(QIODevice::WriteOnly | QIODevice::Append);
		f.write(QString(toPlainText()+"|50|"+QDateTime::currentDateTime().toString(Qt::ISODate)+"\r\n").toAscii());
	f.close();

	emit favoritesChanged();
}
void TextEdit::unsetFavorite()
{
	m_favorites.removeAll(toPlainText());
	QFile f(savePath("favorites.txt"));
	f.open(QIODevice::ReadOnly);
		QString favs = f.readAll();
	f.close();
	favs.replace("\r\n", "\n").replace("\r", "\n").replace("\n", "\r\n");
	QRegExp reg(toPlainText()+"\\|(.+)\\r\\n");
	reg.setMinimal(true);
	favs.remove(reg);
	f.open(QIODevice::WriteOnly);
		f.write(favs.toAscii());
	f.close();
	if (QFile::exists(savePath("thumbs/"+toPlainText()+".png")))
	{ QFile::remove(savePath("thumbs/"+toPlainText()+".png")); }

	emit favoritesChanged();
}
void TextEdit::setKfl()
{
	QStringList viewitlater = loadViewItLater();
	viewitlater.append(toPlainText());

	QFile f(savePath("viewitlater.txt"));
	f.open(QIODevice::WriteOnly);
		f.write(viewitlater.join("\r\n").toAscii());
	f.close();

	emit kflChanged();
}
void TextEdit::unsetKfl()
{
	QStringList viewitlater = loadViewItLater();
	viewitlater.removeAll(toPlainText());

	QFile f(savePath("viewitlater.txt"));
	f.open(QIODevice::WriteOnly);
		f.write(viewitlater.join("\r\n").toAscii());
	f.close();

	emit kflChanged();
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
