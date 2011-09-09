#include <string>
#include <QApplication>
#include <QtNetwork>
#include "functions.h"
#include "optionswindow.h"
#include "QAffiche.h"
#include "zoomwindow.h"
#include "imagethread.h"
#include "ui_zoomwindow.h"

extern mainWindow *_mainwindow;



zoomWindow::zoomWindow(Image *image, QStringMap site) : ui(new Ui::zoomWindow), m_image(image), m_site(site), timeout(300), loaded(0), oldsize(0), m_program(qApp->arguments().at(0)), m_replyExists(false), m_finished(false), m_size(0)
{
	ui->setupUi(this);
	m_favorites = loadFavorites().keys();
	m_viewItLater = loadViewItLater();

	m_mustSave = 0;

	QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
		settings.beginGroup("Zoom");
		this->setWindowState(Qt::WindowStates(settings.value("state", 0).toInt()));
		if (!this->isMaximized())
		{
			this->resize(settings.value("size", QSize(800, 600)).toSize());
			this->move(settings.value("pos", QPoint(200, 200)).toPoint());
		}
		settings.endGroup();

	if (settings.value("autodownload", false).toBool())
	{ saveImage(); }

	QAffiche *labelImage = new QAffiche;
		labelImage->setSizePolicy(QSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored));
		connect(labelImage, SIGNAL(doubleClicked()), this, SLOT(fullScreen()));
	ui->verticalLayout->insertWidget(1, labelImage, 1);
	this->labelImage = labelImage;

	QList<Tag*> taglist = image->tags();
	QStringList hreftags;
	for (int i = 0; i < taglist.count(); i++)
	{ hreftags.append("<a href=\""+taglist.at(i)->text()+"\" style=\"text-decoration:none;color:#000000\">"+taglist.at(i)->text()+"</a>"); }

	QMap<QString, QString> assoc;
		assoc["s"] = tr("Safe");
		assoc["q"] = tr("Questionable");
		assoc["e"] = tr("Explicit");
	m_url = settings.value("Save/downloadoriginals", true).toBool() ? m_image->fileUrl().toString() : (m_image->sampleUrl().isEmpty() ? m_image->fileUrl().toString() : m_image->sampleUrl().toString());

	m_format = m_url.section('.', -1).toUpper().toAscii().data();
	
	QTimer *timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()), this, SLOT(update()));
		timer->setSingleShot(true);
		this->timer = timer;

	QString u = m_site["Urls/Html/Post"];
		u.replace("{id}", QString::number(image->id()));
	m_detailsWindow = new detailsWindow(m_image);
	connect(ui->buttonDetails, SIGNAL(clicked()), m_detailsWindow, SLOT(show()));

	QString pos = settings.value("tagsposition", "top").toString();
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
		ui->labelTagsTop->setText(hreftags.join(" "));
	}
	else
	{
		ui->labelTagsTop->hide();
		ui->labelTagsLeft->setText(hreftags.join("<br/>"));
	}

	connect(image, SIGNAL(finishedLoadingTags(Image*)), this, SLOT(replyFinished(Image*)));
	image->loadTags();
}

/**
 * Destructor of the zoomWindow class
 */
zoomWindow::~zoomWindow()
{
	delete ui;
}

void zoomWindow::openUrl(QString url)
{
	emit linkClicked(url);
}
void zoomWindow::openSaveDir()
{
	QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
	QString path = settings.value("Save/path").toString().replace("\\", "/");
	if (path.right(1) == "/")
	{ path = path.left(path.length()-1); }
	path = QDir::toNativeSeparators(path);
	QString file = m_image->path(), pth = file.section(QDir::toNativeSeparators("/"), 0, -2), url = path+QDir::toNativeSeparators("/")+pth;
	QDir dir(url);
	if (dir.exists())
	{ showInGraphicalShell(url); }
	else
	{
		int reply = QMessageBox::question(this, tr("Dossier inexistant"), tr("Le dossier de sauvegarde n'existe pas encore. Le creer ?"), QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::Yes)
		{
			QDir dir(path);
			if (!dir.mkpath(pth))
			{ error(this, tr("Erreur lors de la cr�ation du dossier.\r\n%1").arg(url)); }
			showInGraphicalShell(url);
		}
	}
}

