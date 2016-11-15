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

#include <QMediaPlaylist>



zoomWindow::zoomWindow(QSharedPointer<Image> image, Site *site, QMap<QString,Site*> *sites, Profile *profile, mainWindow *parent)
	: QDialog(Q_NULLPTR, Qt::Window), m_parent(parent), m_profile(profile), m_favorites(profile->getFavorites()), m_viewItLater(profile->getKeptForLater()), m_ignore(profile->getIgnored()), m_settings(profile->getSettings()), ui(new Ui::zoomWindow), m_site(site), timeout(300), m_loaded(false), m_loadedImage(false), m_loadedDetails(false), image(nullptr), movie(nullptr), m_reply(nullptr), m_finished(false), m_thread(false), m_data(QByteArray()), m_size(0), m_sites(sites), m_source(), m_th(nullptr), m_fullScreen(nullptr)
{
	m_imageTime = nullptr;

	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);

	m_image = image;
	connect(m_image.data(), &Image::urlChanged, this, &zoomWindow::urlChanged);

	m_mustSave = 0;

	restoreGeometry(m_settings->value("Zoom/geometry").toByteArray());
	ui->buttonPlus->setChecked(m_settings->value("Zoom/plus", false).toBool());
	ui->progressBarDownload->hide();

	QShortcut *escape = new QShortcut(QKeySequence(Qt::Key_Escape), this);
		connect(escape, SIGNAL(activated()), this, SLOT(close()));
	QShortcut *save = new QShortcut(QKeySequence::Save, this);
		connect(save, SIGNAL(activated()), this, SLOT(saveImage()));
	QShortcut *saveAs = new QShortcut(QKeySequence("Ctrl+Shift+S"), this);
		connect(saveAs, SIGNAL(activated()), this, SLOT(saveImageAs()));

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

	go();
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

	m_stackedWidget = new QStackedWidget(this);
		ui->verticalLayout->insertWidget(1, m_stackedWidget, 1);
	m_labelImage = new QAffiche(QVariant(), 0, QColor(), this);
		m_labelImage->setSizePolicy(QSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored));
		connect(m_labelImage, SIGNAL(doubleClicked()), this, SLOT(fullScreen()));
		m_stackedWidget->addWidget(m_labelImage);
	m_mediaPlayer = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
		m_videoWidget = new QVideoWidget(this);
		m_stackedWidget->addWidget(m_videoWidget);
		m_mediaPlayer->setVideoOutput(m_videoWidget);

	QMap<QString, QString> assoc;
		assoc["s"] = tr("Safe");
		assoc["q"] = tr("Questionable");
		assoc["e"] = tr("Explicit");
	QString ext = getExtension(m_image->url());
	m_url = (!m_settings->value("Save/downloadoriginals", true).toBool() && !m_image->sampleUrl().isEmpty()) || ext == "zip"
			? m_image->sampleUrl().toString()
			: m_image->fileUrl().toString();

	m_format = m_url.section('.', -1).toUpper().toLatin1().data();

	QTimer *timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()), this, SLOT(update()));
		timer->setSingleShot(true);
		m_resizeTimer = timer;

	QString u = m_site->value("Urls/Html/Post");
		u.replace("{id}", QString::number(m_image->id()));

	connect(ui->buttonDetails, SIGNAL(clicked()), this, SLOT(showDetails()));

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
	}

	m_detailsWindow = new detailsWindow(m_profile, this);

	// Load image details (exact tags & co)
	connect(m_image.data(), &Image::finishedLoadingTags, this, &zoomWindow::replyFinishedDetails);
	m_image->loadDetails();

	activateWindow();
}

/**
 * Destructor of the zoomWindow class
 */
