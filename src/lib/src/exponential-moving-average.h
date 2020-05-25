#ifndef EXPONENTIAL_MOVING_AVERAGE_H
#define EXPONENTIAL_MOVING_AVERAGE_H


class ExponentialMovingAverage
{
	public:
		ExponentialMovingAverage() = default;
		explicit ExponentialMovingAverage(double smoothingFactor);
		double average() const;
		void setSmoothingFactor(double smoothingFactor);
		void addValue(double value);
		void clear();

	private:
		double m_smoothingFactor;
		double m_average = 0;
		bool m_hasValue = false;
};

#endif // EXPONENTIAL_MOVING_AVERAGE_H
