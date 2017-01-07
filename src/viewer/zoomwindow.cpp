#include <QApplication>
#include <QtNetwork>
#include <QMessageBox>
#include <QShortcut>
#include <QMenu>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMessageBox>
#include "functions.h"
#include "settings/optionswindow.h"
#include "ui/QAffiche.h"
#include "zoomwindow.h"
#include "imagethread.h"
#include "ui_zoomwindow.h"
#include "models/filename.h"
#include "functions.h"

#include <QScrollBar>



zoomWindow::zoomWindow(QList<QSharedPointer<Image> > images, QSharedPointer<Image> image, Site *site, QMap<QString,Site*> *sites, Profile *profile, mainWindow *parent)
	: QDialog(Q_NULLPTR, Qt::Window), m_parent(parent), m_profile(profile), m_favorites(profile->getFavorites()), m_viewItLater(profile->getKeptForLater()), m_ignore(profile->getIgnored()), m_settings(profile->getSettings()), ui(new Ui::zoomWindow), m_site(site), timeout(300), m_loaded(false), m_loadedImage(false), m_loadedDetails(false), image(nullptr), movie(nullptr), m_reply(nullptr), m_finished(false), m_thread(false), m_data(QByteArray()), m_size(0), m_sites(sites), m_source(), m_th(nullptr), m_fullScreen(nullptr), m_images(images), m_isFullscreen(false), m_isSlideshowRunning(false), m_imagePath("")
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);

	m_mustSave = 0;

	restoreGeometry(m_settings->value("Zoom/geometry").toByteArray());
	ui->buttonPlus->setChecked(m_settings->value("Zoom/plus", false).toBool());
	ui->progressBarDownload->hide();

	QShortcut *escape = new QShortcut(QKeySequence(Qt::Key_Escape), this);
		connect(escape, &QShortcut::activated, this, &zoomWindow::close);
	QShortcut *save = new QShortcut(QKeySequence::Save, this);
		connect(save, SIGNAL(activated()), this, SLOT(saveImage()));
	QShortcut *saveAs = new QShortcut(QKeySequence::SaveAs, this);
		connect(saveAs, &QShortcut::activated, this, &zoomWindow::saveImageAs);
	QShortcut *arrowNext = new QShortcut(QKeySequence(Qt::Key_Right), this);
		connect(arrowNext, &QShortcut::activated, this, &zoomWindow::next);
	QShortcut *arrowPrevious = new QShortcut(QKeySequence(Qt::Key_Left), this);
		connect(arrowPrevious, &QShortcut::activated, this, &zoomWindow::previous);
	QShortcut *copyImageFile = new QShortcut(QKeySequence::Copy, this);
		connect(copyImageFile, &QShortcut::activated, this, &zoomWindow::copyImageDataToClipboard);
	QShortcut *toggleFullscreen = new QShortcut(QKeySequence::FullScreen, this);
		connect(toggleFullscreen, &QShortcut::activated, this, &zoomWindow::toggleFullScreen);

	m_labelTagsLeft = new QAffiche(QVariant(), 0, QColor(), this);
		m_labelTagsLeft->setContextMenuPolicy(Qt::CustomContextMenu);
		m_labelTagsLeft->setTextInteractionFlags(Qt::TextBrowserInteraction);
		connect(m_labelTagsLeft, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
		connect(m_labelTagsLeft, SIGNAL(linkActivated(QString)), this, SLOT(openUrl(QString)));
		connect(m_labelTagsLeft, SIGNAL(linkHovered(QString)), this, SLOT(linkHovered(QString)));
		connect(m_labelTagsLeft, SIGNAL(middleClicked()), this, SLOT(openInNewTab()));
		ui->scrollAreaWidgetContents->layout()->addWidget(m_labelTagsLeft);

	m_labelTagsTop = new QAffiche(QVariant(), 0, QColor(), this);
		m_labelTagsTop->setWordWrap(true);
		m_labelTagsTop->setContextMenuPolicy(Qt::CustomContextMenu);
		m_labelTagsTop->setTextInteractionFlags(Qt::TextBrowserInteraction);
		connect(m_labelTagsTop, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
		connect(m_labelTagsTop, SIGNAL(linkActivated(QString)), this, SLOT(openUrl(QString)));
		connect(m_labelTagsTop, SIGNAL(linkHovered(QString)), this, SLOT(linkHovered(QString)));
		connect(m_labelTagsTop, SIGNAL(middleClicked()), this, SLOT(openInNewTab()));
		ui->verticalLayout->insertWidget(0, m_labelTagsTop, 0);

	// Automatically re-color if some settings change
	connect(m_profile, &Profile::favoritesChanged, this, &zoomWindow::colore);
	connect(m_profile, &Profile::keptForLaterChanged, this, &zoomWindow::colore);
	connect(m_profile, &Profile::ignoredChanged, this, &zoomWindow::colore);

	m_stackedWidget = new QStackedWidget(this);
		ui->verticalLayout->insertWidget(1, m_stackedWidget, 1);
	m_labelImage = new QAffiche(QVariant(), 0, QColor(), this);
		m_labelImage->setSizePolicy(QSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored));
		connect(m_labelImage, SIGNAL(doubleClicked()), this, SLOT(openFile()));
		m_stackedWidget->addWidget(m_labelImage);

	connect(ui->buttonDetails, SIGNAL(clicked()), this, SLOT(showDetails()));
	connect(ui->buttonPlus, &QPushButton::toggled, this, &zoomWindow::updateButtonPlus);

	m_labelImage->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_labelImage, &QAffiche::customContextMenuRequested, this, &zoomWindow::imageContextMenu);

	m_slideshow.setSingleShot(true);
	connect(&m_slideshow, &QTimer::timeout, this, &zoomWindow::next);

	load(image);
}
void zoomWindow::go()
{
	ui->labelPools->hide();
	bool whitelisted = false;
	if (!m_settings->value("whitelistedtags").toString().isEmpty())
	{
		QStringList whitelist = m_settings->value("whitelistedtags").toString().split(" ");
		for (Tag t : m_image->tags())
		{
			if (whitelist.contains(t.text()))
			{
				whitelisted = true;
				break;
			}
		}
	}
	if (m_settings->value("autodownload", false).toBool() || (whitelisted && m_settings->value("whitelist_download", "image").toString() == "image"))
	{ saveImage(); }

	m_url = m_image->getDisplayableUrl().toString();

	QTimer *timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()), this, SLOT(update()));
		timer->setSingleShot(true);
		m_resizeTimer = timer;

	QString u = m_site->value("Urls/Html/Post");
		u.replace("{id}", QString::number(m_image->id()));

	QString pos = m_settings->value("tagsposition", "top").toString();
	if (pos == "auto")
	{
		if (!m_image->size().isEmpty())
		{
			if (float(m_image->width())/float(m_image->height()) >= 4./3.)
			{ pos = "top"; }
			else
			{ pos = "left"; }
		}
		else
		{ pos = "top"; }
	}
	if (pos == "top")
	{
		ui->widgetLeft->hide();
		m_labelTagsTop->setText(m_image->stylishedTags(m_profile).join(" "));
	}
	else
	{
		m_labelTagsTop->hide();
		m_labelTagsLeft->setText(m_image->stylishedTags(m_profile).join("<br/>"));
		m_labelTagsLeft->setMinimumWidth(m_labelTagsLeft->sizeHint().width() + ui->scrollArea->verticalScrollBar()->sizeHint().width());
	}

	m_detailsWindow = new detailsWindow(m_profile, this);

	// Load image details (exact tags & co)
	connect(m_image.data(), &Image::finishedLoadingTags, this, &zoomWindow::replyFinishedDetails, Qt::UniqueConnection);
	m_image->loadDetails();

	if (!m_isFullscreen)
		activateWindow();
}

