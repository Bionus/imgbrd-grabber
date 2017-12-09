#include "zoom-window.h"
#include "ui_zoom-window.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QShortcut>
#include <QMenu>
#include <QFileDialog>
#include <QScrollBar>
#include "ui/QAffiche.h"
#include "models/filename.h"
#include "models/site.h"
#include "models/page.h"
#include "models/profile.h"
#include "models/post-filter.h"
#include "settings/optionswindow.h"
#include "tags/tag-stylist.h"
#include "threads/image-loader.h"
#include "threads/image-loader-queue.h"
#include "details-window.h"
#include "mainwindow.h"
#include "helpers.h"
#include "functions.h"
#include "image-context-menu.h"
#include "tag-context-menu.h"


ZoomWindow::ZoomWindow(QList<QSharedPointer<Image>> images, QSharedPointer<Image> image, Site *site, Profile *profile, mainWindow *parent)
	: QWidget(Q_NULLPTR, Qt::Window), m_parent(parent), m_profile(profile), m_favorites(profile->getFavorites()), m_viewItLater(profile->getKeptForLater()), m_ignore(profile->getIgnored()), m_settings(profile->getSettings()), ui(new Ui::ZoomWindow), m_site(site), m_timeout(300), m_tooBig(false), m_loadedImage(false), m_loadedDetails(false), m_displayImage(QPixmap()), m_displayMovie(nullptr), m_finished(false), m_size(0), m_source(), m_fullScreen(nullptr), m_images(images), m_isFullscreen(false), m_isSlideshowRunning(false), m_imagePath(""), m_labelImageScaled(false)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);

	m_mustSave = 0;

	restoreGeometry(m_settings->value("Zoom/geometry").toByteArray());
	ui->buttonPlus->setChecked(m_settings->value("Zoom/plus", false).toBool());
	ui->progressBarDownload->hide();

	QShortcut *escape = new QShortcut(QKeySequence(Qt::Key_Escape), this);
		connect(escape, &QShortcut::activated, this, &ZoomWindow::close);
	QShortcut *save = new QShortcut(QKeySequence::Save, this);
		connect(save, SIGNAL(activated()), this, SLOT(saveImage()));
	QShortcut *saveAs = new QShortcut(QKeySequence::SaveAs, this);
		connect(saveAs, &QShortcut::activated, this, &ZoomWindow::saveImageAs);
	QShortcut *arrowNext = new QShortcut(QKeySequence(Qt::Key_Right), this);
		connect(arrowNext, &QShortcut::activated, this, &ZoomWindow::next);
	QShortcut *arrowPrevious = new QShortcut(QKeySequence(Qt::Key_Left), this);
		connect(arrowPrevious, &QShortcut::activated, this, &ZoomWindow::previous);
	QShortcut *copyImageFile = new QShortcut(QKeySequence::Copy, this);
		connect(copyImageFile, &QShortcut::activated, this, &ZoomWindow::copyImageDataToClipboard);
	QShortcut *toggleFullscreen = new QShortcut(QKeySequence::FullScreen, this);
		connect(toggleFullscreen, &QShortcut::activated, this, &ZoomWindow::toggleFullScreen);

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
	connect(m_profile, &Profile::favoritesChanged, this, &ZoomWindow::colore);
	connect(m_profile, &Profile::keptForLaterChanged, this, &ZoomWindow::colore);
	connect(m_profile, &Profile::ignoredChanged, this, &ZoomWindow::colore);
	connect(m_profile, &Profile::blacklistChanged, this, &ZoomWindow::colore);

	m_stackedWidget = new QStackedWidget(this);
		ui->verticalLayout->insertWidget(1, m_stackedWidget, 1);
	m_labelImage = new QAffiche(QVariant(), 0, QColor(), this);
		m_labelImage->setSizePolicy(QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored));
		connect(m_labelImage, SIGNAL(doubleClicked()), this, SLOT(openFile()));
		m_stackedWidget->addWidget(m_labelImage);

	connect(ui->buttonDetails, SIGNAL(clicked()), this, SLOT(showDetails()));
	connect(ui->buttonPlus, &QPushButton::toggled, this, &ZoomWindow::updateButtonPlus);

	m_labelImage->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_labelImage, &QAffiche::customContextMenuRequested, this, &ZoomWindow::imageContextMenu);

	m_slideshow.setSingleShot(true);
	connect(&m_slideshow, &QTimer::timeout, this, &ZoomWindow::next);

	// Overlay progressbar and image
	ui->overlayLayout->removeWidget(ui->progressBarDownload);
	ui->overlayLayout->addWidget(ui->progressBarDownload, 0, 0, Qt::AlignBottom);
	ui->progressBarDownload->raise();

	// Threads
	m_imageLoaderThread.setObjectName("Image loader thread");
	m_imageLoader = new ImageLoader();
	m_imageLoader->moveToThread(&m_imageLoaderThread);
	m_imageLoaderQueueThread.setObjectName("Image loader queue thread");
	m_imageLoaderQueue = new ImageLoaderQueue(m_imageLoader);
	m_imageLoaderQueue->moveToThread(&m_imageLoaderQueueThread);
	connect(&m_imageLoaderThread, &QThread::finished, m_imageLoader, &QObject::deleteLater);
	connect(&m_imageLoaderQueueThread, &QThread::finished, m_imageLoaderQueue, &QObject::deleteLater);
	connect(this, &ZoomWindow::loadImage, m_imageLoaderQueue, &ImageLoaderQueue::load);
	connect(this, &ZoomWindow::clearLoadQueue, m_imageLoaderQueue, &ImageLoaderQueue::clear);
	connect(m_imageLoaderQueue, &ImageLoaderQueue::finished, this, &ZoomWindow::display);
	m_imageLoaderQueueThread.start();
	m_imageLoaderThread.start();

	// Background color
	QString bg = m_settings->value("imageBackgroundColor", "").toString();
	if (!bg.isEmpty())
	{
		setStyleSheet("#zoomWindow, #scrollAreaWidgetContents { background-color:" + bg + "; }");
		m_labelImage->setStyleSheet("background-color:" + bg);
		m_labelTagsLeft->setStyleSheet("background-color:" + bg);
		m_labelTagsTop->setStyleSheet("background-color:" + bg);
	}

	load(image);
}
void ZoomWindow::go()
{
	ui->labelPools->hide();
	bool whitelisted = false;
	if (!m_settings->value("whitelistedtags").toString().isEmpty())
	{
		QStringList whitelist = m_settings->value("whitelistedtags").toString().split(" ");
		for (const Tag &t : m_image->tags())
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

	auto *timer = new QTimer(this);
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
		m_labelTagsTop->show();
	}
	else
	{
		m_labelTagsTop->hide();
		m_labelTagsLeft->show();
		ui->widgetLeft->show();
	}

	m_detailsWindow = new DetailsWindow(m_profile, this);
	colore();

	// Load image details (exact tags & co)
	connect(m_image.data(), &Image::finishedLoadingTags, this, &ZoomWindow::replyFinishedDetails, Qt::UniqueConnection);
	m_image->loadDetails();

	if (!m_isFullscreen)
		activateWindow();
}

