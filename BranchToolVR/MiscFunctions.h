#pragma once

#include <stdio.h> 
#include <stdarg.h> 
#include <string>
#include <cstdlib>
#include <chrono>
#include <iostream>
#include <thread>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtc/matrix_access.hpp>

#include "Constants.h"

namespace MiscFunctions {

	template <typename T>
	std::string to_string_with_precision(const T a_value, const int n)
	{
		std::ostringstream out;
		out << std::setprecision(n) << a_value;
		return out.str();
	}

	static inline void ray_triangle_intersection(const float *v1, const float *v2, const float *v3, const float *p, const float *l, float *cg) 
	{
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


	static inline float XZplaneAngleY(glm::vec3 a)
	{
		glm::vec3 xz = glm::normalize(glm::vec3(a.x, 0.0f, a.z));

		float xzDotXaxis = acos(glm::dot(xz, Constants::X_AXIS));

		if (xz.x > 0 && xz.z < 0) 
		{
			// quad 0
			//std::cout << "0" << std::endl;
			return xzDotXaxis;
		}
		else if (xz.x < 0 && xz.z < 0) 
		{
			// quad 1
			//std::cout << "1" << std::endl;
			return xzDotXaxis;
		}
		else if (xz.x < 0 && xz.z > 0) 
		{
			// quad 2
			//std::cout << "2" << std::endl;
			return Constants::TWOPI - xzDotXaxis;
		}
		else if (xz.x > 0 && xz.z > 0) 
		{
			// quad 3
			//std::cout << "3" << std::endl;
			//std::cout << Constants::TWOPI - xzDotXaxis << std::endl;
			return Constants::TWOPI - xzDotXaxis;
		}
		else
		{
			// inbetween quadrants
			if (xz.x == 0.0f && xz.z < 0)
			{
				return xzDotXaxis;
			}
			else if (xz.x == 0.0f && xz.z > 0)
			{
				return Constants::TWOPI - xzDotXaxis;
			}
			else if (xz.z == 0.0f && xz.x < 0.0f)
			{
				return Constants::PI;
			}
			else if (xz.z == 0.0f && xz.x > 0.0f)
			{
				return 0.0f;
			}
		}
	}

	static inline float XZplaneAngleZ(glm::vec3 a, glm::vec3 b)
	{
		glm::vec3 z = glm::normalize(a);
		glm::vec3 zflat = glm::normalize(glm::vec3(a.x, 0.0f, a.z));

		float zDotXaxis = acos(glm::dot(z, zflat));

		float fac = 1.0f;

		if (b.x < 0.0f) {
			fac = -1.0f;
		}

		if (z.y > 0.0f)
		{
			return fac*zDotXaxis;
		}
		else
		{
			return fac*-zDotXaxis;
		}

		if (z.x < 0)
		{
			if (z.y > 0.0f)
			{
				return -zDotXaxis;
			}
			else
			{
				return zDotXaxis;
			}
		}
		else
		{
			if (z.y > 0.0f)
			{
				return zDotXaxis;
			}
			else
			{
				return -zDotXaxis;
			}
		}


	}


	static inline glm::mat4 RotateTo(glm::vec3 a, glm::vec3 b) 
	{
		if (a == Constants::ZERO_VEC3 || b == Constants::ZERO_VEC3) 
		{
			return glm::mat4(1.0f);
		}

		glm::vec3 aUnit = glm::normalize(a);
		glm::vec3 bUnit = glm::normalize(b);
		glm::vec3 cross = glm::cross(aUnit, bUnit);
		float dot = glm::dot(aUnit, bUnit);
		float crossMag = glm::length(cross);


		//return glm::rotate(glm::mat4(1.0f), -(XZplaneAngle(b) - XZplaneAngle(a)), Constants::Y_AXIS);

		//std::cout << XZplaneAngleY(b) << std::endl;

		//std::cout << (XZplaneAngleZ(b) - XZplaneAngleZ(a)) << std::endl;

		

		return glm::rotate(glm::mat4(1.0f), (XZplaneAngleY(b) - XZplaneAngleY(a)), Constants::Y_AXIS) *glm::rotate(glm::mat4(1.0f), (XZplaneAngleZ(b, a) - XZplaneAngleZ(a, a)), Constants::Z_AXIS) *glm::scale(glm::mat4(1.0f), glm::vec3(glm::length(b) / glm::length(a)));

		//return glm::rotate(glm::mat4(1.0f), (XZplaneAngleZ(b)), Constants::Z_AXIS)*glm::rotate(glm::mat4(1.0f), (XZplaneAngleY(b) - XZplaneAngleY(a)), Constants::Y_AXIS);// *glm::scale(glm::mat4(1.0f), glm::vec3(glm::length(b) / glm::length(a)));

	}

};		


		//float phi = acos()


		//glm::mat4 m(dot, -crossMag,   0, 0.0f,
		//	   crossMag,      0.0f, dot, 0.0f,
		//			  0,         0,   0, 0.0f,
		//			0.0f,     0.0f,   0, 0.0f);


		//glm::mat3 g( dot, -crossMag,   0,
		//		crossMag,      0.0f, dot,
		//		       0,         0,   0);
		//
		//glm::vec3 u = glm::proj(bUnit, aUnit);
		//glm::vec3 v = bUnit - u;
		//
		//glm::mat3 f;
		//glm::column(f, 0, aUnit);
		//glm::column(f, 1, v);
		//glm::column(f, 2, cross);
		//glm::mat3 fi = glm::inverse(f);
		//
		//glm::mat3 ret = fi*g*f;
		//
		//glm::mat4 m(ret);
		//
		//return m;

		//float c = glm::dot(aUnit, bUnit);
		//float s = glm::length(v);
		//float factor = 1.0f / (1.0f + c);
		//
		//glm::mat4 m(0.0f, -v.z,  v.y, 0.0f,
		//			 v.z, 0.0f, -v.x, 0.0f,
		//			-v.y, -v.x,  0.0f, 0.0f,
		//			0.0f,  0.0f, 0.0f, 0.0f);
		//
		//std::cout << m[0][0] << " " << m[0][1] << " " << m[0][2] << " " << m[0][3] << std::endl;
		//std::cout << m[1][0] << " " << m[1][1] << " " << m[1][2] << " " << m[1][3] << std::endl;
		//std::cout << m[2][0] << " " << m[2][1] << " " << m[2][2] << " " << m[2][3] << std::endl;
		//std::cout << m[3][0] << " " << m[3][1] << " " << m[3][2] << " " << m[3][3] << std::endl;
		//std::cout << std::endl;

		//return (glm::mat4(1.0f) + m + (m*m)*factor);