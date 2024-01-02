#pragma once

#include "deprecated.h"
#include <iostream>

template<typename T>
class Vec3
{
protected:
	T posx;
	T posy;
	T posz;
public:
	/** Default constructor. */
	Vec3(T x = 0, T y = 0, T z = 0) : posx(x), posy(y), posz(z) {}

	void operator+=(const Vec3<T> &p) {
		posx += p.x();
		posy += p.y();
		posz += p.z();
	}

	void operator-=(const Vec3<T> &p) {
		posx -= p.x();
		posy -= p.y();
		posz -= p.z();
	}

	Vec3<T> operator-(const Vec3<T> &p) const {
		Vec3<T> res;
		res.x(posx - p.x());
		res.y(posy - p.y());
		res.z(posz - p.z());
		return res;
	}

	Vec3<T> operator+(const Vec3<T> &p) const {
		Vec3<T> res;
		res.x(p.x() + posx);
		res.y(p.y() + posy);
		res.z(p.z() + posz);
		return res;
	}

	/**
	 * Multiply by scalar of any type (int, double, etc)
	 */
	template<typename U>
	Vec3<T> operator*(U scalar) const {
		Vec3<T> res;
		res.x(posx * scalar);
		res.y(posy * scalar);
		res.z(posz * scalar);
		return res;
	}

	bool operator==(const Vec3<T> &p) const {
		return (posx == p.x()) && posy == p.y() && posz == p.z();
	}

	bool operator!=(const Vec3<T> &p) const {
		return posx != p.x() || posy != p.y();
	}

	// needed for e.g. inserting in a std::set.
	bool operator<(const Vec3<T> &p) const {
		return posz == p.z()
				? (posy == p.y()
					   ? posx < p.x()
					   : posy < p.y())
			   : posz < p.z();
	}

	/* Get and set functions */
	T x() const { return posx; }
	T y() const { return posy; }
	T z() const { return posz; }
	void x(T v) { posx = v; }
	void y(T v) { posy = v; }
	void z(T v) { posz = v; }
};

template<typename T>
std::ostream &operator<<(std::ostream &os, const Vec3<T> &p) {
	os << "[" << p.x() << "," << p.y() << "," << p.z() << "]";
	return os;
}