/**
 * Destructor of the zoomWindow class
 */
ZoomWindow::~ZoomWindow()
{
	if (m_displayMovie != nullptr)
		m_displayMovie->deleteLater();

	m_labelTagsTop->deleteLater();
	m_labelTagsLeft->deleteLater();
	m_detailsWindow->deleteLater();

	// Quit threads
	m_imageLoaderQueueThread.quit();
	m_imageLoaderThread.wait(1000);
	m_imageLoaderThread.quit();
	m_imageLoaderThread.wait(1000);

	delete ui;
}

void ZoomWindow::imageContextMenu()
{
	QMenu *menu = new ImageContextMenu(m_settings, m_image, m_parent, this);

	// Reload action
	QAction *reloadImageAction = new QAction(QIcon(":/images/icons/update.png"), tr("Reload"), menu);
	connect(reloadImageAction, SIGNAL(triggered()), this, SLOT(reloadImage()));

	// Copy actions
	QAction *copyImageAction = new QAction(QIcon(":/images/icons/copy.png"), tr("Copy file"), menu);
	connect(copyImageAction, SIGNAL(triggered()), this, SLOT(copyImageFileToClipboard()));
	QAction *copyDataAction = new QAction(QIcon(":/images/icons/document-binary.png"), tr("Copy data"), menu);
	connect(copyDataAction, SIGNAL(triggered()), this, SLOT(copyImageDataToClipboard()));

	// Insert actions at the beginning
	QAction *first = menu->actions().first();
	menu->insertAction(first, reloadImageAction);
	menu->insertSeparator(first);
	menu->insertAction(first, copyImageAction);
	menu->insertAction(first, copyDataAction);
	menu->insertSeparator(first);

	menu->exec(QCursor::pos());
}
void ZoomWindow::reloadImage()
{
	load(true);
}
void ZoomWindow::copyImageFileToClipboard()
{
	QString path = m_imagePath;
	if (path.isEmpty() || !QFile::exists(path))
	{
		QMap<QString, Image::SaveResult> files = m_image->save(m_settings->value("Save/filename").toString(), m_profile->tempPath(), false, false, 1, true);
		path = files.firstKey();
	}

	auto *mimeData = new QMimeData();
	mimeData->setUrls({ QUrl::fromLocalFile(path) });
	QApplication::clipboard()->setMimeData(mimeData);
}
void ZoomWindow::copyImageDataToClipboard()
{
	QApplication::clipboard()->setPixmap(m_displayImage);
}

