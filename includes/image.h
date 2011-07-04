#ifndef IMAGE_H
#define IMAGE_H

#include <QtGui>



class Image
{
	public:
		Image(QMap<QString,QString> details, int timezonedecay = 0);
		QString		url();
		QString		md5();
		QString		author();
		QString		rating();
		QString		source();
		QStringList	tags();
		int			id();
		int			score();
		int			parent_id();
		int			file_size();
		QDateTime	created_at();
		bool		has_children();
		bool		has_notes();
		bool		has_comments();
		QUrl		page_url();
		QUrl		file_url();
		QUrl		sample_url();
		QUrl		preview_url();
		QSize		size();

	private:
		QString		m_url, m_md5, m_author, m_rating, m_source;
		QStringList	m_tags;
		int			m_id, m_score, m_parent_id, m_file_size;
		QDateTime	m_created_at;
		bool		m_has_children, m_has_notes, m_has_comments;
		QUrl		m_page_url, m_file_url, m_sample_url, m_preview_url;
		QSize		m_size;
};

#endif // IMAGE_H
