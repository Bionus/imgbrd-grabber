#include "text-edit.h"
#include <QAbstractItemView>
#include <QActionGroup>
#include <QApplication>
#include <QCompleter>
#include <QMenu>
#include <QMimeData>
#include <QScrollBar>
#include <QSettings>
#include <QTextDocumentFragment>
#include <QWheelEvent>
#include "functions.h"
#include "models/profile.h"
#include "search-syntax-highlighter.h"


TextEdit::TextEdit(Profile *profile, QWidget *parent)
	: QTextEdit(parent), c(nullptr), m_profile(profile), m_favorites(profile->getFavorites()), m_viewItLater(profile->getKeptForLater())
{
	setTabChangesFocus(true);
	setWordWrapMode(QTextOption::NoWrap);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	setFixedHeight(sizeHint().height());
	setContextMenuPolicy(Qt::CustomContextMenu);
	setAcceptRichText(false);
	connect(this, &QTextEdit::customContextMenuRequested, this, &TextEdit::openCustomContextMenu);
	new SearchSyntaxHighlighter(true, document(), m_profile);
}

QSize TextEdit::sizeHint() const
{
	ensurePolished();

	const QFontMetrics fm = fontMetrics();
	const int h = qMax(fm.height(), 14) + 4;
	const int w = fm.horizontalAdvance(QLatin1Char('x')) * 17 + 4;

	QStyleOptionFrame opt;
	opt.initFrom(this);
	return (style()->sizeFromContents(QStyle::CT_LineEdit, &opt, QSize(w, h), this));
}

/**
 * Ignore all wheel events on the field.
 * @param e The Qt event.
 */
void TextEdit::wheelEvent(QWheelEvent *e)
{
	e->ignore();
}

/**
 * Remove line breaks when pasting text into the field.
 */
void TextEdit::insertFromMimeData(const QMimeData *source)
{
	QString text = source->text();
	text.replace("\r\n", " ");
	text.replace("\r", " ");
	text.replace("\n", " ");

	textCursor().insertText(text);
}

void TextEdit::setCompleter(QCompleter *completer)
{
	if (completer == nullptr) {
		return;
	}

	// Disconnect the previous completer
	if (c != nullptr) {
		QObject::disconnect(c, nullptr, this, nullptr);
	}

	// Set the new completer and connect it to the field
	c = completer;
	c->setWidget(this);
	c->setCompletionMode(QCompleter::PopupCompletion);
	c->setCaseSensitivity(Qt::CaseInsensitive);
	QObject::connect(c, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));
}

QCompleter *TextEdit::completer() const
{
	return c;
}

void TextEdit::insertCompletion(const QString &completion)
{
	if (c->widget() != this) {
		return;
	}

	QTextCursor tc = textCursor();
	const int extra = completion.length() - c->completionPrefix().length();
	tc.movePosition(QTextCursor::Left);
	tc.movePosition(QTextCursor::EndOfWord);
	tc.insertText(completion.right(extra));
	setTextCursor(tc);
}

QString TextEdit::textUnderCursor() const
{
	QTextCursor tc = textCursor();
	QString txt = ' ' + toPlainText() + ' ';
	const int pos = tc.position();
	const int i2 = txt.indexOf(' ', pos);
	int i1 = txt.lastIndexOf(' ', i2 - 1) + 1;

	// Skip - and ~ prefixes
	while (i1 < i2 && (txt[i1] == '-' || txt[i1] == '~')) {
		i1++;
	}

	return txt.mid(i1, i2 - i1);
}

void TextEdit::focusInEvent(QFocusEvent *e)
{
	if (c != nullptr) {
		c->setWidget(this);
	}

	QTextEdit::focusInEvent(e);
}

void TextEdit::keyPressEvent(QKeyEvent *e)
{
	if (c != nullptr && c->popup()->isVisible()) {
		// The following keys are forwarded by the completer to the widget
		QString curr = c->popup()->currentIndex().data().toString(), under = textUnderCursor();
		switch (e->key())
		{
			case Qt::Key_Enter:
			case Qt::Key_Return:
				c->popup()->hide();
				if (curr.isEmpty() || under == curr) {
					emit returnPressed();
				} else {
					insertCompletion(curr);
				}
				return;

			case Qt::Key_Escape:
			case Qt::Key_Tab:
			case Qt::Key_Backtab:
				e->ignore();
				return;
		}
	}

	const bool isShortcut = (e->modifiers().testFlag(Qt::ControlModifier) && e->key() == Qt::Key_Space); // CTRL+Space
	if (c == nullptr || !isShortcut) { // do not process the shortcut when we have a completer
		if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) {
			emit returnPressed();
			return;
		}
		QTextEdit::keyPressEvent(e);
	}

	const bool ctrlOrShift = e->modifiers().testFlag(Qt::ControlModifier) || e->modifiers().testFlag(Qt::ShiftModifier);
	if (c == nullptr || (ctrlOrShift && e->text().isEmpty())) {
		return;
	}

	static QString eow(" ");
	const bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
	QString completionPrefix = textUnderCursor();

	if (!isShortcut && (hasModifier || e->text().isEmpty() || completionPrefix.length() < 3 || eow.contains(e->text().right(1)))) {
		c->popup()->hide();
		return;
	}

	if (completionPrefix != c->completionPrefix()) {
		c->setCompletionPrefix(completionPrefix);
	}

	QRect cr = cursorRect();
	cr.setWidth(c->popup()->sizeHintForColumn(0) + c->popup()->verticalScrollBar()->sizeHint().width());
	c->complete(cr);
}

