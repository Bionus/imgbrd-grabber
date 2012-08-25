#include "searchtab.h"

searchTab::searchTab(int id, QWidget *parent) : QWidget(parent), m_id(id)
{ }
searchTab::~searchTab()
{ emit deleted(m_id); }

int searchTab::id()
{ return m_id; }