void ZoomWindow::showDetails()
{
	m_detailsWindow->setImage(m_image);
	m_detailsWindow->show();
}

void ZoomWindow::openUrl(QString url)
{ emit linkClicked(url); }
void ZoomWindow::openPool(QString url)
{
	if (url.startsWith("pool:"))
	{ emit poolClicked(url.right(url.length() - 5).toInt(), m_image->site()); }
	else
	{
		Page *p = new Page(m_profile, m_image->parentSite(), m_profile->getSites().values(), QStringList() << "id:"+url, 1, 1, QStringList(), false, this);
		connect(p, SIGNAL(finishedLoading(Page*)), this, SLOT(openPoolId(Page*)));
		p->load();
	}
}
void ZoomWindow::openPoolId(Page *p)
{
	if (p->images().empty())
	{
		p->deleteLater();
		return;
	}

	m_image = p->images().at(0);
	m_timeout = 300;
	m_loadedDetails = false;
	m_loadedImage = false;
	m_finished = false;
	m_size = 0;
	m_labelImage->hide();
	ui->verticalLayout->removeWidget(m_labelImage);

	go();
}

void ZoomWindow::openSaveDir(bool fav)
{
	// If the file was already saved, we focus on it
	if (!m_imagePath.isEmpty())
	{
		showInGraphicalShell(m_imagePath);
	}
	else
	{
		QString path = m_settings->value("Save/path"+QString(fav ? "_favorites" : "")).toString().replace("\\", "/"), fn = m_settings->value("Save/filename"+QString(fav ? "_favorites" : "")).toString();

		if (path.right(1) == "/")
		{ path = path.left(path.length()-1); }
		path = QDir::toNativeSeparators(path);

		QStringList files = m_image->path(fn, path);
		QString file = files.empty() ? "" : files.at(0);
		QString pth = file.section(QDir::separator(), 0, -2);
		QString url = path + QDir::separator() + pth;

		QDir dir(url);
		if (dir.exists())
		{ showInGraphicalShell(url); }
		else
		{
			int reply = QMessageBox::question(this, tr("Folder does not exist"), tr("The save folder does not exist yet. Create it?"), QMessageBox::Yes | QMessageBox::No);
			if (reply == QMessageBox::Yes)
			{
				QDir rootDir(path);
				if (!rootDir.mkpath(pth))
				{ error(this, tr("Error creating folder.\n%1").arg(url)); }
				showInGraphicalShell(url);
			}
		}
	}
}
void ZoomWindow::openSaveDirFav()
{ openSaveDir(true); }

void ZoomWindow::linkHovered(QString url)
{ m_link = url; }
void ZoomWindow::contextMenu(QPoint)
{
	if (m_link.isEmpty())
		return;

	Page page(m_profile, m_site, QList<Site*>() << m_site, QStringList() << m_link);
	TagContextMenu *menu = new TagContextMenu(m_link, m_image->tags(), page.friendlyUrl(), m_profile, true, this);
	connect(menu, &TagContextMenu::openNewTab, this, &ZoomWindow::openInNewTab);
	connect(menu, &TagContextMenu::setFavoriteImage, this, &ZoomWindow::setfavorite);
	menu->exec(QCursor::pos());
}

void ZoomWindow::openInNewTab()
{
	m_parent->addTab(m_link);
}
void ZoomWindow::setfavorite()
{
	if (!m_loadedImage)
		return;

	Favorite fav(m_link);
	int pos = m_favorites.indexOf(fav);
	if (pos >= 0)
	{
		m_favorites[pos].setImage(m_displayImage);
	}
	else
	{
		fav.setImage(m_displayImage);
		m_favorites.append(fav);
	}

	m_profile->emitFavorite();
}

void ZoomWindow::load(bool force)
{
	log(QString("Loading image from <a href=\"%1\">%1</a>").arg(m_url));

	m_source.clear();

	ui->progressBarDownload->setMaximum(100);
	ui->progressBarDownload->setValue(0);
	ui->progressBarDownload->show();

	connect(m_image.data(), &Image::downloadProgressImage, this, &ZoomWindow::downloadProgress);
	connect(m_image.data(), &Image::finishedImage, this, &ZoomWindow::replyFinishedZoom);

	if (m_image->shouldDisplaySample())
	{
		m_saveUrl = m_image->url();
		m_image->setUrl(m_url);
	}

	m_imageTime.start();
	m_image->loadImage(true, force);
}

