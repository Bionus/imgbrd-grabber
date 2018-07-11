#include "filter.h"


Filter::Filter(bool invert)
	: m_invert(invert)
{}

bool Filter::operator==(const Filter &rhs)
{
	return m_invert == rhs.m_invert && compare(rhs);
}