/**
 * Destructor of the zoomWindow class
 */
zoomWindow::~zoomWindow()
{
	if (image != nullptr)
		delete image;
	if (movie != nullptr)
		movie->deleteLater();

	m_labelTagsTop->deleteLater();
	m_labelTagsLeft->deleteLater();
	m_detailsWindow->deleteLater();

	delete ui;
}

void zoomWindow::imageContextMenu()
{
	QMenu *menu = new QMenu(this);

	// Copy actions
	menu->addAction(QIcon(":/images/icons/copy.png"), tr("Copy file"), this, SLOT(copyImageFileToClipboard()));
	menu->addAction(QIcon(":/images/icons/document-binary.png"), tr("Copy data"), this, SLOT(copyImageDataToClipboard()));
	menu->addSeparator();

	// Reverse search actions
	menu->addAction(QIcon(":/images/sources/saucenao.png"), tr("SauceNAO"), this, SLOT(reverseSearchSauceNao()));
	menu->addAction(QIcon(":/images/sources/iqdb.png"), tr("IQDB"), this, SLOT(reverseSearchIqdb()));
	menu->addAction(QIcon(":/images/sources/tineye.png"), tr("TinEye"), this, SLOT(reverseSearchTinEye()));

	menu->exec(QCursor::pos());
}
void zoomWindow::copyImageFileToClipboard()
{
	QString path = m_imagePath;
	if (path.isEmpty())
	{
		QMap<QString, Image::SaveResult> files = m_image->save(m_settings->value("Save/filename").toString(), QDir::tempPath());
		path = files.firstKey();
	}

	QMimeData* mimeData = new QMimeData();
	mimeData->setUrls({ QUrl::fromLocalFile(path) });
	QApplication::clipboard()->setMimeData(mimeData);
}
void zoomWindow::copyImageDataToClipboard()
{
	QApplication::clipboard()->setPixmap(*image);
}
void zoomWindow::reverseSearchSauceNao()
{
	QDesktopServices::openUrl(QUrl("https://saucenao.com/search.php?db=999&url=" + m_image->fileUrl().toEncoded()));
}
void zoomWindow::reverseSearchIqdb()
{
	QDesktopServices::openUrl(QUrl("https://iqdb.org/?url=" + m_image->fileUrl().toEncoded()));
}
void zoomWindow::reverseSearchTinEye()
{
	QDesktopServices::openUrl(QUrl("https://www.tineye.com/search/?url=" + m_image->fileUrl().toEncoded()));
}

