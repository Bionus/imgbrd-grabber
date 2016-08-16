#include "pool.h"



Pool::Pool(int id, QString name, int current, int next, int previous) : m_name(name), m_id(id), m_current(current), m_next(next), m_previous(previous)
{ }
Pool::~Pool()
{ }

QString	Pool::name()		{ return m_name;		}
int		Pool::id()			{ return m_id;			}
int		Pool::current()		{ return m_current;		}
int		Pool::next()		{ return m_next;		}
int		Pool::previous()	{ return m_previous;	}