#define PERCENT 0.05f
#define TIME 500
void ZoomWindow::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	ui->progressBarDownload->setMaximum(bytesTotal);
	ui->progressBarDownload->setValue(bytesReceived);

	bool isAnimated = m_image->isVideo() || !m_isAnimated.isEmpty();
	if (!isAnimated && (m_imageTime.elapsed() > TIME || (bytesTotal > 0 && bytesReceived / bytesTotal > PERCENT)))
	{
		m_imageTime.restart();
		emit loadImage(m_image->data());
	}
}
void ZoomWindow::display(const QPixmap &pix, int size)
{
	if (!pix.size().isEmpty() && size >= m_size)
	{
		m_size = size;
		m_displayImage = pix;
		update(!m_finished);

		if (!pix.size().isEmpty() && m_image->size().isEmpty())
		{
			m_image->setSize(pix.size());
			updateWindowTitle();
		}

		if (m_isFullscreen && m_fullScreen != nullptr && m_fullScreen->isVisible())
		{ m_fullScreen->setImage(m_displayImage.scaled(QApplication::desktop()->screenGeometry().size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)); }
	}
}

void ZoomWindow::replyFinishedDetails()
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

	m_isAnimated = m_image->isAnimated();

	QString path1 = m_settings->value("Save/path").toString().replace("\\", "/");
	QStringList pth1s = m_image->path(m_settings->value("Save/filename").toString(), path1, 0, true, false, true, true, true);
	QString source1;
	bool file1notexists = false;
	for (const QString &pth1 : pth1s)
	{
		QFile file(pth1);
		if (file.exists())
			source1 = file.fileName();
		else
			file1notexists = true;
	}

	QString path2 = m_settings->value("Save/path_favorites").toString().replace("\\", "/");
	QStringList pth2s = m_image->path(m_settings->value("Save/filename_favorites").toString(), path2, 0, true, false, true, true, true);
	QString source2;
	bool file2notexists = false;
	for (const QString &pth2 : pth2s)
	{
		QFile file(pth2);
		if (file.exists())
			source2 = file.fileName();
		else
			file2notexists = true;
	}

	QString md5Exists = m_profile->md5Exists(m_image->md5());

	// If the file already exists, we directly display it
	if (!md5Exists.isEmpty() || !file1notexists || !file2notexists)
	{
		m_source = !md5Exists.isEmpty() ? md5Exists : (!file1notexists ? source1 : source2);
		m_imagePath = m_source;
		log(QString("Image loaded from the file <a href=\"file:///%1\">%1</a>").arg(m_source));

		// Update save button state
		SaveButtonState md5State = !md5Exists.isEmpty() ? SaveButtonState::ExistsMd5 : SaveButtonState::Save;
		setButtonState(false, !file1notexists ? SaveButtonState::ExistsDisk : md5State);
		setButtonState(true, !file2notexists ? SaveButtonState::ExistsDisk : md5State);

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
void ZoomWindow::colore()
{
	QStringList t = TagStylist(m_profile).stylished(m_image->tags(), m_settings->value("Zoom/showTagCount", false).toBool(), false, m_settings->value("Zoom/tagOrder", "type").toString());
	QString tags = t.join(' ');

	if (ui->widgetLeft->isHidden())
	{ m_labelTagsTop->setText(tags); }
	else
	{
		m_labelTagsLeft->setText(t.join("<br/>"));
		ui->scrollArea->setMinimumWidth(m_labelTagsLeft->sizeHint().width() + ui->scrollArea->verticalScrollBar()->sizeHint().width());
	}

	m_detailsWindow->setImage(m_image);
}
void ZoomWindow::setButtonState(bool fav, SaveButtonState state)
{
	// Update state
	if (fav)
		m_saveButonStateFav = state;
	else
		m_saveButonState = state;

	// Update actual button label
	QPushButton *button = fav ? ui->buttonSaveFav : ui->buttonSave;
	button->setToolTip("");
	switch (state)
	{
		case SaveButtonState::Save:
			button->setText(fav ? tr("Save (fav)") : tr("Save"));
			break;

		case SaveButtonState::Saving:
			button->setText(fav ? tr("Saving... (fav)") : tr("Saving..."));
			break;

		case SaveButtonState::Saved:
			button->setText(fav ? tr("Saved! (fav)") : tr("Saved!"));
			break;

		case SaveButtonState::Copied:
			button->setText(fav ? tr("Copied! (fav)") : tr("Copied!"));
			break;

		case SaveButtonState::Moved:
			button->setText(fav ? tr("Moved! (fav)") : tr("Moved!"));
			break;

		case SaveButtonState::ExistsMd5:
			button->setToolTip(m_imagePath);
			button->setText(fav ? tr("MD5 already exists (fav)") : tr("MD5 already exists"));
			break;

		case SaveButtonState::ExistsDisk:
			button->setToolTip(m_imagePath);
			button->setText(fav ? tr("Already exists (fav)") : tr("Already exists"));
			break;

		case SaveButtonState::Delete:
			button->setText(fav ? tr("Delete (fav)") : tr("Delete"));
			break;
	}

	// Update "Save a close" button label
	QPushButton *saveQuit = fav ? ui->buttonSaveNQuitFav : ui->buttonSaveNQuit;
	switch (state)
	{
		case SaveButtonState::Save:
			saveQuit->setText(fav ? tr("Save and close (fav)") : tr("Save and close"));
			break;

		case SaveButtonState::Saving:
			saveQuit->setText(fav ? tr("Saving... (fav)") : tr("Saving..."));
			break;

		default:
			saveQuit->setText(fav ? tr("Close (fav)") : tr("Close"));
			break;
	}
}

void ZoomWindow::replyFinishedZoom(QNetworkReply::NetworkError err, QString errorString)
{
	log(QString("Image received from <a href=\"%1\">%1</a>").arg(m_url));

	ui->progressBarDownload->hide();
	m_finished = true;

	if (m_image->shouldDisplaySample())
	{ m_image->setUrl(m_saveUrl); }

	if (err == QNetworkReply::NoError)
	{
		m_url = m_image->url();
		m_loadedImage = true;

		updateWindowTitle();
		pendingUpdate();
		draw();
	}
	else if (err == 500)
	{
		m_tooBig = true;
		if (!m_image->isVideo())
		{ error(this, tr("File is too big to be displayed.\n%1").arg(m_image->url())); }
	}
	else if (err == QNetworkReply::ContentNotFoundError)
	{ showLoadingError("Image not found."); }
	else if (err == QNetworkReply::UnknownContentError)
	{ showLoadingError("Error loading the image."); }
	else if (err != QNetworkReply::OperationCanceledError)
	{ error(this, tr("An unexpected error occured loading the image (%1 - %2).\n%3").arg(err).arg(errorString).arg(m_image->url())); }
}

void ZoomWindow::showLoadingError(QString message)
{
	log(message);
	ui->labelLoadingError->setText(message);
	ui->labelLoadingError->show();
}

void ZoomWindow::pendingUpdate()
{
	// If we don't want to save, nothing to do
	if (m_mustSave == 0)
		return;

	// If the image is not even loaded, we cannot save it (unless it's a big file)
	if (!m_loadedImage && !m_tooBig)
		return;

	bool fav = (m_mustSave == 3 || m_mustSave == 4);
	Filename fn(m_settings->value("Save/path" + QString(fav ? "_favorites" : "")).toString());

	// If the image is loaded but we need their tags and we don't have them, we wait
	if (!m_loadedDetails && fn.needExactTags(m_site))
		return;

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

	m_mustSave = 0;
}

#include <QDebug>
void ZoomWindow::draw()
{
	// Videos don't get drawn
	if (m_image->isVideo())
		return;

	QString fn = m_url.section('/', -1).toLower();
	QString ext = fn.section('.', -1).toLower();

	// We need a filename to display animations, so we get it if we're not already loading from a file
	QString filename;
	if (!m_source.isEmpty())
	{ filename = m_source; }
	else if (!m_isAnimated.isEmpty())
	{
		filename = QDir::temp().absoluteFilePath("grabber-" + fn);
		QFile f(filename);
		if (f.open(QFile::WriteOnly))
		{
			f.write(m_image->data());
			f.close();
		}
	}

	// GIF (using QLabel support for QMovie)
	if (!m_isAnimated.isEmpty())
	{
		m_displayMovie = new QMovie(filename, m_isAnimated.toLatin1(), this);
		m_labelImage->setMovie(m_displayMovie);
		m_stackedWidget->setCurrentWidget(m_labelImage);
		m_displayMovie->start();

		m_displayImage = QPixmap();

		if (m_isFullscreen && m_fullScreen != nullptr && m_fullScreen->isVisible())
		{ m_fullScreen->setMovie(m_displayMovie); }
		return;
	}

	// Images
	if (!m_source.isEmpty())
	{
		m_displayImage = QPixmap();
		m_displayImage.load(m_source);

		m_loadedImage = true;
		pendingUpdate();
		update();

		if (m_isFullscreen && m_fullScreen != nullptr && m_fullScreen->isVisible())
		{ m_fullScreen->setImage(m_displayImage.scaled(QApplication::desktop()->screenGeometry().size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)); }
	}
	else
	{
		emit loadImage(m_image->data());
	}
}



/**
 * Updates the image label to use the current image.
 * @param onlySize true to update the image quickly
 */
void ZoomWindow::update(bool onlySize, bool force)
{
	// Update image alignment
	QString ext = m_url.section('.', -1).toLower();
	QString type;
	if (m_image->isVideo())
	{ type = "imagePositionVideo"; }
	else if (!m_isAnimated.isEmpty())
	{ type = "imagePositionAnimation"; }
	else
	{ type = "imagePositionImage"; }
	m_labelImage->setAlignment(getAlignments(type));

	// Only used for images
	if (m_displayImage.isNull())
		return;

	bool needScaling = (m_displayImage.width() > m_labelImage->width() || m_displayImage.height() > m_labelImage->height());
	if (needScaling && (onlySize || m_loadedImage || force))
	{
		Qt::TransformationMode mode = onlySize ? Qt::FastTransformation : Qt::SmoothTransformation;
		m_labelImage->setImage(m_displayImage.scaled(m_labelImage->width(), m_labelImage->height(), Qt::KeepAspectRatio, mode));
		m_labelImageScaled = true;
	}
	else if (m_loadedImage || force || (m_labelImageScaled && !needScaling))
	{
		m_labelImage->setImage(m_displayImage);
		m_labelImageScaled = false;
	}

	m_stackedWidget->setCurrentWidget(m_labelImage);
}

Qt::Alignment ZoomWindow::getAlignments(QString type)
{
	QString vertical = m_settings->value(type + "V", "center").toString();
	QString horizontal = m_settings->value(type + "H", "left").toString();

	Qt::Alignment vAlign = vertical == "top" ? Qt::AlignTop : (vertical == "bottom" ? Qt::AlignBottom : Qt::AlignVCenter);
	Qt::Alignment hAlign = horizontal == "left" ? Qt::AlignLeft : (horizontal == "right" ? Qt::AlignRight : Qt::AlignHCenter);

	return vAlign | hAlign;
}

void ZoomWindow::saveNQuit()
{
	if (!m_imagePath.isEmpty())
	{
		close();
		return;
	}

	setButtonState(false, SaveButtonState::Saving);
	m_mustSave = 2;
	pendingUpdate();
}
void ZoomWindow::saveNQuitFav()
{
	if (!m_imagePath.isEmpty())
	{
		close();
		return;
	}

	setButtonState(true, SaveButtonState::Saving);
	m_mustSave = 4;
	pendingUpdate();
}

void ZoomWindow::saveImage(bool fav)
{
	SaveButtonState state = fav ? m_saveButonStateFav : m_saveButonState;
	switch (state)
	{
		case SaveButtonState::Save:
			setButtonState(fav, SaveButtonState::Saving);
			m_mustSave = fav ? 3 : 1;
			pendingUpdate();
			break;

		case SaveButtonState::Saving:
			return;

		case SaveButtonState::Delete:
		{
			QFile f(m_imagePath);
			if (m_image->data().isEmpty() && f.open(QFile::ReadOnly))
			{ m_image->setData(f.readAll()); }
			f.remove();
			m_imagePath = "";
			setButtonState(fav, SaveButtonState::Save);
			break;
		}

		default:
			setButtonState(fav, SaveButtonState::Delete);
	}
}
void ZoomWindow::saveImageFav()
{ saveImage(true); }
QStringList ZoomWindow::saveImageNow(bool fav)
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
			auto *options = new optionsWindow(m_profile, parentWidget());
			//options->onglets->setCurrentIndex(3);
			options->setWindowModality(Qt::ApplicationModal);
			options->show();
			connect(options, SIGNAL(closed()), this, SLOT(saveImage()));
		}
		return QStringList();
	}

	QStringList paths;
	QMap<QString, Image::SaveResult> results = m_image->save(m_settings->value("Save/filename"+QString(fav ? "_favorites" : "")).toString(), pth, true, false, 1, true);
	auto it = results.begin();
	while (it != results.end())
	{
		Image::SaveResult res = it.value();
		paths.append(it.key());
		m_imagePath = it.key();

		switch (res)
		{
			case Image::SaveResult::Saved:
				setButtonState(fav, SaveButtonState::Saved);
				break;

			case Image::SaveResult::Copied:
				setButtonState(fav, SaveButtonState::Copied);
				break;

			case Image::SaveResult::Moved:
				setButtonState(fav, SaveButtonState::Moved);
				break;

			case Image::SaveResult::Ignored:
				setButtonState(fav, SaveButtonState::ExistsMd5);
				m_imagePath = m_profile->md5Exists(m_image->md5());
				break;

			case Image::SaveResult::AlreadyExists:
				setButtonState(fav, SaveButtonState::ExistsDisk);
				break;

			default:
				error(this, tr("Error saving image."));
				return QStringList();
				break;
		}

		++it;
	}

	if (m_mustSave == 2 || m_mustSave == 4)
		close();

	m_mustSave = 0;
	return paths;
}

