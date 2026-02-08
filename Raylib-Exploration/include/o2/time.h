#pragma once
#include <stdbool.h>
#include <o2/types.h>

typedef struct o2_Limiter {
	float64 threshold;
	float64 last;
} o2_Limiter;

bool o2_time_limiterReady(o2_Limiter* limiter, float64 currentTime);
bool o2_time_limiterResetIfReady(o2_Limiter* limiter, float64 currentTime);