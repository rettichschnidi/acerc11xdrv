/**
 * @file FrameCounter.cpp
 */
#include "FrameCounter.h"

#include <iostream>
#include <boost/date_time/posix_time/time_period.hpp>

namespace acerc11xdrv {

	FrameCounter::FrameCounter(unsigned int number_of_samples) :
		max_number_of_samples(number_of_samples), sum_duration(boost::posix_time::microseconds(1)) {
		last_call_time = boost::posix_time::microsec_clock::local_time();
}

	void FrameCounter::update() {
		Time current_call_time(boost::posix_time::microsec_clock::local_time());
		TimeDuration duration =  current_call_time - last_call_time;
		samples.push(duration);
		sum_duration += duration;
		if(samples.size() > max_number_of_samples) {
			sum_duration -= samples.front();
			samples.pop();
		}
		last_call_time = current_call_time;
	}

	FrameCounter::TimeDuration FrameCounter::getDurationPerFrame() const {
		return sum_duration/samples.size();
	}

}