void zoomWindow::showDetails()
{
	m_detailsWindow->setImage(m_image);
	m_detailsWindow->show();
}

void zoomWindow::openUrl(QString url)
{ emit linkClicked(url); }
void zoomWindow::openPool(QString url)
{
	if (url.startsWith("pool:"))
	{ emit poolClicked(url.right(url.length() - 5).toInt(), m_image->site()); }
	else
	{
		Page *p = new Page(m_profile, m_sites->value(m_image->site()), m_sites->values(), QStringList() << "id:"+url, 1, 1, QStringList(), false, this);
		connect(p, SIGNAL(finishedLoading(Page*)), this, SLOT(openPoolId(Page*)));
		p->load();
	}
}
void zoomWindow::openPoolId(Page *p)
{
	if (p->images().size() < 1)
	{
		p->deleteLater();
		return;
	}

	m_image = p->images().at(0);
	timeout = 300;
	m_loaded = false;
	m_loadedDetails = false;
	m_loadedImage = false;
	m_reply->deleteLater();
	m_reply = nullptr;
	m_finished = false;
	m_size = 0;
	m_labelImage->hide();
	ui->verticalLayout->removeWidget(m_labelImage);

	go();
}

void zoomWindow::openSaveDir(bool fav)
{
	QString path = m_settings->value("Save/path"+QString(fav ? "_favorites" : "")).toString().replace("\\", "/"), fn = m_settings->value("Save/filename"+QString(fav ? "_favorites" : "")).toString();

	if (path.right(1) == "/")
	{ path = path.left(path.length()-1); }
	path = QDir::toNativeSeparators(path);

	QStringList files = m_image->path(fn, path);
	QString file = files.empty() ? "" : files.at(0);
	QString pth = file.section(QDir::toNativeSeparators("/"), 0, -2);
	QString url = path+QDir::toNativeSeparators("/")+pth;

	QDir dir(url);
	if (dir.exists())
	{ showInGraphicalShell(url); }
	else
	{
		int reply = QMessageBox::question(this, tr("Folder does not exist"), tr("The save folder does not exist yet. Create it?"), QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::Yes)
		{
			QDir dir(path);
			if (!dir.mkpath(pth))
			{ error(this, tr("Error creating folder.\r\n%1").arg(url)); }
			showInGraphicalShell(url);
		}
	}
}
void zoomWindow::openSaveDirFav()
{ openSaveDir(true); }

void zoomWindow::linkHovered(QString url)
{ this->link = url; }
void zoomWindow::contextMenu(QPoint)
{
	QMenu *menu = new QMenu(this);
	if (!this->link.isEmpty())
	{
		// Favorites
		if (m_favorites.contains(Favorite(link)))
		{
			menu->addAction(QIcon(":/images/icons/remove.png"), tr("Remove from favorites"), this, SLOT(unfavorite()));
			menu->addAction(QIcon(":/images/icons/save.png"), tr("Choose as image"), this, SLOT(setfavorite()));
		}
		else
		{ menu->addAction(QIcon(":/images/icons/add.png"), tr("Add to favorites"), this, SLOT(favorite())); }

		// Keep for later
		if (m_viewItLater.contains(link, Qt::CaseInsensitive))
		{ menu->addAction(QIcon(":/images/icons/remove.png"), tr("Don't keep for later"), this, SLOT(unviewitlater())); }
		else
		{ menu->addAction(QIcon(":/images/icons/add.png"), tr("Keep for later"), this, SLOT(viewitlater())); }

		// Ignore
		if (m_ignore.contains(link, Qt::CaseInsensitive))
		{ menu->addAction(QIcon(":/images/icons/showed.png"), tr("Don't ignore"), this, SLOT(unignore())); }
		else
		{ menu->addAction(QIcon(":/images/icons/hidden.png"), tr("Ignore"), this, SLOT(ignore())); }
		menu->addSeparator();

		// Copy
		menu->addAction(QIcon(":/images/icons/copy.png"), tr("Copy tag"), this, SLOT(copyTagToClipboard()));
		menu->addAction(QIcon(":/images/icons/copy.png"), tr("Copy all tags"), this, SLOT(copyAllTagsToClipboard()));
		menu->addSeparator();

		// Tabs
		menu->addAction(QIcon(":/images/icons/tab-plus.png"), tr("Open in a new tab"), this, SLOT(openInNewTab()));
		menu->addAction(QIcon(":/images/icons/window.png"), tr("Open in new a window"), this, SLOT(openInNewWindow()));
		menu->addAction(QIcon(":/images/icons/browser.png"), tr("Open in browser"), this, SLOT(openInBrowser()));
	}
	menu->exec(QCursor::pos());
}