void zoomWindow::linkHovered(QString url)
{ this->link = url; }
void zoomWindow::contextMenu(QPoint point)
{
	QMenu *menu = new QMenu(this);
	if (!this->link.isEmpty())
	{
		if (m_favorites.contains(link, Qt::CaseInsensitive))
		{
			menu->addAction(QIcon(":/images/icons/remove.png"), tr("Retirer des favoris"), this, SLOT(unfavorite()));
			menu->addAction(QIcon(":/images/icons/save.png"), tr("Choisir comme image"), this, SLOT(setfavorite()));
		}
		else
		{ menu->addAction(QIcon(":/images/icons/add.png"), tr("Ajouter aux favoris"), this, SLOT(favorite())); }
		if (m_viewItLater.contains(link, Qt::CaseInsensitive))
		{ menu->addAction(QIcon(":/images/icons/remove.png"), tr("Ne pas garder pour plus tard"), this, SLOT(unviewitlater())); }
		else
		{ menu->addAction(QIcon(":/images/icons/add.png"), tr("Garder pour plus tard"), this, SLOT(viewitlater())); }
		menu->addSeparator();
		menu->addAction(QIcon(":/images/icons/tab.png"), tr("Ouvrir dans un nouvel onglet"), this, SLOT(openInNewTab()));
		menu->addAction(QIcon(":/images/icons/window.png"), tr("Ouvrir dans une nouvelle fen�tre"), this, SLOT(openInNewWindow()));
	}
	menu->exec(QCursor::pos());
}
void zoomWindow::openInNewTab()
{ _mainwindow->addTab(link); }
void zoomWindow::openInNewWindow()
{
	QProcess myProcess;
	myProcess.startDetached(m_program, QStringList(link));
}
void zoomWindow::favorite()
{
	QString image = saveImage();
	m_favorites.append(link);
	QFile f(savePath("favorites.txt"));
		f.open(QIODevice::WriteOnly | QIODevice::Append);
		f.write(QString(link+"|50|"+QDateTime::currentDateTime().toString(Qt::ISODate)+"\r\n").toAscii());
	f.close();
	QPixmap img = image;
	if (img.width() > 150 || img.height() > 150)
	{ img = img.scaled(QSize(150,150), Qt::KeepAspectRatio, Qt::SmoothTransformation); }
	if (!QDir(savePath("thumbs")).exists())
	{ QDir(savePath()).mkdir("thumbs"); }
	img.save(savePath("thumbs/"+link+".png"), "PNG");
	_mainwindow->updateFavorites();
}
void zoomWindow::setfavorite()
{
	QPixmap img = image;
	if (img.width() > 150 || img.height() > 150)
	{ img = img.scaled(QSize(150,150), Qt::KeepAspectRatio, Qt::SmoothTransformation); }
	if (!QDir(savePath("thumbs")).exists())
	{ QDir(savePath()).mkdir("thumbs"); }
	img.save(savePath("thumbs/"+link+".png"), "PNG");
	_mainwindow->updateFavorites();
}
void zoomWindow::unfavorite()
{
	m_favorites.removeAll(link);
	QFile f(savePath("favorites.txt"));
	f.open(QIODevice::ReadOnly);
		QString favs = f.readAll();
	f.close();
	favs.replace("\r\n", "\n").replace("\r", "\n").replace("\n", "\r\n");
	QRegExp reg(link+"\\|(.+)\\r\\n");
	reg.setMinimal(true);
	favs.remove(reg);
	f.open(QIODevice::WriteOnly);
		f.write(favs.toAscii());
	f.close();
	if (QFile::exists(savePath("thumbs/"+link+".png")))
	{ QFile::remove(savePath("thumbs/"+link+".png")); }
	_mainwindow->updateFavorites();
}
void zoomWindow::viewitlater()
{
	m_viewItLater.append(link);
	QFile f(savePath("viewitlater.txt"));
	f.open(QIODevice::WriteOnly);
		f.write(m_viewItLater.join("\r\n").toAscii());
	f.close();
}
void zoomWindow::unviewitlater()
{
	m_viewItLater.removeAll(link);
	QFile f(savePath("viewitlater.txt"));
	f.open(QIODevice::WriteOnly);
		f.write(m_viewItLater.join("\r\n").toAscii());
	f.close();
}



void zoomWindow::load()
{
	log(tr("Chargement de l'image depuis <a href=\"%1\">%1</a>").arg(m_url));
	m_data = QByteArray();
	QNetworkRequest request = QNetworkRequest(QUrl(m_url));
		request.setRawHeader("Referer", m_url.toAscii());
	QNetworkAccessManager *manager = new QNetworkAccessManager;
	m_reply = manager->get(request);
	connect(m_reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(downloadProgress(qint64, qint64)));
	connect(m_reply, SIGNAL(finished()), this, SLOT(replyFinishedZoom()));
	m_replyExists = true;
}