QString ZoomWindow::saveImageAs()
{
	Filename format(m_settings->value("Save/filename").toString());
	QStringList filenames = format.path(*m_image, m_profile);
	QString filename = filenames.first().section(QDir::separator(), -1);
	QString lastDir = m_settings->value("Zoom/lastDir", "").toString();

	QString path = QFileDialog::getSaveFileName(this, tr("Save image"), QDir::toNativeSeparators(lastDir + "/" + filename), "Images (*.png *.gif *.jpg *.jpeg)");
	if (!path.isEmpty())
	{
		path = QDir::toNativeSeparators(path);
		m_settings->setValue("Zoom/lastDir", path.section(QDir::toNativeSeparators("/"), 0, -2));

		m_image->save(path, true, true, true, false, 1, true);
		m_imagePath = path;
	}

	return path;
}


void ZoomWindow::toggleFullScreen()
{
	if (m_isFullscreen)
		unfullScreen();
	else
		fullScreen();
}

void ZoomWindow::fullScreen()
{
	if (!m_loadedImage && m_displayMovie == nullptr)
		return;

	QString ext = m_url.section('.', -1).toLower();

	QWidget *widget;
	m_fullScreen = new QAffiche(QVariant(), 0, QColor(), this);
	m_fullScreen->setStyleSheet("background-color: black");
	m_fullScreen->setAlignment(Qt::AlignCenter);
	if (!m_isAnimated.isEmpty())
	{ m_fullScreen->setMovie(m_displayMovie); }
	else
	{ m_fullScreen->setImage(m_displayImage.scaled(QApplication::desktop()->screenGeometry().size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)); }
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
		connect(arrowNext, &QShortcut::activated, this, &ZoomWindow::next);
	QShortcut *arrowPrevious = new QShortcut(QKeySequence(Qt::Key_Left), widget);
		connect(arrowPrevious, &QShortcut::activated, this, &ZoomWindow::previous);
	QShortcut *space = new QShortcut(QKeySequence(Qt::Key_Space), widget);
		connect(space, &QShortcut::activated, this, &ZoomWindow::toggleSlideshow);

	widget->setFocus();
}