void zoomWindow::openInNewTab()
{
	m_parent->addTab(link);
}
void zoomWindow::openInNewWindow()
{
	QProcess myProcess;
	myProcess.startDetached(qApp->arguments().at(0), QStringList(link));
}
void zoomWindow::openInBrowser()
{
	QDesktopServices::openUrl(m_image->pageUrl());
}
void zoomWindow::copyTagToClipboard()
{
	QApplication::clipboard()->setText(this->link);
}
void zoomWindow::copyAllTagsToClipboard()
{
	QStringList tags;
	for (Tag tag : m_image->tags())
		tags.append(tag.text());

	QApplication::clipboard()->setText(tags.join(' '));
}

void zoomWindow::favorite()
{
	Favorite fav(link);
	if (image != nullptr)
		fav.setImage(*image);

	m_profile->addFavorite(fav);
}
void zoomWindow::setfavorite()
{
	if (image == nullptr)
		return;

	Favorite fav(link);
	int pos = m_favorites.indexOf(fav);
	if (pos >= 0)
	{
		m_favorites[pos].setImage(*image);
	}
	else
	{
		fav.setImage(*image);
		m_favorites.append(fav);
	}

	m_profile->emitFavorite();
}
void zoomWindow::unfavorite()
{
	m_profile->removeFavorite(Favorite(link));
}

void zoomWindow::viewitlater()
{
	m_profile->addKeptForLater(link);
}
void zoomWindow::unviewitlater()
{
	m_profile->removeKeptForLater(link);
}

void zoomWindow::ignore()
{
	m_profile->addIgnored(link);
}
void zoomWindow::unignore()
{
	m_profile->removeIgnored(link);
}

void zoomWindow::load()
{
	log(QString("Loading image from <a href=\"%1\">%1</a>").arg(m_url));

	m_data.clear();
	m_source.clear();

	ui->progressBarDownload->setMaximum(100);
	ui->progressBarDownload->setValue(0);

	m_imageTime.start();

	if (m_reply != nullptr && m_reply->isRunning())
		m_reply->abort();

	m_reply = m_site->get(m_url, nullptr, "image", m_image.data());
	connect(m_reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(downloadProgress(qint64, qint64)));
	connect(m_reply, SIGNAL(finished()), this, SLOT(replyFinishedZoom()));
}
void zoomWindow::sslErrorHandler(QNetworkReply* qnr, QList<QSslError>)
{ qnr->ignoreSslErrors(); }

#define PERCENT 0.05f
#define TIME 500
void zoomWindow::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	ui->progressBarDownload->show();
	ui->progressBarDownload->setMaximum(bytesTotal);
	ui->progressBarDownload->setValue(bytesReceived);

	if (m_imageTime.elapsed() > TIME || (bytesTotal > 0 && bytesReceived / bytesTotal > PERCENT))
	{
		m_imageTime.restart();

		if (!m_thread)
		{
			m_data.append(m_reply->readAll());
			m_thread = true;
			m_th = new ImageThread(m_data);
			connect(m_th, SIGNAL(finished(QPixmap*, int)), this, SLOT(display(QPixmap*, int)));
			connect(m_th, SIGNAL(finished()), m_th, SLOT(deleteLater()));
			m_th->start();
		}
	}
}
void zoomWindow::display(QPixmap *pix, int size)
{
	if (!pix->size().isEmpty() && size >= m_size)
	{
		m_size = size;
		delete image;
		image = pix;
		if (m_url.section('.', -1).toLower() == "gif")
		{ /*m_labelImage->setPixmap(*image);*/ }
		else
		{ update(!m_finished); }
		m_thread = false;

		if (m_isFullscreen && m_fullScreen != nullptr && m_fullScreen->isVisible())
		{ m_fullScreen->setImage(image->scaled(QApplication::desktop()->screenGeometry().size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)); }
	}
}

