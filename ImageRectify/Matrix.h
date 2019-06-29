// Matrix.h: interface for the CMatrix class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATRIX_H__1398F5F4_36E4_11D6_996A_00D0B72CA4A9__INCLUDED_)
#define AFX_MATRIX_H__1398F5F4_36E4_11D6_996A_00D0B72CA4A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "math.h"
//#include "..\\a_Initial\\GeoArithmetic.h"
#define BOOL bool
#define FALSE false
#define TRUE true

//#define max(a,b) ((a) > (b) ? (a) : (b))

#pragma warning(disable : 4018)
#pragma warning(disable : 4305)

struct matrixN
{
	double BTA[3][6],ATB[3][6];
	double NIBTNBU[6];
};

class CMatrix
{
public:
	CMatrix();
	virtual ~CMatrix() ;
	//virtual ~CMatrix() = 0;//2015-12-26,xxw,
//	BOOL FirstTime;
	BOOL ExportCoviaranceMatrix; 

	void MatrixPrint(double *a, int m, int n);//2016-01-04,xxw

	//常用矩阵运算函数
	BOOL MatrixInversion_General(double *a, int n);
	int  MatrixInversion(double *a, int n);
	void MatrixInvResult(double *N, double *L, double *X, int n);
	BOOL MatrixTranspose(double * A ,double * AT, int M , int N );
	BOOL MatrixMulti(double *a, double *b, double *c, int m, int n, int k);
	void MultiplyMatrixWeight(double *A, double *P, double* AP,int m, int n);
	BOOL MatrixABAT (double * A, double * B , double * C, int M, int N);
	BOOL PartitionMatrixInversion(double *N,double *L,double *X, int photoNumber,int Cmr_ParaNum,int ValidSpcPtNum, int Spc_ParaNum, int IntrinsicParaNum);

	//法方程系数矩阵由 外方位元素 和 空间坐标未知数 组成时的分块求逆法
	void FourPartMatrixInversion(double *NN, double *L, double *X, int photoNumber,int Cmr_ParaNum,int ValidSpcPtNum, int Spc_ParaNum);
	
	//正方形矩阵分为2*2子矩阵求逆通用程序
	void Sub2by2Matrix_Inversion(double *N,int first,int second);


	//病态矩阵求解
	void Ill_Condition_Matrix_Sovle(double *N, double *L, double *X,double *Qxx, int n,int times, double espAngle,double espPoint);
	//矩阵条件数计算函数族
	double Matrix_Condition(double *matrix,int m, int n);

	
	//AX = 0 的解为A的最小奇异值对应的列向量，由于V返回值为列向量的转置，故而采用行向量即可
	BOOL  SingularValueDecomposition(double *a, int m,int  n, double *u,double * v, double eps, int ka);

	void a_ppp(double *a, double *e, double *s, double *v,int m, int n);
	void a_sss(double fg[2], double cs[2]);

	void Zero(double *p, int n);
	void  Nrml (double*aa,int n,double bb,double*a,double*b, double p=1);
	void Solve(double*a,double*b,double*x,int n,int wide);

//////////////////////////////////////////////////////
//
//矩阵填充
//
//a矩阵为m*n阶，b为p*q阶（小块矩阵）,
//将b放在a的第(r,c)个元素处(r=0,1……m-1，c=0,1,……n-1)
//
//////////////////////////////////////////////////////
	void FillMatrix(double *a,double *b,int m,int n,int p,int q,int r,int c);

//////////////////////////////////////////////////////
//
//矩阵填充 ------  相减
//
//a矩阵为m*n阶，b为p*q阶（小块矩阵）,
//将b放在a的第(r,c)个元素处(r=0,1……m-1，c=0,1,……n-1)
//
//////////////////////////////////////////////////////
	void FillMatrix_MINUS(double *a,double *b,int m,int n,int p,int q,int r,int c);

/////////////////////////////////////////////////////////////
//功  能：矩阵相加
//输  入：矩阵a（mxn阶)加上矩阵b（mxn阶）放在矩阵a（mxn阶）
//输  出：void，结果在矩阵a中
/////////////////////////////////////////////////////////////
	void MatrixAdd(double *a, double *b,int m, int n);

private:
	void Ldltban1(double*a,double*d,double*l,int n,int wide);
	void Ldltban2 (double*l,double*d,double*b,double*x,int n,int wide);

};

#endif // !defined(AFX_MATRIX_H__1398F5F4_36E4_11D6_996A_00D0B72CA4A9__INCLUDED_)