void ZoomWindow::unfullScreen()
{
	m_slideshow.stop();

	m_fullScreen->close();
	m_fullScreen->deleteLater();
	m_fullScreen = nullptr;

	m_isFullscreen = false;
}

void ZoomWindow::prepareNextSlide()
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
	if (!m_isAnimated.isEmpty())
		additionalInterval = m_displayMovie->nextFrameDelay() * m_displayMovie->frameCount();

	qint64 totalInterval = interval * 1000 + additionalInterval;
	m_slideshow.start(totalInterval);
	m_isSlideshowRunning = true;
}

void ZoomWindow::toggleSlideshow()
{
	m_isSlideshowRunning = !m_isSlideshowRunning;

	if (!m_isSlideshowRunning)
		m_slideshow.stop();
	else
		prepareNextSlide();
}



void ZoomWindow::resizeEvent(QResizeEvent *e)
{
	if (!m_resizeTimer->isActive())
	{ m_timeout = qMin(500, qMax(50, (m_displayImage.width() * m_displayImage.height()) / 100000)); }
	m_resizeTimer->stop();
	m_resizeTimer->start(m_timeout);
	update(true);

	QWidget::resizeEvent(e);
}

void ZoomWindow::closeEvent(QCloseEvent *e)
{
	m_settings->setValue("Zoom/geometry", saveGeometry());
	m_settings->setValue("Zoom/plus", ui->buttonPlus->isChecked());
	m_settings->sync();

	m_image->abortTags();
	m_image->abortImage();

	e->accept();
}