void zoomWindow::replyFinishedDetails()
{
	m_loadedDetails = true;
	colore();

	// Show pool information
	if (!m_image->pools().isEmpty())
	{
		QStringList pools = QStringList();
		for (const Pool &p : m_image->pools())
		{ pools.append((p.previous() != 0 ? "<a href=\""+QString::number(p.previous())+"\">&lt;</a> " : "")+"<a href=\"pool:"+QString::number(p.id())+"\">"+p.name()+"</a>"+(p.next() != 0 ? " <a href=\""+QString::number(p.next())+"\">&gt;</a>" : "")); }
		ui->labelPools->setText(pools.join("<br />"));
		ui->labelPools->show();
	}

	QString path1 = m_settings->value("Save/path").toString().replace("\\", "/");
	QStringList pth1s = m_image->path(m_settings->value("Save/filename").toString(), path1);
	QString source1;
	bool file1notexists = false;
	for (QString pth1 : pth1s)
	{
		if (path1.right(1) == "/")
		{ path1 = path1.left(path1.length()-1); }
		QFile file(path1+"/"+pth1);
		if (file.exists())
			source1 = file.fileName();
		else
			file1notexists = true;
	}

	QString path2 = m_settings->value("Save/path_favorites").toString().replace("\\", "/");
	QStringList pth2s = m_image->path(m_settings->value("Save/filename_favorites").toString(), path2);
	QString source2;
	bool file2notexists = false;
	for (QString pth2 : pth2s)
	{
		if (path2.right(1) == "/")
		{ path2 = path1.left(path2.length()-1); }
		QFile file(path2+"/"+pth2);
		if (file.exists())
			source2 = file.fileName();
		else
			file2notexists = true;
	}

	// If the file already exists, we directly display it
	if (!file1notexists || !file2notexists)
	{
		if (!file1notexists)
		{
			ui->buttonSave->setText(tr("File already exists"));
			ui->buttonSaveNQuit->setText(tr("Close"));
		}
		if (!file2notexists)
		{
			ui->buttonSaveFav->setText(tr("File already exists (fav)"));
			ui->buttonSaveNQuitFav->setText(tr("Close (fav)"));
		}
		m_source = !file1notexists ? source1 : source2;
		log(QString("Image loaded from the file <a href=\"file:///%1\">%1</a>").arg(m_source));

		// Fix extension when it should be guessed
		QString fext = m_source.section('.', -1);
		m_url = m_url.section('.', 0, -2) + "." + fext;
		m_image->setFileExtension(fext);
		m_finished = true;

		draw();
	}

	// If the file does not exist, we have to load it
	else
	{
		if (m_url.isEmpty())
		{ m_url = m_image->url(); }
		load();
	}

	updateWindowTitle();
}
void zoomWindow::colore()
{
	QStringList t = m_image->stylishedTags(m_profile);
	tags = t.join(' ');
	if (ui->widgetLeft->isHidden())
	{ m_labelTagsTop->setText(tags); }
	else
	{ m_labelTagsLeft->setText(t.join("<br/>")); }
	m_detailsWindow->setTags(tags);
}

void zoomWindow::replyFinishedZoom()
{
	ui->progressBarDownload->hide();

	// Check redirection
	QUrl redir = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redir.isEmpty())
	{
		m_url = redir.toString();
		m_image->setUrl(m_url);
		load();
		return;
	}

	log(QString("Image received from <a href=\"%1\">%1</a>").arg(m_url));
	m_finished = true;
	if (m_reply->error() == QNetworkReply::NoError)
	{
		m_data.append(m_reply->readAll());
		m_image->setData(m_data);
		m_url = m_url.section('.', 0, -2) + "." + getExtension(m_image->url());
		m_loadedImage = true;
		pendingUpdate();

		draw();
	}
	else if (m_reply->error() == QNetworkReply::ContentNotFoundError)
	{
		QString ext = m_url.section('.', -1);
		QString newext = m_image->getNextExtension(ext);
		if (newext.isEmpty())
		{
			log("Image not found.");
		}
		else
		{
			QString oldUrl = m_url;
			m_url = m_url.section('.', 0, -2) + "." + newext;
			m_image->setFileExtension(newext);
			log(QString("Image not found. New try with extension %1 (%2)...").arg(newext, oldUrl));
			load();
			return;
		}
	}
	else if (m_reply->error() != QNetworkReply::OperationCanceledError)
	{ error(this, tr("An unexpected error occured loading the image (%1 - %2).\r\n%3").arg(m_reply->error()).arg(m_reply->errorString()).arg(m_reply->url().toString())); }

	m_reply->deleteLater();
	m_reply = nullptr;
}

void zoomWindow::pendingUpdate()
{
	// If we don't want to save, nothing to do
	if (m_mustSave == 0)
		return;

	// If the image is not even loaded, we cannot save it
	if (!m_loadedImage)
		return;

	bool fav = (m_mustSave == 3 || m_mustSave == 4);
	Filename fn(m_settings->value("Save/path" + QString(fav ? "_favorites" : "")).toString());

	// If the image is loaded but we need their tags and we don't have them, we wait
	if (!m_loadedDetails && fn.needExactTags(m_site))
		return;

	m_loaded = true;

	switch (m_mustSave)
	{
		case 1:
			saveImageNow();
			break;

		case 2:
			if (!saveImageNow().isEmpty())
				close();
			break;

		case 3:
			saveImageNow(true);
			break;

		case 4:
			if (!saveImageNow(true).isEmpty())
				close();
			break;

		case 5:
			openFile(true);
			break;
	}
}

