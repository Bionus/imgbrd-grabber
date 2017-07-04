#include "tag-context-menu.h"
#include <QProcess>
#include <QDesktopServices>
#include <QApplication>
#include <QClipboard>


TagContextMenu::TagContextMenu(QString tag, QList<Tag> allTags, QUrl browserUrl, Profile *profile, bool setImage, QWidget *parent)
	: QMenu(parent), m_tag(tag), m_allTags(allTags), m_browserUrl(browserUrl), m_profile(profile)
{
	// Favorites
	if (profile->getFavorites().contains(Favorite(m_tag)))
	{
		addAction(QIcon(":/images/icons/remove.png"), tr("Remove from favorites"), this, SLOT(unfavorite()));
		if (setImage)
		{ addAction(QIcon(":/images/icons/save.png"), tr("Choose as image"), this, SLOT(setfavorite())); }
	}
	else
	{ addAction(QIcon(":/images/icons/add.png"), tr("Add to favorites"), this, SLOT(favorite())); }

	// Keep for later
	if (profile->getKeptForLater().contains(m_tag, Qt::CaseInsensitive))
	{ addAction(QIcon(":/images/icons/remove.png"), tr("Don't keep for later"), this, SLOT(unviewitlater())); }
	else
	{ addAction(QIcon(":/images/icons/add.png"), tr("Keep for later"), this, SLOT(viewitlater())); }

	// Blacklist
	QStringList blacklistedTags = profile->getSettings()->value("blacklistedtags").toString().split(' ');
	if (blacklistedTags.contains(m_tag))
	{ addAction(QIcon(":/images/icons/eye-plus.png"), tr("Don't blacklist"), this, SLOT(unblacklist())); }
	else
	{ addAction(QIcon(":/images/icons/eye-minus.png"), tr("Blacklist"), this, SLOT(blacklist())); }

	// Ignore
	if (profile->getIgnored().contains(m_tag, Qt::CaseInsensitive))
	{ addAction(QIcon(":/images/icons/eye-plus.png"), tr("Don't ignore"), this, SLOT(unignore())); }
	else
	{ addAction(QIcon(":/images/icons/eye-minus.png"), tr("Ignore"), this, SLOT(ignore())); }
	addSeparator();

	// Copy
	addAction(QIcon(":/images/icons/copy.png"), tr("Copy tag"), this, SLOT(copyTagToClipboard()));
	if (!allTags.isEmpty())
	{ addAction(QIcon(":/images/icons/copy.png"), tr("Copy all tags"), this, SLOT(copyAllTagsToClipboard())); }
	addSeparator();

	// Tabs
	addAction(QIcon(":/images/icons/tab-plus.png"), tr("Open in a new tab"), this, SLOT(openInNewTab()));
	addAction(QIcon(":/images/icons/window.png"), tr("Open in new a window"), this, SLOT(openInNewWindow()));
	if (!browserUrl.isEmpty())
	{ addAction(QIcon(":/images/icons/browser.png"), tr("Open in browser"), this, SLOT(openInBrowser())); }
}

void TagContextMenu::favorite()
{
	Favorite fav(m_tag);
	m_profile->addFavorite(fav);
	emit setFavoriteImage();
}
void TagContextMenu::setfavorite()
{
	emit setFavoriteImage();
}
void TagContextMenu::unfavorite()
{
	m_profile->removeFavorite(Favorite(m_tag));
}

void TagContextMenu::viewitlater()
{
	m_profile->addKeptForLater(m_tag);
}
void TagContextMenu::unviewitlater()
{
	m_profile->removeKeptForLater(m_tag);
}

void TagContextMenu::ignore()
{
	m_profile->addIgnored(m_tag);
}
void TagContextMenu::unignore()
{
	m_profile->removeIgnored(m_tag);
}

void TagContextMenu::blacklist()
{
	QString blacklistedTags = m_profile->getSettings()->value("blacklistedtags").toString();
	blacklistedTags += " " + m_tag;
	m_profile->getSettings()->setValue("blacklistedtags", blacklistedTags);
	//colore();
}
void TagContextMenu::unblacklist()
{
	QStringList blacklistedTags = m_profile->getSettings()->value("blacklistedtags").toString().split(' ');
	blacklistedTags.removeAll(m_tag);
	m_profile->getSettings()->setValue("blacklistedtags", blacklistedTags.join(' '));
	//colore();
}

void TagContextMenu::openInNewTab()
{
	emit openNewTab();
}
void TagContextMenu::openInNewWindow()
{
	QProcess myProcess;
	myProcess.startDetached(qApp->arguments().at(0), QStringList(m_tag));
}
void TagContextMenu::openInBrowser()
{
	QDesktopServices::openUrl(m_browserUrl);
}
void TagContextMenu::copyTagToClipboard()
{
	QApplication::clipboard()->setText(m_tag);
}
void TagContextMenu::copyAllTagsToClipboard()
{
	QStringList tags;
	for (Tag tag : m_allTags)
		tags.append(tag.text());

	QApplication::clipboard()->setText(tags.join(' '));
}
