#pragma once
#include "CoorConv.hpp"
#include <vector>
#include "Matrix.h"
using namespace std;

#define InvalidValue          -99999


template <class DataType>
class Point2_
{
public:
	typedef Point2_<DataType> MiType;
	Point2_() { x = InvalidValue; y = InvalidValue; }
	//		: Point2_(DataType(InvalidValue), DataType(InvalidValue)){}
	Point2_(const DataType _x, const DataType _y)
		: x(_x), y(_y) {}
	Point2_(const MiType &p)
		: x(p.x), y(p.y) {}
	~Point2_() {}

private:

public:
	DataType x;
	DataType y;

public:
	// define MiType operators

	MiType operator+(MiType pt) {
		return Point2_(x + pt.x, y + pt.y);
	}
	MiType operator-(MiType pt) {
		return Point2_(x - pt.x, y - pt.y);
	}
	MiType operator-() {
		return Point2_(-x, -y);
	}
	MiType operator*(MiType pt) {
		return Point2_(x *= pt.x, y *= pt.y);
	}
	MiType operator=(MiType pt) {
		return Point2_(x = pt.x, y = pt.y);
	}
	bool operator!=(MiType pt) {
		return (x != pt.x || y != pt.y);
	}
	bool operator==(MiType pt) {
		return (x == pt.x && y == pt.y);
	}
	MiType operator+=(MiType pt) {
		return (*this = *this + pt);
	}
	MiType operator-=(MiType pt) {
		return (*this = *this - pt);
	}
	MiType operator*(DataType k) {
		return Point2_(x *= k, y *= k);
	}
	MiType operator/(DataType k) {
		if (k != 0) return Point2_(x /= k, y /= k);
		else return Point2_(x, y);
	}
};

template <class DataType>
class Point3_
{
public:
	typedef Point3_<DataType> MiType;

	Point3_() { X = InvalidValue; Y = InvalidValue; Z = InvalidValue; }
	//		: Point3_(DataType(InvalidValue), DataType(InvalidValue), DataType(InvalidValue)){}
	Point3_(const DataType _x, const DataType _y, const DataType _z)
		: X(_x), Y(_y), Z(_z) {}
	~Point3_() {}


private:

public:
	DataType X;
	DataType Y;
	DataType Z;

public:

	MiType operator+(MiType gt) {
		return Point3_(X + gt.X, Y + gt.Y, Z + gt.Z);
	}
	MiType operator-(MiType gt) {
		return Point3_(X - gt.X, Y - gt.Y, Z - gt.Z);
	}
	MiType operator-() {
		return Point3_(-X, -Y, -Z);
	}
	MiType operator*(MiType gt) {
		return Point3_(X *= gt.X, Y *= gt.Y, Z *= gt.Z);
	}
	MiType operator/(DataType a) {
		if (a != DataType(0))
			return Point3_(X /= a, Y /= a, Z /= a);
		else return Point3_(999999999999999, 999999999999999, 9999999999999999);
	}
	MiType operator=(MiType gt) {
		return Point3_(X = gt.X, Y = gt.Y, Z = gt.Z);
	}
	bool operator==(MiType gt) {
		return (X == gt.X && Y == gt.Y && Z = gt.Z);
	}
	MiType operator+=(MiType gt) {
		return (*this = *this + gt);
	}
	MiType operator-=(MiType gt) {
		return (*this = *this - gt);
	}
	MiType operator/=(DataType a) {
		if (a != DataType(0))
			return (*this = *this / a);
		else return Point3_(999999999999999, 999999999999999, 9999999999999999);
	}
	bool operator!=(MiType pt) {
		return (X != pt.X || Y != pt.Y || Z != pt.Z);
	}
};

template <class DataType>
class CRect_
{
public:  //构造初始化
	typedef CRect_<DataType> MiType;
	CRect_() {}
	//		: CRect_(DataType(InvalidValue), DataType(InvalidValue), DataType(InvalidValue), DataType(InvalidValue)){}
	CRect_(const DataType _l, const DataType _t, const DataType _r, const DataType _b)
		: left(_l), top(_t), right(_r), bottom(_b) {}
	CRect_(Point2_<DataType> p1, Point2_<DataType> p2)
		: left(p1.x), top(p1.y), right(p2.x), bottom(p2.y) {}
	CRect_(const MiType &p)
		: left(p.left), top(p.top), right(p.right), bottom(p.bottom) {}
public:
	DataType left;
	DataType right;
	DataType top;
	DataType bottom;

	Point2_<DataType> RightTop() { return Point2_<DataType>(right, top); };
	Point2_<DataType> LeftBottom() { return Point2_<DataType>(left, bottom); };
	Point2_<DataType> LeftTop() { return Point2_<DataType>(left, top); };
	Point2_<DataType> RightBottom() { return Point2_<DataType>(right, bottom); };
	Point2_<DataType> CenterPoint() { return Point2_<DataType>((right + left) / 2, (top + bottom) / 2); };

	DataType Width() { return fabs(right - left); };
	DataType Height() { return fabs(top - bottom); };
	DataType Area() { return Width()*Height(); };

	bool operator==(MiType drect) { //矩形赋值
		return (left == drect.left&& top == drect.top&& right == drect.right&& bottom == drect.bottom);
	}
	MiType operator=(MiType drect) { //矩形赋值
		return CRect_(left = drect.left, top = drect.top, right = drect.right, bottom = drect.bottom);
	}