void TextEdit::openCustomContextMenu(const QPoint &pos)
{
	Q_UNUSED(pos)

	const QString text = toPlainText().trimmed();

	auto *menu = new QMenu(this);
		auto *favs = new QMenu(tr("Favorites"), menu);
			auto *favsGroup = new QActionGroup(favs);
				favsGroup->setExclusive(true);
				connect(favsGroup, &QActionGroup::triggered, this, &TextEdit::insertFav);
				for (const Favorite &fav : qAsConst(m_favorites)) {
					favsGroup->addAction(fav.getName());
				}
				if (!text.isEmpty()) {
					if (m_favorites.contains(Favorite(text))) {
						favs->addAction(QIcon(":/images/icons/remove.png"), tr("Remove"), this, SLOT(unsetFavorite()));
					} else {
						favs->addAction(QIcon(":/images/icons/add.png"), tr("Add"), this, SLOT(setFavorite()));
					}
					favs->addSeparator();
				}
				favs->addActions(favsGroup->actions());
				favs->setIcon(QIcon(":/images/icons/favorite.png"));
				favs->setStyleSheet("* { menu-scrollable: 1 }");
			menu->addMenu(favs);
		auto *vils = new QMenu(tr("Kept for later"), menu);
			auto *vilsGroup = new QActionGroup(vils);
				vilsGroup->setExclusive(true);
				connect(vilsGroup, &QActionGroup::triggered, this, &TextEdit::insertFav);
				for (const QString &viewItLater : qAsConst(m_viewItLater)) {
					vilsGroup->addAction(viewItLater);
				}
				if (!text.isEmpty()) {
					if (m_viewItLater.contains(text)) {
						vils->addAction(QIcon(":/images/icons/remove.png"), tr("Remove"), this, SLOT(unsetKfl()));
					} else {
						vils->addAction(QIcon(":/images/icons/add.png"), tr("Add"), this, SLOT(setKfl()));
					}
					vils->addSeparator();
				}
				vils->addActions(vilsGroup->actions());
				vils->setIcon(QIcon(":/images/icons/book.png"));
			menu->addMenu(vils);
		auto *ratings = new QMenu(tr("Ratings"), menu);
			auto *ratingsGroup = new QActionGroup(favs);
				ratingsGroup->setExclusive(true);
				connect(ratingsGroup, &QActionGroup::triggered, this, &TextEdit::insertFav);
					ratingsGroup->addAction(QIcon(":/images/ratings/general.png"), "rating:general");
					ratingsGroup->addAction(QIcon(":/images/ratings/safe.png"), "rating:safe");
					ratingsGroup->addAction(QIcon(":/images/ratings/questionable.png"), "rating:questionable");
					ratingsGroup->addAction(QIcon(":/images/ratings/explicit.png"), "rating:explicit");
				ratings->addActions(ratingsGroup->actions());
				ratings->setIcon(QIcon(":/images/ratings/none.png"));
			menu->addMenu(ratings);
		auto *sortings = new QMenu(tr("Sortings"), menu);
			auto *sortingsGroup = new QActionGroup(favs);
				sortingsGroup->setExclusive(true);
				connect(sortingsGroup, &QActionGroup::triggered, this, &TextEdit::insertFav);
					sortingsGroup->addAction(QIcon(":/images/sortings/change.png"), "order:change");
					sortingsGroup->addAction(QIcon(":/images/sortings/change.png"), "order:change_desc");
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
			if (!textCursor().selection().isEmpty()) {
				menu->addAction(tr("Copy"), this, SLOT(copy()), QKeySequence::Copy);
				menu->addAction(tr("Cut"), this, SLOT(cut()), QKeySequence::Cut);
			}
			menu->addAction(tr("Paste"), this, SLOT(paste()), QKeySequence::Paste);
	menu->exec(QCursor::pos());
}

void TextEdit::setFavorite()
{
	const QString text = toPlainText().trimmed();
	m_profile->addFavorite(Favorite(text));
	emit addedFavorite(text);
}
void TextEdit::unsetFavorite()
{
	m_profile->removeFavorite(Favorite(toPlainText().trimmed()));
}
void TextEdit::setKfl()
{
	m_profile->addKeptForLater(toPlainText().trimmed());
}
void TextEdit::unsetKfl()
{
	m_profile->removeKeptForLater(toPlainText().trimmed());
}

void TextEdit::insertFav(QAction *act)
{
	QString text = act->text();

	QTextCursor cursor = this->textCursor();
	int pos = cursor.columnNumber();

	QString txt = this->toPlainText();
	if (!cursor.hasSelection()) {
		if (pos == 0 && txt.count() != 0 && txt[0] != ' ') {
			text.append(' ');
		}
		if (pos == txt.count() && txt.count() != 0 && txt[txt.count() - 1] != ' ') {
			text.prepend(' ');
		}
		this->setPlainText(txt.mid(0, pos) + text + txt.mid(pos));
	} else {
		this->setPlainText(txt.mid(0, cursor.selectionStart()) + text + txt.mid(cursor.selectionEnd()));
	}

	cursor.clearSelection();
	cursor.setPosition(pos + text.length(), QTextCursor::KeepAnchor);

	this->setTextCursor(cursor);
}
