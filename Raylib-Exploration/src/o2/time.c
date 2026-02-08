#include <o2/time.h>
#include <assert.h>

bool o2_time_limiterReady(o2_Limiter* limiter, float64 currentTime) {
	assert(NULL != limiter);

	return currentTime - limiter->last > limiter->threshold;
}

bool o2_time_limiterResetIfReady(o2_Limiter* limiter, float64 currentTime) {
	if (o2_time_limiterReady(limiter, currentTime)) {
		limiter->last = currentTime;
		return true;
	}

	return false;
}