#define UPDATES 16
void zoomWindow::downloadProgress(qint64 size, qint64 total)
{
	if (size-m_data.size() >= total/UPDATES)
	{
		m_data.append(m_reply->readAll());
		ImageThread *th = new ImageThread(m_data);
		connect(th, SIGNAL(finished(QPixmap, int)), this, SLOT(display(QPixmap, int)));
		th->start();
	}
}
void zoomWindow::display(QPixmap pix, int size)
{
	if (!pix.size().isEmpty() && size > m_size)
	{
		m_size = size;
		this->image = pix;
		this->update(!m_finished);
	}
}

void zoomWindow::replyFinished(Image* img)
{
	QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
	bool under = settings.value("Save/remplaceblanksbyunderscores", false).toBool();
	QStringList blacklistedtags(settings.value("blacklistedtags").toString().split(' '));

	QStringList tlist = QStringList() << "artists" << "copyrights" << "characters" << "models" << "generals";
	QMap<QString,QString> styles;
	for (int i = 0; i < tlist.size(); i++)
	{
		QFont font;
		font.fromString(settings.value("Coloring/Fonts/"+tlist.at(i)).toString());
		styles[tlist.at(i)] = "color:"+settings.value("Coloring/Colors/"+tlist.at(i), "#00aa00").toString()+"; "+qfonttocss(font);
	}

	QStringList t;
	for (int i = 0; i < img->tags().size(); i++)
	{
		Tag *tag = img->tags().at(i);
		QString normalized = tag->text().replace("\\", " ").replace("/", " ").replace(":", " ").replace("|", " ").replace("*", " ").replace("?", " ").replace("\"", " ").replace("<", " ").replace(">", " ").trimmed();
		if (under)
		{ normalized.replace(' ', '_'); }
		this->details[tag->type()+"s"].append(normalized);
		this->details["alls"].append(normalized);
		if (blacklistedtags.contains(tag->text(), Qt::CaseInsensitive))
		{ t.append("<a href=\""+tag->text()+"\" style=\"\">"+tag->text()+"</a>"); }
		else
		{ t.append("<a href=\""+tag->text()+"\" style=\""+(styles.contains(tag->type()+"s") ? styles[tag->type()+"s"] : styles["generals"])+"\">"+tag->text()+"</a>"); }
	}
	tags = t.join(" ");
	if (ui->widgetLeft->isHidden())
	{ ui->labelTagsTop->setText(tags); }
	else
	{ ui->labelTagsLeft->setText(t.join("<br/>")); }
	m_detailsWindow->setTags(tags);

	QString pth = m_image->path();
	QString path = settings.value("Save/path").toString().replace("\\", "/");
	if (path.right(1) == "/")
	{ path = path.left(path.length()-1); }
	QFile file(path+"/"+pth);
	if (file.exists() && !path.isEmpty() && !pth.isEmpty())
	{
		ui->buttonSave->setText(tr("Fichier d�j� existant"));
		ui->buttonSaveNQuit->setText(tr("Fermer"));
		this->loaded = true;
		if (m_url.section('.', -1).toUpper() == "GIF")
		{
			QMovie *movie = new QMovie(path+"/"+pth);
			labelImage->setMovie(movie);
			movie->start();
		}
		else
		{
			m_data.clear();
			if (!file.open(QIODevice::ReadOnly))
			{ error(this, tr("Erreur inattendue lors de l'ouverture du fichier.\r\n%1").arg(path+"/"+pth)); }
			m_data = file.readAll();
			this->image.loadFromData(m_data, m_format);
			this->update();
		}
	}
	else
	{ this->load(); }
}

