#include <string>
#include <QApplication>
#include <QtNetwork>
#include "functions.h"
#include "optionsWindow.h"
#include "QAffiche.h"
#include "zoomWindow.h"
#include "ui_zoomWindow.h"

using namespace std;



zoomWindow::zoomWindow(QString m_program, QString site, QStringList regex, QMap<QString,QString> details, mainWindow *parent) : m_parent(parent), ui(new Ui::zoomWindow), regex(regex), m_details(details), timeout(300), loaded(0), oldsize(0), site(site), m_program(m_program), m_mustSave(false), m_replyExists(false)
{
	ui->setupUi(this);
	favorites = loadFavorites().keys();

	QSettings settings("settings.ini", QSettings::IniFormat);
		settings.beginGroup("Zoom");
		this->setWindowState(Qt::WindowStates(settings.value("state", 0).toInt()));
		if (!this->isMaximized())
		{
			this->resize(settings.value("size", QSize(800, 600)).toSize());
			this->move(settings.value("pos", QPoint(200, 200)).toPoint());
		}

	QAffiche *labelImage = new QAffiche;
		labelImage->setSizePolicy(QSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored));
		connect(labelImage, SIGNAL(doubleClicked()), this, SLOT(fullScreen()));
	ui->verticalLayout->insertWidget(1, labelImage, 2);
	this->labelImage = labelImage;

	QStringList taglist = m_details.value("tags").split(' ');
	QString hreftags;
	for (int i = 0; i < taglist.count(); i++)
	{ hreftags += " <a href=\""+taglist.at(i)+"\" style=\"text-decoration:none;color:#000000\">"+taglist.at(i)+"</a>"; }
	ui->labelTags->setText(hreftags.trimmed());

	QMap<QString, QString> assoc;
		assoc["s"] = tr("Safe");
		assoc["q"] = tr("Questionable");
		assoc["e"] = tr("Explicit");
	this->rating = assoc.value(m_details.value("rating"));
	this->url = settings.value("Save/downloadoriginals", true).toBool() ? m_details.value("file_url") : (m_details.value("sample_url").isEmpty() ? m_details.value("file_url") : m_details.value("sample_url"));

	this->format = this->url.section('.', -1).toUpper().toAscii().data();
	
	QTimer *timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()), this, SLOT(update()));
		timer->setSingleShot(true);
		this->timer = timer;

	m_detailsWindow = new detailsWindow(m_details);
	connect(ui->buttonDetails, SIGNAL(clicked()), m_detailsWindow, SLOT(show()));

	QString u = this->regex.at(4);
		u.replace("{id}", m_details.value("id"));
	QUrl rl(u);
	QNetworkAccessManager *manager = new QNetworkAccessManager(this);
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
	QNetworkRequest rq(rl);
		rq.setRawHeader("Referer", u.toAscii());
	this->r = manager->get(rq);
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
	m_parent->setTags(url);
	m_parent->webUpdate();
	m_parent->activateWindow();
}
void zoomWindow::openSaveDir()
{
	QSettings settings("settings.ini", QSettings::IniFormat);
	QString path = settings.value("Save/path").toString().replace("\\", "/");
	if (path.right(1) == "/")
	{ path = path.left(path.length()-1); }
	QString pth = this->getSavePath().section('/', 0, -2), url = path+"/"+pth;
	QDir dir(url);
	if (dir.exists())
	{ QDesktopServices::openUrl(QUrl("file:///"+url)); }
	else
	{
		int reply = QMessageBox::question(this, tr("Dossier inexistant"), tr("Le dossier de sauvegarde n'existe pas encore. Le creer ?"), QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::Yes)
		{
			QDir dir(path);
			if (!dir.mkpath(pth))
			{ error(this, tr("Erreur lors de la création du dossier.\r\n%1").arg(url)); }
			QDesktopServices::openUrl(QUrl("file:///"+url));
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
		if (favorites.contains(link, Qt::CaseInsensitive))
		{
			menu->addAction(QIcon(":/images/icons/remove.png"), tr("Retirer des favoris"), this, SLOT(unfavorite()));
			menu->addAction(QIcon(":/images/icons/save.png"), tr("Choisir comme image"), this, SLOT(setfavorite()));
		}
		else
		{ menu->addAction(QIcon(":/images/icons/add.png"), tr("Ajouter aux favoris"), this, SLOT(favorite())); }
		menu->addAction(QIcon(":/images/icons/window.png"), tr("Ouvrir dans une nouvelle fenêtre"), this, SLOT(openInNewWindow()));
	}
	menu->exec(QCursor::pos());
}
void zoomWindow::openInNewWindow()
{
	QProcess myProcess;
	myProcess.startDetached(m_program, QStringList(link));
}
void zoomWindow::favorite()
{
	QString image = saveImage();
	favorites.append(link);
	QFile f("favorites.txt");
		f.open(QIODevice::WriteOnly | QIODevice::Append);
		f.write(QString(link+"|50|"+QDateTime::currentDateTime().toString(Qt::ISODate)+"\r\n").toAscii());
	f.close();
	QPixmap img = image;
	if (img.width() > 150 || img.height() > 150)
	{ img = img.scaled(QSize(150,150), Qt::KeepAspectRatio, Qt::SmoothTransformation); }
	img.save("thumbs/"+link+".png", "PNG");
	m_parent->updateFavorites();
}
void zoomWindow::setfavorite()
{
	QPixmap img = image;
	if (img.width() > 150 || img.height() > 150)
	{ img = img.scaled(QSize(150,150), Qt::KeepAspectRatio, Qt::SmoothTransformation); }
	img.save("thumbs/"+link+".png", "PNG");
	m_parent->updateFavorites();
}
void zoomWindow::unfavorite()
{
	favorites.removeAll(link);
	QFile f("favorites.txt");
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
	if (QFile::exists("thumbs/"+link+".png"))
	{ QFile::remove("thumbs/"+link+".png"); }
	m_parent->updateFavorites();
}



void zoomWindow::load()
{
	QNetworkRequest request(QUrl(this->url));
		request.setRawHeader("Referer", this->url.toAscii());
	QEventLoop *q = new QEventLoop;
	QNetworkAccessManager *manager = new QNetworkAccessManager;
	m_replyExists = true;
	m_reply = manager->get(request);
	connect(m_reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(rR(qint64, qint64)));
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinishedZoom(QNetworkReply*)));
	connect(manager, SIGNAL(finished(QNetworkReply*)), q, SLOT(quit()));
	q->exec();
}