zoomWindow::~zoomWindow()
{
	if (m_imageTime != nullptr)
		delete m_imageTime;
	if (image != nullptr)
		delete image;
	if (movie != nullptr)
		movie->deleteLater();

	m_labelTagsTop->deleteLater();
	m_labelTagsLeft->deleteLater();
	m_detailsWindow->deleteLater();

	delete ui;
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

	ui->progressBarDownload->setMaximum(100);
	ui->progressBarDownload->setValue(0);
	ui->progressBarDownload->show();

	m_imageTime = new QTime;
	m_imageTime->start();

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
	ui->progressBarDownload->setMaximum(bytesTotal);
	ui->progressBarDownload->setValue(bytesReceived);

	if (m_imageTime->elapsed() > TIME || (bytesTotal > 0 && bytesReceived / bytesTotal > PERCENT))
	{
		m_imageTime->restart();
		m_data.append(m_reply->readAll());
		m_thread = true;
		m_th = new ImageThread(m_data);
		connect(m_th, SIGNAL(finished(QImage, int)), this, SLOT(display(QImage, int)));
		connect(m_th, SIGNAL(finished()), m_th, SLOT(deleteLater()));
		m_th->start();
	}
}
void zoomWindow::display(QImage pix, int size)
{
	if (!pix.size().isEmpty() && size >= m_size)
	{
		m_size = size;
		delete image;
		image = new QPixmap(QPixmap::fromImage(pix));
		if (m_url.section('.', -1).toLower() == "gif")
		{ /*m_labelImage->setPixmap(*image);*/ }
		else
		{ update(!m_finished); }
		m_thread = false;

		if (m_fullScreen != nullptr && m_fullScreen->isVisible())
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
	delete m_imageTime;
	m_imageTime = nullptr;

	ui->progressBarDownload->hide();

	// Check redirection
	QUrl redir = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redir.isEmpty())
	{
		m_data.clear();
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
			m_url = m_url.section('.', 0, -2) + "." + newext;
			m_image->setFileExtension(newext);
			log(QString("Image not found. New try with extension %1...").arg(newext));
			load();
			return;
		}
	}
	else if (m_reply->error() != QNetworkReply::OperationCanceledError)
	{ error(this, tr("An unexpected error occured loading the image (%1).\r\n%2").arg(m_reply->error()).arg(m_reply->url().toString())); }

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
	}
}

void zoomWindow::draw()
{
	QString fn = m_url.section('/', -1).toLower();
	QString ext = fn.section('.', -1).toLower();
	bool isVideo = ext == "mp4" || ext == "webm" || ext == "flv";

	// We need a filename to display gifs and videos, so we get it if we're not already loading from a file
	QString filename;
	if (m_source.isEmpty())
	{
		if (ext == "gif" || isVideo)
		{
			filename = QDir::temp().absoluteFilePath("grabber-" + fn);
			QFile f(filename);
			if (f.open(QFile::WriteOnly))
			{
				f.write(m_data);
				f.close();
			}
		}
	}
	else
	{ filename = m_source; }

	// GIF (using QLabel support for QMovie)
	if (ext == "gif")
	{
		this->movie = new QMovie(filename, QByteArray(), this);
		m_labelImage->setMovie(this->movie);
		m_stackedWidget->setCurrentWidget(m_labelImage);
		this->movie->start();

		this->image = nullptr;
		return;
	}

	// Videos (using a media player)
	if (isVideo)
	{
		QMediaPlaylist *playlist = new QMediaPlaylist(this);
		playlist->addMedia(QUrl::fromLocalFile(filename));
		playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);

		m_mediaPlayer->setPlaylist(playlist);
		m_stackedWidget->setCurrentWidget(m_videoWidget);
		m_mediaPlayer->play();

		this->image = nullptr;
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
	}
	else
	{
		m_thread = true;
		m_th = new ImageThread(m_data);
		connect(m_th, SIGNAL(finished(QImage, int)), this, SLOT(display(QImage, int)));
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
	// Ignore this event for GIF and videos
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
	}

	return path;
}



void zoomWindow::fullScreen()
{
	if (image == nullptr && movie == nullptr)
		return;

	QString ext = m_url.section('.', -1).toLower();
	bool isVideo = ext == "mp4" || ext == "webm" || ext == "flv";

	if (isVideo)
	{
		m_videoWidget->setFullScreen(true);
	}
	else
	{
		m_fullScreen = new QAffiche(QVariant(), 0, QColor(), this);
		m_fullScreen->setStyleSheet("background-color: black");
		m_fullScreen->setAlignment(Qt::AlignCenter);
		if (ext == "gif")
		{ m_fullScreen->setMovie(movie); }
		else
		{ m_fullScreen->setImage(image->scaled(QApplication::desktop()->screenGeometry().size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)); }
		m_fullScreen->setWindowFlags(Qt::Window);
		m_fullScreen->showFullScreen();

		connect(m_fullScreen, SIGNAL(doubleClicked()), m_fullScreen, SLOT(close()));
	}

	QShortcut *escape = new QShortcut(QKeySequence(Qt::Key_Escape), this);
	connect(escape, SIGNAL(activated()), this, SLOT(unfullScreen()));
}

void zoomWindow::unfullScreen()
{
	QString ext = m_url.section('.', -1).toLower();
	bool isVideo = ext == "mp4" || ext == "webm" || ext == "flv";

	if (isVideo)
	{
		m_videoWidget->setFullScreen(false);
	}
	else
	{
		m_fullScreen->close();
		m_fullScreen->deleteLater();
		m_fullScreen = nullptr;
	}
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
	/*if (m_thread && m_th->isRunning())
	{ m_th->quit(); }*/
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