void zoomWindow::draw()
{
	QString fn = m_url.section('/', -1).toLower();
	QString ext = fn.section('.', -1).toLower();

	// We need a filename to display animations, so we get it if we're not already loading from a file
	QString filename;
	if (!m_source.isEmpty())
	{ filename = m_source; }
	else if (ext == "gif")
	{
		filename = QDir::temp().absoluteFilePath("grabber-" + fn);
		QFile f(filename);
		if (f.open(QFile::WriteOnly))
		{
			f.write(m_data);
			f.close();
		}
	}

	// GIF (using QLabel support for QMovie)
	if (ext == "gif")
	{
		this->movie = new QMovie(filename, QByteArray(), this);
		m_labelImage->setMovie(this->movie);
		m_stackedWidget->setCurrentWidget(m_labelImage);
		this->movie->start();

		this->image = nullptr;

		if (m_isFullscreen && m_fullScreen != nullptr && m_fullScreen->isVisible())
		{ m_fullScreen->setMovie(movie); }
		return;
	}

	// Images
	if (!m_source.isEmpty())
	{
		QPixmap *img = new QPixmap;
		img->load(m_source);
		this->image = img;

		m_loadedImage = true;
		pendingUpdate();
		update();

		if (m_isFullscreen && m_fullScreen != nullptr && m_fullScreen->isVisible())
		{ m_fullScreen->setImage(image->scaled(QApplication::desktop()->screenGeometry().size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)); }
	}
	else
	{
		m_thread = true;
		m_th = new ImageThread(m_data);
		connect(m_th, SIGNAL(finished(QPixmap*, int)), this, SLOT(display(QPixmap*, int)));
		connect(m_th, SIGNAL(finished()), m_th, SLOT(deleteLater()));
		m_th->start();
	}
}



/**
 * Updates the image label to use the current image.
 * @param onlysize true to update the image quickly
 */
void zoomWindow::update(bool onlysize)
{
	// Ignore this event for animations
	if (this->image == nullptr)
		return;

	bool needScaling = (this->image->width() > m_labelImage->width() || this->image->height() > m_labelImage->height());
	if (needScaling && (onlysize || m_loadedImage))
	{
		Qt::TransformationMode mode = onlysize ? Qt::FastTransformation : Qt::SmoothTransformation;
		m_labelImage->setImage(this->image->scaled(m_labelImage->width(), m_labelImage->height(), Qt::KeepAspectRatio, mode));
	}
	else if (m_loadedImage)
	{
		m_labelImage->setImage(*this->image);
	}

	m_stackedWidget->setCurrentWidget(m_labelImage);
}

void zoomWindow::saveNQuit()
{
	ui->buttonSaveNQuit->setText(tr("Saving..."));
	m_mustSave = 2;
	pendingUpdate();
}
void zoomWindow::saveNQuitFav()
{
	ui->buttonSaveNQuitFav->setText(tr("Saving..."));
	m_mustSave = 4;
	pendingUpdate();
}

void zoomWindow::saveImage(bool fav)
{
	if (fav)
	{
		ui->buttonSaveFav->setText(tr("Saving... (fav)"));
		m_mustSave = 3;
	}
	else
	{
		ui->buttonSave->setText(tr("Saving..."));
		m_mustSave = 1;
	}
	pendingUpdate();
}
void zoomWindow::saveImageFav()
{ saveImage(true); }
QStringList zoomWindow::saveImageNow(bool fav)
{
	QString pth = m_settings->value("Save/path"+QString(fav ? "_favorites" : "")).toString().replace("\\", "/");
	if (pth.right(1) == "/")
	{ pth = pth.left(pth.length()-1); }

	if (pth.isEmpty() || m_settings->value("Save/filename").toString().isEmpty())
	{
		int reply;
		if (pth.isEmpty())
		{ reply = QMessageBox::question(this, tr("Error"), tr("You did not specified a save folder! Do you want to open the options window?"), QMessageBox::Yes | QMessageBox::No); }
		else
		{ reply = QMessageBox::question(this, tr("Error"), tr("You did not specified a save format! Do you want to open the options window?"), QMessageBox::Yes | QMessageBox::No); }
		if (reply == QMessageBox::Yes)
		{
			optionsWindow *options = new optionsWindow(m_profile, parentWidget());
			//options->onglets->setCurrentIndex(3);
			options->setWindowModality(Qt::ApplicationModal);
			options->show();
			connect(options, SIGNAL(closed()), this, SLOT(saveImage()));
		}
		return QStringList();
	}

	QStringList paths;
	QMap<QString, Image::SaveResult> results = m_image->save(m_settings->value("Save/filename"+QString(fav ? "_favorites" : "")).toString(), pth);
	auto it = results.begin();
	while (it != results.end())
	{
		Image::SaveResult res = it.value();
		paths.append(it.key());
		m_imagePath = it.key();

		QPushButton *button = fav ? ui->buttonSaveFav : ui->buttonSave;
		QPushButton *saveQuit = fav ? ui->buttonSaveNQuitFav : ui->buttonSaveNQuit;
		switch (res)
		{
			case Image::SaveResult::Error:
				error(this, tr("Error saving image."));
				return QStringList();
				break;

			case Image::SaveResult::Saved:
				button->setText(fav ? tr("Saved! (fav)") : tr("Saved!"));
				break;

			case Image::SaveResult::Copied:
				button->setText(fav ? tr("Copied! (fav)") : tr("Copied!"));
				break;

			case Image::SaveResult::Moved:
				button->setText(fav ? tr("Moved! (fav)") : tr("Moved!"));
				break;

			case Image::SaveResult::Ignored:
				button->setText(fav ? tr("Ignored! (fav)") : tr("Ignored!"));
				break;

			case Image::SaveResult::AlreadyExists:
				button->setText(fav ? tr("File already exists (fav)") : tr("File already exists"));
				break;
		}
		saveQuit->setText(fav ? tr("Close (fav)") : tr("Close"));

		++it;
	}

	if (m_mustSave == 2 || m_mustSave == 4)
		close();

	m_mustSave = 0;
	return paths;
}