#define UPDATES 20
void zoomWindow::rR(qint64 size, qint64 total)
{
	if (m_reply->size() >= (total/UPDATES) || size == total)
	{
		this->d.append(m_reply->readAll());
		if (!this->d.isEmpty())
		{
			this->image.loadFromData(this->d, m_format);
		}
		this->update(true);
	}
}

void zoomWindow::replyFinished(QNetworkReply* reply)
{
	if (reply->error() == QNetworkReply::NoError)
	{
		QSettings settings("settings.ini", QSettings::IniFormat);
		bool under = settings.value("Save/remplaceblanksbyunderscores", false).toBool();
		QStringList blacklistedtags(settings.value("blacklistedtags").toString().split(' '));
		QString source = reply->readAll();
		QRegExp rx(this->regex.at(5));
		rx.setMinimal(true);
		int pos = 0;
		QString tags = " "+m_details.value("tags")+" ";
		while ((pos = rx.indexIn(source, pos)) != -1)
		{
			pos += rx.matchedLength();
			QString type = rx.cap(1), tag = rx.cap(2).replace(" ", "_"), normalized = tag;
			normalized.remove('\\').remove('/').remove(':').remove('*').remove('?').remove('"').remove('<').remove('>').remove('|');
			if (!under)
			{ normalized.replace('_', ' '); }
			if (blacklistedtags.contains(tag, Qt::CaseInsensitive))
			{ tags.replace(" "+tag+" ", " <a href=\""+tag+"\" style=\"font-weight:bold;text-decoration:underline;color:#000000\">"+tag+"</a> ");	}
			if (type == "character")		{ this->details["characters"].append(normalized);	tags.replace(" "+tag+" ", " <a href=\""+tag+"\" style=\"text-decoration:none;color:#00aa00\">"+tag+"</a> ");	}
			else if (type == "copyright")	{ this->details["copyrights"].append(normalized);	tags.replace(" "+tag+" ", " <a href=\""+tag+"\" style=\"text-decoration:none;color:#aa00aa\">"+tag+"</a> ");	}
			else if (type == "artist")		{ this->details["artists"].append(normalized);		tags.replace(" "+tag+" ", " <a href=\""+tag+"\" style=\"text-decoration:none;color:#aa0000\">"+tag+"</a> ");	}
			else if (type == "model")		{ this->details["models"].append(normalized);		tags.replace(" "+tag+" ", " <a href=\""+tag+"\" style=\"text-decoration:none;color:#0000ee\">"+tag+"</a> ");	}
			else							{ this->details["generals"].append(normalized);		tags.replace(" "+tag+" ", " <a href=\""+tag+"\" style=\"text-decoration:none;color:#000000\">"+tag+"</a> "); }
			this->details["alls"].append(normalized);
		}
		ui->labelTags->setText(tags.trimmed());
		m_details["tags"] = tags.trimmed();
		m_detailsWindow->setTags(tags.trimmed());
		QString pth = this->getSavePath();
		QString path = settings.value("Save/path").toString().replace("\\", "/");
		if (path.right(1) == "/")
		{ path = path.left(path.length()-1); }
		QFile file(path+"/"+pth);
		if (file.exists() && !path.isEmpty() && !pth.isEmpty())
		{
			ui->buttonSave->setText(tr("Fichier déjà existant"));
			ui->buttonSaveNQuit->setText(tr("Fermer"));
			this->d.clear();
				if (!file.open(QIODevice::ReadOnly))
				{ error(this, tr("Erreur inattendue lors de l'ouverture du fichier.\r\n%1").arg(path+"/"+pth)); }
				while (!file.atEnd())
				{ this->d.append(file.readLine()); }
			this->image.loadFromData(this->d, m_format);
			this->loaded = true;
			this->update();
		}
		else
		{
			ui->buttonSave->setText(tr("Enregistrer"));
			ui->buttonSaveNQuit->setText(tr("Enregistrer et fermer"));
			this->load();
		}
	}
	else if (reply->error() != QNetworkReply::OperationCanceledError)
	{ error(this, tr("Une erreur inattendue est survenue lors du chargement des tags.\r\n%1").arg(reply->url().toString())); }
}