void zoomWindow::replyFinishedZoom()
{
	log(tr("Image re�ue depuis <a href=\"%1\">%1</a>").arg(m_url));
	m_finished = true;
	if (m_reply->error() == QNetworkReply::NoError)
	{
		m_data.append(m_reply->readAll());
		this->loaded = true;
		if (m_url.section('.', -1).toUpper() == "GIF")
		{
			QTemporaryFile f;
			if (f.open())
			{
				f.write(m_data);
				f.close();
				QMovie *movie = new QMovie(f.fileName());
				labelImage->setMovie(movie);
				movie->start();
			}
			else
			{ error(this, tr("Une erreur inattendue est survenue lors du chargement de l'image GIF.\r\n%1").arg(m_reply->url().toString())); }
		}
		else
		{
			qDebug() << "startfinal";
			ImageThread *th = new ImageThread(m_data);
			connect(th, SIGNAL(finished(QPixmap, int)), this, SLOT(display(QPixmap, int)));
			th->start();
		}
		if (this->m_mustSave > 0)
		{ this->saveImage(); }
	}
	else if (m_reply->error() == QNetworkReply::ContentNotFoundError && m_url.section('.', -1) != "jpeg")
	{
		QString ext = m_url.section('.', -1);
		QMap<QString,QString> nextext;
		nextext["jpg"] = "png";
		nextext["png"] = "gif";
		nextext["gif"] = "jpeg";
		m_url = m_url.section('.', 0, -2)+"."+nextext[ext];
		log(tr("Image non trouv�e. Nouvel essai avec l'extension %1...").arg(nextext[ext]));
		load();
	}
	else if (m_reply->error() != QNetworkReply::OperationCanceledError)
	{ error(this, tr("Une erreur inattendue est survenue lors du chargement de l'image.\r\n%1").arg(m_reply->url().toString())); }
	m_reply->abort();
}



void zoomWindow::update(bool onlysize)
{
	if (m_url.section('.', -1).toUpper() != "GIF")
	{
		if (onlysize && (this->image.width() > this->labelImage->width() || this->image.height() > this->labelImage->height()))
		{ this->labelImage->setImage(this->image.scaled(this->labelImage->width(), this->labelImage->height(), Qt::KeepAspectRatio, Qt::FastTransformation)); }
		else if (this->loaded)
		{
			if (this->image.width() > this->labelImage->width() || this->image.height() > this->labelImage->height())
			{ this->labelImage->setImage(this->image.scaled(this->labelImage->width(), this->labelImage->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation)); }
			else
			{ this->labelImage->setImage(this->image); }
		}
	}
}

void zoomWindow::saveNQuit()
{
	if (loaded)
	{
		if (!this->saveImage().isEmpty())
		{ this->close(); }
	}
	else
	{
		ui->buttonSaveNQuit->setText(tr("Sauvegarde..."));
		m_mustSave = 2;
	}
}

