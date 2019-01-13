#include "tag-context-menu.h"
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QProcess>
#include "functions.h"
#include "models/profile.h"
#include "tags/tag.h"


TagContextMenu::TagContextMenu(QString tag, QList<Tag> allTags, QUrl browserUrl, Profile *profile, bool setImage, QWidget *parent)
	: QMenu(parent), m_tag(std::move(tag)), m_allTags(std::move(allTags)), m_browserUrl(std::move(browserUrl)), m_profile(profile)
{
	// Favorites
	if (profile->getFavorites().contains(Favorite(m_tag))) {
		addAction(QIcon(":/images/icons/remove.png"), tr("Remove from favorites"), this, SLOT(unfavorite()));
		if (setImage) {
			addAction(QIcon(":/images/icons/save.png"), tr("Choose as image"), this, SLOT(setfavorite()));
		}
	} else {
		addAction(QIcon(":/images/icons/add.png"), tr("Add to favorites"), this, SLOT(favorite()));
	}

	// Keep for later
	if (profile->getKeptForLater().contains(m_tag, Qt::CaseInsensitive)) {
		addAction(QIcon(":/images/icons/remove.png"), tr("Don't keep for later"), this, SLOT(unviewitlater()));
	} else {
		addAction(QIcon(":/images/icons/add.png"), tr("Keep for later"), this, SLOT(viewitlater()));
	}

	// Blacklist
	if (profile->getBlacklist().contains(m_tag)) {
		addAction(QIcon(":/images/icons/eye-plus.png"), tr("Don't blacklist"), this, SLOT(unblacklist()));
	} else {
		addAction(QIcon(":/images/icons/eye-minus.png"), tr("Blacklist"), this, SLOT(blacklist()));
	}

	// Ignore
	if (profile->getIgnored().contains(m_tag, Qt::CaseInsensitive)) {
		addAction(QIcon(":/images/icons/eye-plus.png"), tr("Don't ignore"), this, SLOT(unignore()));
	} else {
		addAction(QIcon(":/images/icons/eye-minus.png"), tr("Ignore"), this, SLOT(ignore()));
	}
	addSeparator();

	// Copy
	addAction(QIcon(":/images/icons/copy.png"), tr("Copy tag"), this, SLOT(copyTagToClipboard()));
	if (!allTags.isEmpty()) {
		addAction(QIcon(":/images/icons/copy.png"), tr("Copy all tags"), this, SLOT(copyAllTagsToClipboard()));
	}
	addSeparator();

	// Tabs
	addAction(QIcon(":/images/icons/tab-plus.png"), tr("Open in a new tab"), this, SLOT(openInNewTab()));
	addAction(QIcon(":/images/icons/window.png"), tr("Open in new a window"), this, SLOT(openInNewWindow()));
	if (!browserUrl.isEmpty()) {
		addAction(QIcon(":/images/icons/browser.png"), tr("Open in browser"), this, SLOT(openInBrowser()));
	}
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
	m_profile->addBlacklistedTag(m_tag);
}
void TagContextMenu::unblacklist()
{
	m_profile->removeBlacklistedTag(m_tag);
}

void TagContextMenu::openInNewTab()
{
	emit openNewTab();
}
void TagContextMenu::openInNewWindow()
{
	QProcess::startDetached(qApp->arguments().at(0), QStringList(m_tag));
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
	tags.reserve(m_allTags.count());
	for (const Tag &tag : qAsConst(m_allTags)) {
		tags.append(tag.text());
	}

	QApplication::clipboard()->setText(tags.join(' '));
}