void zoomWindow::replyFinishedZoom(QNetworkReply* reply)
{
	if (reply->error() == QNetworkReply::NoError)
	{
		this->d.append(reply->readAll());
		this->image.loadFromData(this->d, m_format);
		this->loaded = true;
		this->update();
		if (this->m_mustSave)
		{ this->saveImage(); }
	}
	else if (reply->error() != QNetworkReply::OperationCanceledError)
	{ error(this, tr("Une erreur inattendue est survenue lors du chargement de l'image.\r\n%1").arg(reply->url().toString())); }
}



void zoomWindow::update(bool onlysize)
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

void zoomWindow::saveNQuit()
{
	if (!this->saveImage().isEmpty())
	{ this->close(); }
}

QString zoomWindow::saveImage()
{
	if (!loaded)
	{
		ui->buttonSave->setText(tr("Sauvegarde..."));
		m_mustSave = true;
		return QString();
	}
	QSettings settings("settings.ini", QSettings::IniFormat);
	QString pth = settings.value("Save/path").toString().replace("\\", "/");
	if (pth.right(1) == "/")
	{ pth = pth.left(pth.length()-1); }
	QString path = this->getSavePath();
	if (pth.isEmpty())
	{
		int reply = QMessageBox::question(this, tr("Erreur"), tr("Vous n'avez pas précisé de dossier de sauvegarde ! Voulez-vous ouvrir les options ?"), QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::Yes)
		{
			optionsWindow *options = new optionsWindow(m_parent);
			options->onglets->setCurrentIndex(2);
			options->setWindowModality(Qt::ApplicationModal);
			options->show();
			connect(options, SIGNAL(destroyed()), this, SLOT(saveImage()));
		}
		return QString();
	}
	if (settings.value("Save/filename").toString().isEmpty())
	{
		int reply = QMessageBox::question(this, tr("Erreur"), tr("Vous n'avez pas précisé de format de sauvegarde ! Voulez-vous ouvrir les options ?"), QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::Yes)
		{
			optionsWindow *options = new optionsWindow(m_parent);
			options->onglets->setCurrentIndex(2);
			options->setWindowModality(Qt::ApplicationModal);
			options->show();
			connect(options, SIGNAL(closed()), this, SLOT(saveImage()));
		}
		return QString();
	}
	QFile f(pth+"/"+path);
	if (!f.exists())
	{
		QDir path_to_file(pth+"/"+path.section('/', 0, -2));
		if (!path_to_file.exists())
		{
			QDir dir(pth);
			if (!dir.mkpath(path.section('/', 0, -2)))
			{ error(this, tr("Erreur lors de la sauvegarde de l'image.\r\n%1").arg(pth+"/"+path)); }
		}
		f.open(QIODevice::WriteOnly);
		f.write(this->d);
		f.close();
		m_parent->log(tr("Saved %1").arg(pth+"/"+path));
		ui->buttonSave->setText(tr("Sauvegardé !"));
		ui->buttonSaveNQuit->setText(tr("Fermer"));
	}
	else
	{ ui->buttonSave->setText(tr("Fichier déjà existant")); }
	return pth+"/"+path;
}

