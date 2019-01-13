#include "exponential-moving-average.h"


ExponentialMovingAverage::ExponentialMovingAverage(double smoothingFactor)
	: m_smoothingFactor(smoothingFactor)
{}


double ExponentialMovingAverage::average() const
{
	return m_average;
}

void ExponentialMovingAverage::setSmoothingFactor(double smoothingFactor)
{
	m_smoothingFactor = smoothingFactor;
}

void ExponentialMovingAverage::addValue(double value)
{
	if (!m_hasValue) {
		m_average = value;
		m_hasValue = true;
		return;
	}

	m_average = m_smoothingFactor * value + (1 - m_smoothingFactor) * m_average;
}

void ExponentialMovingAverage::clear()
{
	m_average = 0;
	m_hasValue = false;
}