void ZoomWindow::showEvent(QShowEvent *e)
{
	Q_UNUSED(e);
	showThumbnail();
}

void ZoomWindow::showThumbnail()
{
	QSize size = m_image->size();
	if (size.isEmpty())
	{ size = m_image->previewImage().size() * 2 * m_settings->value("thumbnailUpscale", 1.0f).toFloat(); }

	// Videos get a static resizable overlay
	if (m_image->isVideo())
	{
		// A video thumbnail should not be upscaled to more than three times its size
		QSize maxSize = QSize(500, 500) * m_settings->value("thumbnailUpscale", 1.0f).toFloat();
		if (size.width() > maxSize.width() || size.height() > maxSize.height())
		{ size.scale(maxSize, Qt::KeepAspectRatio); }

		QPixmap base = m_image->previewImage();
		QPixmap overlay = QPixmap(":/images/play-overlay.png");
		QPixmap result(size.width(), size.height());
		result.fill(Qt::transparent);
		{
			QPainter painter(&result);
			painter.drawPixmap(0, 0, size.width(), size.height(), base);
			painter.drawPixmap(qMax(0, (size.width() - overlay.width()) / 2), qMax(0, (size.height() - overlay.height()) / 2), overlay.width(), overlay.height(), overlay);
		}
		m_displayImage = result;
		update(false, true);
	}

	// Gifs get non-resizable thumbnails
	else if (!m_isAnimated.isEmpty())
	{
		m_labelImage->setPixmap(m_image->previewImage().scaled(size, Qt::IgnoreAspectRatio, Qt::FastTransformation));
	}

	// Other images get a resizable thumbnail
	else if (m_displayImage.isNull())
	{
		m_displayImage = m_image->previewImage().scaled(size, Qt::IgnoreAspectRatio, Qt::FastTransformation);
		update(false, true);
	}
}

