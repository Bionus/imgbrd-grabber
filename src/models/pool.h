#ifndef POOL_H
#define POOL_H

#include <QString>



class Pool
{
	public:
		explicit Pool(int id, QString name, int current, int next = 0, int previous = 0);
		~Pool();
		QString name();
		int		id();
		int		current();
		int		next();
		int		previous();

	private:
		QString	m_name;
		int		m_id, m_current, m_next, m_previous;
};

#endif // POOL_H
