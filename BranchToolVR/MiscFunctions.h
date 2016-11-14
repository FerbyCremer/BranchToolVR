#pragma once

#include <stdio.h> 
#include <stdarg.h> 
#include <string>
#include <cstdlib>
#include <chrono>
#include <thread>

namespace MiscFunctions {

	template <typename T>
	std::string to_string_with_precision(const T a_value, const int n)
	{
		std::ostringstream out;
		out << std::setprecision(n) << a_value;
		return out.str();
	}

	static inline void ray_triangle_intersection(const float *v1, const float *v2, const float *v3, const float *p, const float *l, float *cg) {
		cg[0] = (v2[1] * l[2] * p[0] + l[1] * v2[0] * p[2] - v2[2] *
		l[1] * p[0] - l[2] * v2[0] * p[1] - l[0] * v2[1] * p[2] + l[0] *
		v2[2] * p[1] - v2[1] * l[2] * v3[0] - l[1] * v2[0] * v3[2] +
		v2[2] * l[1] * v3[0] + l[2] * v2[0] * v3[1] + l[0] * v2[1] * v3[2]
		- l[0] * v2[2] * v3[1] + l[1] * p[0] * v3[2] - v3[1] * l[2] * p[0]
		- l[1] * v3[0] * p[2] + p[1] * l[2] * v3[0] + l[0] * v3[1] * p[2]
		- l[0] * p[1] * v3[2]) / (v2[1] * l[2] * v1[0] - v2[1] * l[2] *
		v3[0] + l[1] * v2[0] * v1[2] + v1[1] * l[2] * v3[0] - l[2] * v2[0]
		* v1[1] + l[2] * v2[0] * v3[1] - l[2] * v1[0] * v3[1] - l[0] *
		v1[1] * v3[2] + l[0] * v1[2] * v3[1] - l[0] * v2[1] * v1[2] + l[0]
		* v2[1] * v3[2] - l[0] * v2[2] * v3[1] - l[1] * v2[0] * v3[2] +
		l[1] * v1[0] * v3[2] - v1[2] * l[1] * v3[0] + v2[2] * l[1] * v3[0]
		- v2[2] * l[1] * v1[0] + l[0] * v2[2] * v1[1]); cg[1] = -(l[0] *
		v1[1] * v3[2] - l[0] * v1[1] * p[2] - l[0] * v1[2] * v3[1] + l[0]
		* v3[1] * p[2] + l[0] * v1[2] * p[1] - l[0] * p[1] * v3[2] - v1[2]
		* l[1] * p[0] - l[1] * v3[0] * p[2] - v3[1] * l[2] * p[0] + v1[1]
		* l[2] * p[0] + l[1] * v1[0] * p[2] - v1[1] * l[2] * v3[0] + l[1]
		* p[0] * v3[2] - l[1] * v1[0] * v3[2] + l[2] * v1[0] * v3[1] - l[2
		] * v1[0] * p[1] + p[1] * l[2] * v3[0] + v1[2] * l[1] * v3[0]) /
		(v2[1] * l[2] * v1[0] - v2[1] * l[2] * v3[0] + l[1] * v2[0] *
		v1[2] + v1[1] * l[2] * v3[0] - l[2] * v2[0] * v1[1] + l[2] *
		v2[0] * v3[1] - l[2] * v1[0] * v3[1] - l[0] * v1[1] * v3[2] + l[0]
		* v1[2] * v3[1] - l[0] * v2[1] * v1[2] + l[0] * v2[1] * v3[2] -
		l[0] * v2[2] * v3[1] - l[1] * v2[0] * v3[2] + l[1] * v1[0] * v3[2]
		- v1[2] * l[1] * v3[0] + v2[2] * l[1] * v3[0] - v2[2] * l[1] *
		v1[0] + l[0] * v2[2] * v1[1]); cg[2] = (-l[2] * v2[0] * v1[1] +
		l[2] * v2[0] * p[1] - l[2] * v1[0] * p[1] + l[0] * v2[1] * p[2] +
		v1[1] * l[2] * p[0] + v2[1] * l[2] * v1[0] - v2[1] * l[2] * p[0] +
		l[1] * v2[0] * v1[2] - l[1] * v2[0] * p[2] + l[1] * v1[0] * p[2]
		- v1[2] * l[1] * p[0] + v2[2] * l[1] * p[0] - v2[2] * l[1] * v1[0]
		- l[0] * v2[1] * v1[2] - l[0] * v1[1] * p[2] + l[0] * v1[2] *
		p[1] + l[0] * v2[2] * v1[1] - l[0] * v2[2] * p[1]) / (v2[1] * l[2]
		* v1[0] - v2[1] * l[2] * v3[0] + l[1] * v2[0] * v1[2] + v1[1] *
		l[2] * v3[0] - l[2] * v2[0] * v1[1] + l[2] * v2[0] * v3[1] - l[2]
		* v1[0] * v3[1] - l[0] * v1[1] * v3[2] + l[0] * v1[2] * v3[1] -
		l[0] * v2[1] * v1[2] + l[0] * v2[1] * v3[2] - l[0] * v2[2] * v3[1]
		- l[1] * v2[0] * v3[2] + l[1] * v1[0] * v3[2] - v1[2] * l[1] *
		v3[0] + v2[2] * l[1] * v3[0] - v2[2] * l[1] * v1[0] + l[0] * v2[2]
		* v1[1]);
	}

	static inline void thread_sleep(unsigned long nMilliseconds)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(nMilliseconds));
	}

	static inline void dprintf(const char *fmt, ...)
	{
		va_list args;
		char buffer[2048];

		va_start(args, fmt);
		vsprintf_s(buffer, fmt, args);
		va_end(args);

		printf("%s", buffer);
	}

};