QString zoomWindow::saveImageAs()
{
	QString filename = m_image->fileUrl().toString().section('/', -1);
	QString lastDir = m_settings->value("Zoom/lastDir", "").toString();

	QString path = QFileDialog::getSaveFileName(this, tr("Save image"), QDir::toNativeSeparators(lastDir + "/" + filename), "Images (*.png *.gif *.jpg *.jpeg)");
	if (!path.isEmpty())
	{
		path = QDir::toNativeSeparators(path);
		m_settings->setValue("Zoom/lastDir", path.section(QDir::toNativeSeparators("/"), 0, -2));

		m_image->save(path, true, true);
		m_imagePath = path;
	}

	return path;
}


void zoomWindow::toggleFullScreen()
{
	if (m_isFullscreen)
		unfullScreen();
	else
		fullScreen();
}

void zoomWindow::fullScreen()
{
	if (image == nullptr && movie == nullptr)
		return;

	QString ext = m_url.section('.', -1).toLower();

	QWidget *widget;
	m_fullScreen = new QAffiche(QVariant(), 0, QColor(), this);
	m_fullScreen->setStyleSheet("background-color: black");
	m_fullScreen->setAlignment(Qt::AlignCenter);
	if (ext == "gif")
	{ m_fullScreen->setMovie(movie); }
	else
	{ m_fullScreen->setImage(image->scaled(QApplication::desktop()->screenGeometry().size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)); }
	m_fullScreen->setWindowFlags(Qt::Window);
	m_fullScreen->showFullScreen();

	connect(m_fullScreen, SIGNAL(doubleClicked()), this, SLOT(unfullScreen()));
	widget = m_fullScreen;

	m_isFullscreen = true;
	prepareNextSlide();

	QShortcut *escape = new QShortcut(QKeySequence(Qt::Key_Escape), widget);
		connect(escape, SIGNAL(activated()), this, SLOT(unfullScreen()));
	QShortcut *toggleFullscreen = new QShortcut(QKeySequence::FullScreen, widget);
		connect(toggleFullscreen, SIGNAL(activated()), this, SLOT(unfullScreen()));
	QShortcut *arrowNext = new QShortcut(QKeySequence(Qt::Key_Right), widget);
		connect(arrowNext, &QShortcut::activated, this, &zoomWindow::next);
	QShortcut *arrowPrevious = new QShortcut(QKeySequence(Qt::Key_Left), widget);
		connect(arrowPrevious, &QShortcut::activated, this, &zoomWindow::previous);
	QShortcut *space = new QShortcut(QKeySequence(Qt::Key_Space), widget);
		connect(space, &QShortcut::activated, this, &zoomWindow::toggleSlideshow);

	widget->setFocus();
}

void zoomWindow::unfullScreen()
{
	m_slideshow.stop();

	m_fullScreen->close();
	m_fullScreen->deleteLater();
	m_fullScreen = nullptr;

	m_isFullscreen = false;
}

void zoomWindow::prepareNextSlide()
{
	// Slideshow is only enabled in fullscreen
	if (!m_isFullscreen)
		return;

	// If the slideshow is disabled
	int interval = m_settings->value("slideshow", 0).toInt();
	if (interval <= 0)
		return;

	// We make sure to wait to see the whole displayed item
	qint64 additionalInterval = 0;
	QString ext = getExtension(m_image->url());
	if (ext == "gif")
		additionalInterval = movie->nextFrameDelay() * movie->frameCount();

	qint64 totalInterval = interval * 1000 + additionalInterval;
	m_slideshow.start(totalInterval);
	m_isSlideshowRunning = true;
}