void ZoomWindow::urlChanged(QString old, QString nouv)
{
	Q_UNUSED(old);
	m_url = nouv;
}


void ZoomWindow::load(QSharedPointer<Image> image)
{
	emit clearLoadQueue();
	disconnect(m_image.data(), &Image::finishedLoadingTags, this, &ZoomWindow::replyFinishedDetails);

	m_displayImage = QPixmap();
	m_loadedImage = false;
	m_imagePath = "";
	m_image = image;
	m_isAnimated = image->isAnimated();
	connect(m_image.data(), &Image::urlChanged, this, &ZoomWindow::urlChanged, Qt::UniqueConnection);
	m_size = 0;
	ui->labelLoadingError->hide();

	// Show the thumbnail if the image was not already preloaded
	if (isVisible())
	{ showThumbnail(); }

	// Preload gallery images
	int preload = m_settings->value("preload", 0).toInt();
	if (preload > 0)
	{
		QSet<int> preloaded;
		int index = m_images.indexOf(m_image);
		for (int i = index - preload; i <= index + preload; ++i)
		{
			int pos = (i + m_images.count()) % m_images.count();
			if (pos < 0 || pos == index || pos > m_images.count() || preloaded.contains(pos))
				continue;

			preloaded.insert(pos);
			log(QString("Preloading data for image #%1").arg(pos));
			m_images[pos]->loadDetails();
			m_images[pos]->loadImage();
		}
	}

	// Reset buttons
	setButtonState(false, SaveButtonState::Save);
	setButtonState(true, SaveButtonState::Save);

	// Window title
	updateWindowTitle();

	prepareNextSlide();
	go();
}

void ZoomWindow::updateWindowTitle()
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
	QString title;
	if (infos.isEmpty())
		title = tr("Image");
	else
		title = QString(tr("Image") + " (%1)").arg(infos.join(", "));
	setWindowTitle(QString("%1 - %2 (%3/%4)").arg(title, m_image->parentSite()->name(), QString::number(m_images.indexOf(m_image) + 1), QString::number(m_images.count())));
}

int ZoomWindow::firstNonBlacklisted(int direction)
{
	int index = m_images.indexOf(m_image);
	int first = index;
	index = (index + m_images.count() + direction) % m_images.count();

	// Skip blacklisted images
	auto blacklistedtags = m_profile->getBlacklist();
	while (!PostFilter::blacklisted(m_images[index]->tokens(m_profile), blacklistedtags).isEmpty() && index != first)
	{
		index = (index + m_images.count() + direction) % m_images.count();
	}

	return index;
}

void ZoomWindow::next()
{
	m_image->abortTags();
	m_image->abortImage();

	int index = firstNonBlacklisted(+1);
	load(m_images[index]);
}

void ZoomWindow::previous()
{
	m_image->abortTags();
	m_image->abortImage();

	int index = firstNonBlacklisted(-1);
	load(m_images[index]);
}

void ZoomWindow::updateButtonPlus()
{
	ui->buttonPlus->setText(ui->buttonPlus->isChecked() ? "-" : "+");
}

void ZoomWindow::openFile(bool now)
{
	if (!now)
	{
		m_mustSave = 5;
		pendingUpdate();
		return;
	}

	QString path = m_imagePath;
	if (path.isEmpty() || !QFile::exists(path))
	{
		QMap<QString, Image::SaveResult> files = m_image->save(m_settings->value("Save/filename").toString(), m_profile->tempPath(), false, false, 1, true);
		path = files.firstKey();
	}

	QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void ZoomWindow::mouseReleaseEvent(QMouseEvent *e)
{
	if (e->button() == Qt::MiddleButton && m_settings->value("imageCloseMiddleClick", true).toBool())
	{
		close();
		return;
	}

	QWidget::mouseReleaseEvent(e);
}

void ZoomWindow::wheelEvent(QWheelEvent *e)
{
	if (m_settings->value("imageNavigateScroll", true).toBool())
	{
		// Ignore events triggered when reaching the bottom of the tag list
		if (ui->scrollArea->underMouse())
			return;

		// Ignore events if we already got one less than 500ms ago
		if (!m_lastWheelEvent.isNull() && m_lastWheelEvent.elapsed() <= 500)
			e->ignore();
		m_lastWheelEvent.start();

		int angle = e->angleDelta().y();
		if (angle <= -120) // Scroll down
		{
			next();
			return;
		}
		if (angle >= 120) // Scroll up
		{
			previous();
			return;
		}
	}

	QWidget::wheelEvent(e);
}
