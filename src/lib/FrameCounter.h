/**
 * @file FrameCounter.h
 */
#ifndef FRAMECOUNTER_H_
#define FRAMECOUNTER_H_

#include <queue>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace acerc11xdrv {

	class FrameCounter {
		public:
			typedef boost::posix_time::ptime Time;
			typedef boost::posix_time::time_duration TimeDuration;

			FrameCounter(unsigned int number_of_samples = 25);
			void update();
			TimeDuration getDurationPerFrame() const;

		private:
			unsigned int max_number_of_samples;
			Time last_call_time;
			TimeDuration sum_duration;
			std::queue<TimeDuration> samples;
	};
}

#endif /* FRAMECOUNTER_H_ */