	MiType operator+(MiType drect) {//矩形求并
		return CRect_(left = min(left, drect.left), top = max(top, drect.top),
			right = max(right, drect.right), bottom = min(bottom, drect.bottom));
	}

public:
	bool Intersect_Rect(MiType dr) {
		if (left > dr.right || right < dr.left ||
			top<dr.bottom || bottom>dr.top)
			return false;
		return true;
	}
	bool Intersect_Pt(Point2_<DataType> pt) {
		if ((pt.x - left)*(pt.x - right) < 0 && (pt.y - top)*(pt.y - bottom) < 0)
			return true;
		return false;
	}

private:

};

typedef Point2_<double> Point2D;
typedef Point3_<double> Point3D;
typedef CRect_<double> CRectD;

struct cameraInfo
{
	CString strLabel;
	double Xs, Ys, Zs;
	double phi, omg, kap;
	double R[9];
	double f;
	cameraInfo()
	{
		f = 14000;
		for (int i = 0; i < 9; i++)
			R[i] = InvalidValue;
	}
	cameraInfo operator+(cameraInfo dpos)
	{
		cameraInfo res;
		dpos.CalRotMatrixByPOK();
		if (this->R[0] == InvalidValue)
			this->CalRotMatrixByPOK();
		CMatrix mt;
		mt.MatrixMulti(this->R, dpos.R, res.R, 3, 3, 3);
		double trans[3] = { dpos.Xs,dpos.Ys,dpos.Zs };
		double tTamp[3];
		mt.MatrixMulti(this->R, trans, tTamp, 3, 3, 1);
		res.Xs = this->Xs + tTamp[0];
		res.Ys = this->Ys + tTamp[1];
		res.Zs = this->Zs + tTamp[2];
		return res;
	}
	void CalRotMatrixByPOK()
	{
		double phi = DegToRad(this->phi), omega = DegToRad(this->omg), kappa = DegToRad(this->kap);
		double cp = cos(phi), sp = sin(phi);
		double co = cos(omega), so = sin(omega);
		double ck = cos(kappa), sk = sin(kappa);
		this->R[0] = cp*ck - so*sp*sk; this->R[1] = -cp*sk - so*sp*ck; this->R[2] = -co*sp;
		this->R[3] = co*sk; this->R[4] = co*ck; this->R[5] = -so;
		this->R[6] = sp*ck + so*cp*sk; this->R[7] = -sp*sk + so*cp*ck; this->R[8] = co*cp;
	}
};
//void CalRotMatrixByPOK(void* pos2)
//{
//	cameraInfo*pos = (cameraInfo*)pos2;
//	double phi = DegToRad(pos.phi), omega = DegToRad(pos.omg), kappa = DegToRad(pos.kap);
//	double cp = cos(phi), sp = sin(phi);
//	double co = cos(omega), so = sin(omega);
//	double ck = cos(kappa), sk = sin(kappa);
//	pos.R[0] = cp*ck - so*sp*sk; pos.R[1] = -cp*sk - so*sp*ck; pos.R[2] = -co*sp;
//	pos.R[3] = co*sk; pos.R[4] = co*ck; pos.R[5] = -so;
//	pos.R[6] = sp*ck + so*cp*sk; pos.R[7] = -sp*sk + so*cp*ck; pos.R[8] = co*cp;
//}
Point2D XYZ2Img(int nCols, int nRows, Point3D &ptXYZ, cameraInfo pos)
{
	if (pos.R[0] == InvalidValue) pos.CalRotMatrixByPOK();
	double Xs, Ys, Zs, f;
	Xs = pos.Xs;
	Ys = pos.Ys;
	Zs = pos.Zs;
	f = pos.f;
	double _X = ptXYZ.Y;
	double _Y = ptXYZ.X;
	double _Z = ptXYZ.Z;
	double X_new = pos.R[0] * (_X - Xs) + pos.R[3] * (_Y - Ys) + pos.R[6] * (_Z - Zs);
	double Y_new = pos.R[1] * (_X - Xs) + pos.R[4] * (_Y - Ys) + pos.R[7] * (_Z - Zs);
	double Z_new = pos.R[2] * (_X - Xs) + pos.R[5] * (_Y - Ys) + pos.R[8] * (_Z - Zs);
	double x, y;
	x = -f*X_new / Z_new + nCols / 2;
	y = -f*Y_new / Z_new + nRows / 2;
	Point2D	ptImg = Point2D(x, y);
	return ptImg;
}
Point3D Img2XYZ(int nCols, int nRows, Point2D &ptImg, double Z, cameraInfo pos)
{
	if (pos.R[0] == InvalidValue) pos.CalRotMatrixByPOK();
	double Xs, Ys, Zs, f;
	Xs = pos.Xs;
	Ys = pos.Ys;
	Zs = pos.Zs;
	f = pos.f;
	double _px = ptImg.x - nCols / 2;
	double _py = ptImg.y - nRows / 2;
	double _Z = Z;
	double A1 = pos.R[2] * _px + pos.R[0] * f;
	double B1 = pos.R[5] * _px + pos.R[3] * f;
	double M1 = (pos.R[2] * _px + pos.R[0] * f)*Xs +
		(pos.R[5] * _px + pos.R[3] * f)*Ys -
		(pos.R[8] * _px + pos.R[6] * f)*(_Z - Zs);

	double A2 = pos.R[2] * _py + pos.R[1] * f;
	double B2 = pos.R[5] * _py + pos.R[4] * f;
	double M2 = (pos.R[2] * _py + pos.R[1] * f)*Xs +
		(pos.R[5] * _py + pos.R[4] * f)*Ys -
		(pos.R[8] * _py + pos.R[7] * f)*(_Z - Zs);
	double _X = (M1*B2 - M2*B1) / (A1*B2 - A2*B1);
	double _Y = (M1*A2 - M2*A1) / (A2*B1 - A1*B2);
	Point3D ptXYZ = Point3D(_Y, _X, Z);
	return ptXYZ;
}