void zoomWindow::toggleSlideshow()
{
	m_isSlideshowRunning = !m_isSlideshowRunning;

	if (!m_isSlideshowRunning)
		m_slideshow.stop();
	else
		prepareNextSlide();
}



void zoomWindow::resizeEvent(QResizeEvent *e)
{
	if (m_loadedImage && m_finished && !m_thread)
	{
		if (!m_resizeTimer->isActive())
		{ this->timeout = qMin(500, qMax(50, (this->image->width() * this->image->height()) / 100000)); }
		m_resizeTimer->stop();
		m_resizeTimer->start(this->timeout);
		this->update(true);
	}
	QWidget::resizeEvent(e);
}

void zoomWindow::closeEvent(QCloseEvent *e)
{
	m_settings->setValue("Zoom/geometry", saveGeometry());
	m_settings->setValue("Zoom/plus", ui->buttonPlus->isChecked());
	m_settings->sync();

	//m_image->abortTags();
	if (m_reply != nullptr && m_reply->isRunning())
	{
		m_reply->abort();
		log("Image loading stopped.");
	}

	e->accept();
}

void zoomWindow::urlChanged(QString old, QString nouv)
{
	Q_UNUSED(old);
	m_url = nouv;
}


void zoomWindow::load(QSharedPointer<Image> image)
{
	disconnect(m_image.data(), &Image::finishedLoadingTags, this, &zoomWindow::replyFinishedDetails);

	m_image = image;
	connect(m_image.data(), &Image::urlChanged, this, &zoomWindow::urlChanged, Qt::UniqueConnection);

	// Update image label to show the thumbnail while waiting for the full size image
	QSize size = m_image->previewImage().size() * 2 * m_settings->value("thumbnailUpscale", 1.0f).toFloat();
	if (m_image->isVideo())
	{
		QPixmap base = m_image->previewImage();
		QPixmap overlay = QPixmap(":/images/play-overlay.png");
		QPixmap result(size.width(), size.height());
		result.fill(Qt::transparent);
		{
			QPainter painter(&result);
			painter.drawPixmap(0, 0, size.width(), size.height(), base);
			painter.drawPixmap(qMax(0, (size.width() - overlay.width()) / 2), qMax(0, (size.height() - overlay.height()) / 2), overlay.width(), overlay.height(), overlay);
		}
		m_labelImage->setPixmap(result);
	}
	else
	{
		m_labelImage->setPixmap(m_image->previewImage().scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	}

	m_size = 0;

	// Preload gallery images
	int preload = m_settings->value("preload", 0).toInt();
	if (preload > 0)
	{
		int index = m_images.indexOf(m_image);
		for (int i = index - preload; i <= index + preload; ++i)
		{
			if (i == index)
				continue;

			int pos = (i + m_images.count()) % m_images.count();
			log(QString("Preloading data for image #%1").arg(pos));
			m_images[pos]->loadDetails();
			m_images[pos]->loadImage();
		}
	}

	// Reset buttons
	ui->buttonSave->setText(tr("Save"));
	ui->buttonSaveFav->setText(tr("Save"));
	ui->buttonSaveNQuit->setText(tr("Save and close"));
	ui->buttonSaveNQuitFav->setText(tr("Save and close"));

	// Window title
	updateWindowTitle();

	prepareNextSlide();
	go();
}

void zoomWindow::updateWindowTitle()
{
	QStringList infos;

	// Extension
	infos.append(getExtension(m_image->fileUrl()).toUpper());

	// Filesize
	if (m_image->fileSize() != 0)
		infos.append(formatFilesize(m_image->fileSize()));

	// Image size
	if (!m_image->size().isEmpty())
		infos.append(QString("%1 x %2").arg(m_image->size().width()).arg(m_image->size().height()));

	// Update title if there are infos to show
	if (infos.isEmpty())
		setWindowTitle(tr("Image"));
	else
		setWindowTitle(QString(tr("Image") + " (%1)").arg(infos.join(", ")));
}

void zoomWindow::next()
{
	m_image->abortTags();
	m_image->abortImage();

	int index = m_images.indexOf(m_image);
	index = (index + 1) % m_images.count();

	load(m_images[index]);
}

void zoomWindow::previous()
{
	m_image->abortTags();
	m_image->abortImage();

	int index = m_images.indexOf(m_image);
	index = (index + m_images.count() - 1) % m_images.count();

	load(m_images[index]);
}

void zoomWindow::updateButtonPlus()
{
	ui->buttonPlus->setText(ui->buttonPlus->isChecked() ? "-" : "+");
}

void zoomWindow::openFile(bool now)
{
	if (!now)
	{
		m_mustSave = 5;
		pendingUpdate();
		return;
	}

	QString path = m_imagePath;
	if (path.isEmpty())
	{
		QMap<QString, Image::SaveResult> files = m_image->save(m_settings->value("Save/filename").toString(), QDir::tempPath());
		path = files.firstKey();
	}

	QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}