QString zoomWindow::saveImage()
{
	if (!loaded) // If image is still loading, we wait for it to finish
	{
		ui->buttonSave->setText(tr("Sauvegarde..."));
		m_mustSave = 1;
		return QString();
	}
	QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
	QString pth = settings.value("Save/path").toString().replace("\\", "/");
	if (pth.right(1) == "/")
	{ pth = pth.left(pth.length()-1); }
	QString path = m_image->path();
	if (pth.isEmpty() || settings.value("Save/filename").toString().isEmpty())
	{
		int reply;
		if (pth.isEmpty())
		{ reply = QMessageBox::question(this, tr("Erreur"), tr("Vous n'avez pas pr�cis� de dossier de sauvegarde ! Voulez-vous ouvrir les options ?"), QMessageBox::Yes | QMessageBox::No); }
		else
		{ reply = QMessageBox::question(this, tr("Erreur"), tr("Vous n'avez pas pr�cis� de format de sauvegarde ! Voulez-vous ouvrir les options ?"), QMessageBox::Yes | QMessageBox::No); }
		if (reply == QMessageBox::Yes)
		{
			optionsWindow *options = new optionsWindow(_mainwindow);
			//options->onglets->setCurrentIndex(3);
			options->setWindowModality(Qt::ApplicationModal);
			options->show();
			connect(options, SIGNAL(closed()), this, SLOT(saveImage()));
		}
		return QString();
	}
	QString fp = QDir::toNativeSeparators(pth+"/"+path);
	QFile f(fp);
	if (!f.exists())
	{
		QProcess *p = new QProcess;
		if (!settings.value("Exec/Group/init").toString().isEmpty())
		{
			log(tr("Execution de la commande d'initialisation' \"%1\"").arg(settings.value("Exec/Group/init").toString()));
			p->start(settings.value("Exec/Group/init").toString());
			if (!p->waitForStarted(10000))
			{ log(tr("<b>Erreur :</b> %1").arg(tr("erreur lors de la commande d'initialisation : %1.").arg("timed out"))); }
		}
		QDir path_to_file(fp.section(QDir::toNativeSeparators("/"), 0, -2));
		if (!path_to_file.exists())
		{
			QDir dir(pth);
			if (!dir.mkpath(path.section(QDir::toNativeSeparators("/"), 0, -2)))
			{ error(this, tr("Erreur lors de la sauvegarde de l'image.\r\n%1").arg(fp)); return QString(); }
		}
		f.open(QIODevice::WriteOnly);
			f.write(m_data);
			log(tr("Sauvegarde de l'image dans le fichier <a href=\"file:///%1\">%1</a>").arg(f.fileName()));
		f.close();

		QMap<QString,int> types;
		types["general"] = 0;
		types["artist"] = 1;
		types["general"] = 2;
		types["copyright"] = 3;
		types["character"] = 4;
		types["model"] = 5;
		types["photo_set"] = 6;
		for (int i = 0; i < m_image->tags().count(); i++)
		{
			Tag *tag = m_image->tags().at(i);
			QString original = tag->text().replace(" ", "_");
			if (!settings.value("Exec/tag").toString().isEmpty())
			{
				QString exec = settings.value("Exec/tag").toString()
				.replace("%tag%", original)
				.replace("%type%", tag->type())
				.replace("%number%", QString::number(types[tag->type()]));
				log(tr("Execution seule de \"%1\"").arg(exec));
				QProcess::execute(exec);
			}
			if (!settings.value("Exec/Group/tag").toString().isEmpty())
			{
				QString exec = settings.value("Exec/Group/tag").toString()
				.replace("%tag%", original)
				.replace("%type%", tag->type())
				.replace("%number%", QString::number(types[tag->type()]));
				log(tr("Execution group�e de \"%1\"").arg(exec));
				p->write(exec.toAscii());
			}
		}
		if (!settings.value("Exec/image").toString().isEmpty())
		{
			QString exec = m_image->path(settings.value("Exec/image").toString());
			exec.replace("%path%", fp);
			exec.replace(" \\C ", " /C ");
			log(tr("Execution seule de \"%1\"").arg(exec));
			QProcess::execute(exec);
		}
		if (!settings.value("Exec/Group/image").toString().isEmpty())
		{
			QString exec = m_image->path(settings.value("Exec/Group/image").toString());
			exec.replace("%path%", fp);
			log(tr("Execution group�e de \"%1\"").arg(exec));
			p->write(exec.toAscii());
		}

		log(tr("Sauvegard� <a href=\"file:///%1\">%1</a>").arg(pth+"/"+path));
		ui->buttonSave->setText(tr("Sauvegard� !"));
		ui->buttonSaveNQuit->setText(tr("Fermer"));
		if (!settings.value("Exec/Group/init").toString().isEmpty())
		{
			p->closeWriteChannel();
			p->waitForFinished(1000);
			p->close();
		}
	}
	else
	{ ui->buttonSave->setText(tr("Fichier d�j� existant")); }
	if (m_mustSave == 2)
	{ close(); }
	m_mustSave = 0;
	return pth+"/"+path;
}

QString zoomWindow::saveImageAs()
{
	QString path = QFileDialog::getSaveFileName(this, tr("Enregistrer l'image"), m_image->fileUrl().toString().section('/', -1), "Images (*.png *.gif *.jpg *.jpeg)");
	QFile f(path);
	f.open(QIODevice::WriteOnly);
	f.write(m_data);
	f.close();
	return path;
}



void zoomWindow::fullScreen()
{
	QAffiche *label = new QAffiche();
		label->setStyleSheet("background-color: black");
		label->setAlignment(Qt::AlignCenter);
		label->setImage(this->image.scaled(QApplication::desktop()->screenGeometry().size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
		label->showFullScreen();
	QShortcut *escape= new QShortcut(QKeySequence(Qt::Key_Escape), label);
		connect(escape, SIGNAL(activated()), label, SLOT(close()));
	connect(label, SIGNAL(doubleClicked()), label, SLOT(close()));
}



void zoomWindow::resizeEvent(QResizeEvent *e)
{
	if (this->loaded)
	{
		if (!this->timer->isActive())
		{
			this->timeout = this->image.width()*this->image.height()/100000;
			if (this->timeout < 50)		{ this->timeout = 50;	}
			if (this->timeout > 500)	{ this->timeout = 500;	}
		}
		this->timer->stop();
		this->timer->start(this->timeout);
		this->update(true);
	}
	QWidget::resizeEvent(e);
}

void zoomWindow::closeEvent(QCloseEvent *e)
{
	QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
		settings.beginGroup("Zoom");
		settings.setValue("state", int(this->windowState()));
		settings.setValue("size", this->size());
		settings.setValue("pos", this->pos());
	m_image->abortTags();
	if (m_replyExists)
	{
		if (m_reply->isRunning())
		{
			m_reply->abort();
			log(tr("Chargement de l'image stopp�."));
		}
	}
	e->accept();
}