QString zoomWindow::saveImageAs()
{
	QString path = QFileDialog::getSaveFileName(this, tr("Enregistrer l'image"), m_details.value("file_url").section('/', -1), "Images (*.png *.gif *.jpg *.jpeg)");
	QFile f(path);
	f.open(QIODevice::WriteOnly);
	f.write(this->d);
	f.close();
	return path;
}

QString zoomWindow::getSavePath()
{
	QSettings settings("settings.ini", QSettings::IniFormat);
	settings.beginGroup("Save");
	QStringList copyrights;
	QString cop;
	bool found;
	for (int i = 0; i < this->details["copyrights"].size(); i++)
	{
		found = false;
		cop = this->details["copyrights"].at(i);
		for (int r = 0; r < copyrights.size(); r++)
		{
			if (copyrights.at(r).left(cop.size()) == cop.left(copyrights.at(r).size()))
			{
				if (cop.size() < copyrights.at(r).size())
				{ copyrights[r] = cop; }
				found = true;
			}
		}
		if (!found)
		{ copyrights.append(cop); }
	}
	settings.value("copyright_useshorter", true).toBool();
	QString filename = settings.value("filename").toString()
	.replace("%artist%", (this->details["artists"].isEmpty() ? settings.value("artist_empty").toString() : (settings.value("artist_useall").toBool() || this->details["artists"].count() == 1 ? this->details["artists"].join(settings.value("artist_sep").toString()) : settings.value("artist_value").toString())))
	.replace("%general%", this->details["generals"].join(settings.value("separator").toString()))
	.replace("%copyright%", (copyrights.isEmpty() ? settings.value("copyright_empty").toString() : (settings.value("copyright_useall").toBool() || copyrights.count() == 1 ? copyrights.join(settings.value("copyright_sep").toString()) : settings.value("copyright_value").toString())))
	.replace("%character%", (this->details["characters"].isEmpty() ? settings.value("character_empty").toString() : (settings.value("character_useall").toBool() || this->details["characters"].count() == 1 ? this->details["characters"].join(settings.value("character_sep").toString()) : settings.value("character_value").toString())))
	.replace("%model%", (this->details["models"].isEmpty() ? settings.value("model_empty").toString() : (settings.value("model_useall").toBool() || this->details["models"].count() == 1 ? this->details["models"].join(settings.value("model_sep").toString()) : settings.value("model_value").toString())))
	.replace("%model|artist%", (!this->details["models"].isEmpty() ? (settings.value("model_useall").toBool() || this->details["models"].count() == 1 ? this->details["models"].join(settings.value("model_sep").toString()) : settings.value("model_value").toString()) : (this->details["artists"].isEmpty() ? settings.value("artist_empty").toString() : (settings.value("artist_useall").toBool() || this->details["artists"].count() == 1 ? this->details["artists"].join(settings.value("artist_sep").toString()) : settings.value("artist_value").toString()))))
	.replace("%all%", this->details["alls"].join(settings.value("separator").toString()))
	.replace("%filename%", m_details.value("file_url").section('/', -1).section('.', 0, -2))
	.replace("%rating%", this->rating)
	.replace("%md5%", m_details.value("md5"))
	.replace("%website%", m_details.value("site"))
	.replace("%ext%", this->url.section('.', -1))
	.replace("\\", "/");
	if (filename.left(1) == "/")
	{ filename = filename.right(filename.length()-1); }
	return filename;
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
	QSettings settings("settings.ini", QSettings::IniFormat);
		settings.beginGroup("Zoom");
		settings.setValue("state", int(this->windowState()));
		settings.setValue("size", this->size());
		settings.setValue("pos", this->pos());
	if (r->isRunning())			{ r->abort();		}
	if (m_replyExists)			{ m_reply->abort();	}
	QWidget::closeEvent(e);
}
