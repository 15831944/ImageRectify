// Matrix.cpp: implementation of the CMatrix class.
//
//////////////////////////////////////////////////////////////////////

//#include "..\visualinspector.h"
#include "Matrix.h"

#include <stdio.h>
#include <stdlib.h>

#define max(a,b) a>b?a:b
#define min(a,b) a<b?a:b

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMatrix::CMatrix()
{

}

CMatrix::~CMatrix()
{

}

void  CMatrix::Nrml (double*aa,int n,double bb,double*a,double*b, double p)
{
   register int  i,j;
				                          	
  for (i=0; i<n; i++) 
  {
      for (j=0; j<n-i; j++) 
	  {
		  *a += p * *aa * *(aa+j);
          a++; 
      }
      *b += p * *aa * bb;
      b++; aa++;
  }
}

void CMatrix::Zero(double *p, int n)
{
	while(n--) *p++=0;
}


	/*******************     SOLVE.CPP      *************************

	function   :    decomposes symmetric  A  used for solving
			symmetric linear equations with LDLT
			   A . X = B      A = LDLT
			and solves the vector  X

	input      :    a    matrix  A -- one dimensional arrangement
			     by row   ( symmetric lowertrangular )
			b    right-hand vector B
			n    the dimension of  A or B

	output     :    x         solve vectir  X

	call       :    ldltban1.c ldltban2.c

	********************************************************/

void CMatrix::Solve(double*a,double*b,double*x,int n,int wide)
{
	int      m;
	double   *d,*l;

	m= n*(n+1)/2;
	d=(double *) malloc(n * sizeof(double));
	l=(double *) malloc((m-n) * sizeof(double));

	Zero (d,n); 
	Zero (l,m-n);

	Ldltban1 (a,d,l,n,wide);

	Ldltban2 (l,d,b,x,n,wide);
	free(d); free(l);
}

/**********************   LDLTBAN1   ***********************

       function: decomposes band-symmetric matrix  A used for solving
        	      symmetric linear equation-system with LDL' method

	input      :  a      matrix  A  -- one dimensional arrangement
		      n      dimension of matrix  A
		      wide   band-wide of coefficient matrix  A

	output     :  l      unit lowertriangular matrix
		      d      diagnal matrix

			    Wu Xiaoliang    IDP/WTUSM    1989.2.14

	**********************************************************/
void CMatrix::Ldltban1(double*a,double*d,double*l,int n,int wide)
{
		int i,j,k,kk,km,m;
	double *ao,*aa,*co,*c;

	m = wide*(2*n+1-wide)/2;
	c =(double *)calloc ((m-wide),sizeof(double));

	ao=a; co=c; a +=wide;
	for (i=0; i<m-wide; i++) *c++ = *a++;
	c=co; a=ao;

	for (k=1; k<n; k++) {
	   if (k<n-wide+2) kk=wide-1;
	   else kk--;
	
	   *d = *a++; aa=a;  a += kk;

	   if (k<n-wide+1) km=wide;
	   else km=n-k+1;

	   for (i=1; i<kk+1; i++) {
              *l = *aa++ / *d;
	      for (j=0; j<kk-i+1; j++) *(a+j) -= *l * *(aa+j-1);
              l++;

              if (k+i>n-wide+1) km--;
              a += km;
           }

	   a=aa; d++;
           if (k==n-1)  *d = *a;
        }

	a=ao;  a +=wide;
	for (i=0; i<m-wide; i++) *a++ = *c++;
	c=co; free(c);
}

/**********************   LDLTBAND2.C    *******************

        function   :   backsubstition after decomposing  band-symmetric
        	       matrix  A  into  LDLT

	input      :   l     unit lowertriangular matrix  L
		       d     diagnal matrix  D
		       b     right-hand vector   B
		       n     dimension of  A, L, D or B
		       wide  band-wide of coefficient matrix  A

	output     :   x     solution vector  X

				Wu Xiaoliang  IDP/WTUSM    1989.2.14

	*****************************************************/	
void CMatrix::Ldltban2 (double*l,double*d,double*b,double*x,int n,int wide)
{
	int i,j,kk,m;
	double *bo, *lo, *xx;
	double *bb,*bbo;

	bb =(double*)calloc(n,sizeof(double));
	bbo=bb;

	bo=b; lo=l;

	for (i=0; i<n; i++)*bb++ = *b++;
	b=bo;  bb=bbo;
	m = wide*(2*n+1-wide)/2;

	for (i=1; i<n; i++){
	   if (i<n-wide+2) kk=wide;
	   else kk--;

	   b=bo+i;
	   for (j=1; j<kk; j++) *b++ -= *(b-j) * *l++;
	} 		
	
        kk=0;
	b=bo+n-1;  l=lo+m-n-1;
	x += n-1;  xx=x;  d += n-1;
	*x-- = *b-- / *d--;

	for (i=1; i<n; i++){
	   if (i<wide) kk++;
	   else {  kk=wide-1;  xx--; }

	   *x = *b-- / *d--;
	   for (j=1; j<kk+1; j++) *x -= *l-- * *(xx-j+1);
	   x--;
	}

	b=bo;
	for (i=0; i<n; i++) *b++ = *bb++;
	bb=bbo; free(bb);
}


void CMatrix::MatrixPrint(double *a, int m, int n)
{
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			printf("%.6lf ", a[i * n + j]);
		}
		printf("\n");
	}
}

/************************
一般实矩阵的求逆
**********************/
BOOL  CMatrix::MatrixInversion_General(double *a, int n)
//brinv
//int n;
//double a[];

{
    int        *is, *js, i, j, k, l, u, v;
    double     d, p;
    
    is = new int [n];	// = malloc(n *sizeof(int));
    js = new int [n];	// = malloc(n *sizeof(int));
    
    for(k = 0; k <= n - 1; k++)
    {
        d = 0.0;
        
        for(i = k; i <= n - 1; i++)
            for(j = k; j <= n - 1; j++)
            {
                l = i * n + j;
                p = fabs(a[l]);
                
                if(p > d)
                {
                    d = p;
                    is[k] = i;
                    js[k] = j;
                }
            }
        
        if(d + 1.0 == 1.0)
        {
            free(is);
            free(js);
            printf("err**not inv\n");
            //TRACE("err**not inv\n");
            return (0);
        }
        
        if(is[k] != k)
            for(j = 0; j <= n - 1; j++)
            {
                u = k * n + j;
                v = is[k] * n + j;
                p = a[u];
                a[u] = a[v];
                a[v] = p;
            }
        
        if(js[k] != k)
            for(i = 0; i <= n - 1; i++)
            {
                u = i * n + k;
                v = i * n + js[k];
                p = a[u];
                a[u] = a[v];
                a[v] = p;
            }
        l = k * n + k;
        a[l] = 1.0 / a[l];
        
        for(j = 0; j <= n - 1; j++)
            if(j != k)
            {
                u = k * n + j;
                a[u] = a[u] * a[l];
            }
        
        for(i = 0; i <= n - 1; i++)
            if(i != k)
                for(j = 0; j <= n - 1; j++)
                    if(j != k)
                    {
                        u = i * n + j;
                        a[u] = a[u] - a[i * n + k] * a[k * n + j];
                    }
        
        for(i = 0; i <= n - 1; i++)
            if(i != k)
            {
                u = i * n + k;
                a[u] = -a[u] * a[l];
            }
    }
    
    for(k = n - 1; k >= 0; k--)
    {
        if(js[k] != k)
            for(j = 0; j <= n - 1; j++)
            {
                u = k * n + j;
                v = js[k] * n + j;
                p = a[u];
                a[u] = a[v];
                a[v] = p;
            }
        
        if(is[k] != k)
            for(i = 0; i <= n - 1; i++)
            {
                u = i * n + k;
                v = i * n + is[k];
                p = a[u];
                a[u] = a[v];
                a[v] = p;
            }
    }
//    free(is);
//    free(js);
	delete is; delete js;
    return (1);
}


//对称正定矩阵求逆 a为n*n阶对称正定矩阵，n为矩阵阶数
int    CMatrix::MatrixInversion(double *a, int n)
{
    int i, j, k, m;
    double     w, g, *b;
    
    b = new double [n];
    
    for(k = 0; k <= n - 1; k++)
    {
        w = a[0];
        
        if(fabs(w)+1.0 == 1.0)
        {
            delete b;
            printf("fail\n");
            return(-2);
        }
        m = n - k - 1;
        
        for(i = 1; i <= n - 1; i++)
        {
            g = a[i * n];
            b[i] = g / w;
            
            if(i <= m)
                b[i] = -b[i];
            
            for(j = 1; j <= i; j++)
                a[(i - 1) * n + j - 1] = a[i * n + j] + g * b[j];
        }
        a[n * n - 1] = 1.0 / w;
        
        for(i = 1; i <= n - 1; i++)
            a[(n - 1) * n + i - 1] = b[i];
    }
    
    for(i = 0; i <= n - 2; i++)
        for(j = i + 1; j <= n - 1; j++)
            a[i * n + j] = a[j * n + i];
    delete b;
    return(2);
}

/////////////////////// //N为上三角矩阵时求逆并解算结果向量

// N 法方程系数矩阵(n*n)阶上三角排列，L 常数项矩阵(n*1)，X 解向量矩阵(n*1)，n 矩阵阶数
void CMatrix::MatrixInvResult(double *N, double *L, double *X, int n)
{	

	double *A  = new  double[n*n];	//求逆阵部分	//n为逆阵的阶数
	int i,j,k,m;				
	long double w,g,*b;
	b=new long double[n];
	for(i=0; i<n; i++)
	{
		for(j=0; j<n; j++)
		{
			if(i<=j)
				A[i*n+j] = N[i*(2*n-i-1)/2+j];
			else
				A[i*n+j] = N[j*(2*n-j-1)/2+i];
		}
		X[i]=0.0;
	}

//	MatrixInversion(A,n);

	for(k=0;k<=n-1;k++)
	{
		w=A[0]+1.0e-10;
		m=n-k-1;
		for(i=1;i<=n-1;i++)
		{
			g=A[i*n];b[i]=g/(w*1.0);
			if(i<=m) b[i]=-b[i];
			for(j=1;j<=i;j++)
				A[(i-1)*n+j-1]=A[i*n+j]+g*b[j];		
		}
		A[n*n-1]=1.0/(w*1.0);
		for(i=1;i<=n-1;i++)
			A[(n-1)*n+i-1]=b[i];
	}
	for(i=0;i<=n-2;i++)
		for(j=i+1;j<=n-1;j++)
			A[i*n+j]=A[j*n+i];
/////////////////////////////////////////////////////////////
	for(i=0; i<n; i++)
	{
		for(j=0; j<n; j++)
		{
			X[i] += A[i*n+j]*L[j];
			if(j<i)
				continue;
			else
				N[i*(2*n-i-1)/2+j] = A[i*n+j];
		}
	}
/////////////////////////////////////////////////////////////
	delete b;
	delete A;
}


// 求矩阵A(m*n)的转置矩阵AT(n*m)
BOOL CMatrix::MatrixTranspose (  double  * A ,  double  * AT , int M , int N )
{
	int i , j ; 
	if (A==NULL||AT==NULL)  return FALSE;
	for(i=0;i<N;i++) 
		for(j=0;j<M;j++)
			AT[i*M+j]=A[j*N+i] ;

	return  TRUE;
}

///////////////////////////////////////////////
//设A为m x n阶矩阵，B为n x l阶矩阵，C为m x l阶矩阵，计算 C=A x B的子程序为：
BOOL CMatrix::MatrixMulti(double *a, double *b, double *c, int m, int n, int k)
{
	int i,j,ii;
	for(i=0;i<m;i++)
	{
		for(j=0;j<k;j++)
		{
			*(c+i*k+j)=0;
			for(ii=0;ii<n;ii++)
				*(c+i*k+j)=*(c+i*k+j)+*(a+i*n+ii)**(b+ii*k+j);
		}
	}

	return true;
}

void CMatrix::MultiplyMatrixWeight(double *A, double *P, double* AP,int m, int n)
{
	for(int i=0;i<m;i++)
	{
		for(int j=0;j<n;j++)
		{
			AP[i*n+j] = A[i*n+j] * P[j];
		}
	}
}

//设A为m x n 阶矩阵，B为n x n阶矩阵，C为m x m阶矩阵，计算C = ABAT 的子程序快速方法为：
//A 为压缩存储的系数矩阵，B 为协因数矩阵，C为结果矩阵 (只计算主对角线元素)
BOOL CMatrix::MatrixABAT(double * A, double * B, double * C, int M, int N)
{
/////////////////////////////////////////////////////////////////////////
	int i = 0,k = 0;
/////////////////////////////////////////////////////////////////////////
	double *AB = new double [M*N];

	double *AT = new double [N*M];

	if ( A==NULL||B==NULL||C==NULL) return FALSE ;

	MatrixMulti(A, B, AB, M, N, N);
	
	MatrixTranspose(A, AT, M , N);
	
//	MatrixMulti(AB, AT, C, M, M, N);	//注意要计算全矩阵时需放开此行，注释掉下面7行

	for (i=0;i<M;i++)
	{
		C[i]=0.0 ;
		for (k=0;k<N;k++)
			C[i]=C[i]+AB[i*N+k]*AT[k*M+i];	//只计算主对角线元素
	}
/*
	int i , j , k , r ; 
	for (i=0;i<M;i++)
	{
		for(j=0;j<M;j++) 
		{
			C[i*M+j]=0.0;
			for (r=0;r<N;r++)
				for (k=0;k<N;k++)
					C[i*M+j]=C[i*M+j]+A[i*N+k]*B[k*N+r]*A[j*N+r] ;
		}
	}
*/
	delete AT;
	delete AB;
	return TRUE ;
}

//正方形矩阵分为2*2子矩阵求逆通用程序
//  2001/10/31       
void CMatrix::Sub2by2Matrix_Inversion(double *N, int first, int second)
{
	int i=0,j=0,k=0;
	int paraNumber = first + second;
	double *A1 = NULL,*A2 = NULL,*A3 = NULL,*A4 = NULL,*A5 = NULL;
	double *A6 = NULL,*A7 = NULL,*A8 = NULL,*A9 = NULL,*AA = NULL;
	A1 = new double [first*first];
	A2 = new double [first*second];
	A3 = new double [second*first];
	A4 = new double [second*second];

	A5 = new double [second*second];
	A6 = new double [first*first];
	A7 = new double [second*second];
	A8 = new double [first*second];
	A9 = new double [second*first];

	for(i=0; i<first; i++)		//A1
	{
		for(j=0; j<first; j++)
		{
			A1[i*first + j] = A6[i*first + j] = N[i*paraNumber + j];
		}
	}

	for(i=0; i<first; i++)		//A2
	{
		for(j=0; j<second; j++)
		{
			A2[i*second + j] = N[i*paraNumber + first + j];
		}
	}

	for(i=0; i<second; i++)		//A3
	{
		for(j=0; j<first; j++)
		{
			A3[i*first + j] = N[(i + first)*paraNumber + j];
		}
	}

	for(i=0; i<second; i++)		//A4
	{
		for(j=0; j<second; j++)
		{
			A4[i*second + j] = A7[i*second + j] = N[(i + first)*paraNumber + first + j];
		}
	}
	MatrixInversion(A1,first);
	MatrixInversion(A4,second);
/*///////
	int ssecond = second;
	do
	{
		ssecond = ssecond - first;
		Sub2by2Matrix_Inversion(A4,first,ssecond);
	}while(ssecond==first);
//*//////

	Zero(A5,second*second);
	MatrixMulti(A4,A3,A5,second,first,second);
	Zero(A4,second*second);
	MatrixMulti(A2,A5,A4,first,first,second);
	for(i=0; i<first; i++)		//A1
	{
		for(j=0; j<first; j++)
		{
			A6[i*first + j] -= A4[i*first + j];
		}
	}
	MatrixInversion(A6,first);	//upper left

	MatrixMulti(A5,A6,A9,second,first,first);	//lower left
	for(i=0; i<second; i++)		
	{
		for(j=0; j<first; j++)
		{
			A9[i*first + j] = -A9[i*first + j];
		}
	}

	Zero(A5,second*second);
	MatrixMulti(A1,A2,A5,first,second,first);
	Zero(A4,second*second);
	MatrixMulti(A3,A5,A4,second,second,first);
	for(i=0; i<second; i++)		//A4
	{
		for(j=0; j<second; j++)
		{
			A7[i*second + j] -= A4[i*second + j];
		}
	}
	
	MatrixInversion(A7,second);	//lower right
/*///////
	do
	{
		second = second - first;
		Sub2by2Matrix_Inversion(A7,first,second);
	}while(second==first);
//*//////////
	MatrixMulti(A5,A7,A8,first,second,second);	//upper right
	for(i=0; i<first; i++)		
	{
		for(j=0; j<second; j++)
		{
			A8[i*second + j] = -A8[i*second + j];
		}
	}
//////////////////	Export Inverted Matrix
	for(i=0; i<first; i++)		//A1
	{
		for(j=0; j<first; j++)
		{
			N[i*paraNumber + j] = A6[i*first + j];
		}
	}

	for(i=0; i<first; i++)		//A2
	{
		for(j=0; j<second; j++)
		{
			 N[i*paraNumber + first + j] = A8[i*second + j];
		}
	}

	for(i=0; i<second; i++)		//A3
	{
		for(j=0; j<first; j++)
		{
			N[(i + first)*paraNumber + j] = A9[i*first + j];
		}
	}

	for(i=0; i<second; i++)		//A4
	{
		for(j=0; j<second; j++)
		{
			N[(i + first)*paraNumber + first + j] = A7[i*second + j];
		}
	}
	delete A1;	delete A2;	delete A3;	delete A4; delete A5;	delete A6; delete A7;	delete A8; delete A9;
}

/***********************************
法方程矩阵分块求逆函数，最大可能分为9块
				   |      |
			   A1  |  A2  |   A3
			-------|------|-------
			   A4  |  A5  |   A6
			-------|------|-------
			   A7  |  A8  |   A9
				   |      |

  一般情况下分为4块
				   |      
			   A1  |  A2  
			-------|------
			   A3  |  A4  
                   |

  Input:
		N                 上三角形式的法方程矩阵
		L                 法方程的常数项
		photoNumber       有效相片数
		Cmr_ParaNum       相机参数未知数个数0--6(Xs,Ys,Zs,Phi,Omega,Kappa)
		ValidSpcPtNum     有效空间点数
		Spc_ParaNum       空间点未知数个数0---3 个(x,y,z)
		IntrinsicParaNum  内方位及系统差元素

  Output:
       N                  求逆后的法方程矩阵
	   X                  解向量

  2001/11/02              张永军
***********************************/
BOOL CMatrix::PartitionMatrixInversion(double *N, double *L, double *X, int photoNumber,int Cmr_ParaNum,int ValidSpcPtNum, int Spc_ParaNum, int IntrParaNum)
{
	int i=0,j=0,k=0;
	double *A1 = NULL,*A2 = NULL,*A3 = NULL,*A4 = NULL,*A5 = NULL;
	double *A6 = NULL,*A7 = NULL,*A8 = NULL,*A9 = NULL,*AA = NULL;
	double *B1 = NULL,*B2 = NULL,*B3 = NULL;

	int paraNumber = photoNumber*Cmr_ParaNum + ValidSpcPtNum*Spc_ParaNum + IntrParaNum;

	int extrParaNum = photoNumber*Cmr_ParaNum;
	int SpPtParaNum = ValidSpcPtNum*Spc_ParaNum;
	

	if(Cmr_ParaNum>0 && Spc_ParaNum>0 && IntrParaNum>0)	//分为9块：相机参数  空间坐标  内方位元素系统参数
	{
	//	Solve (N, L, X, paraNumber, paraNumber);

		MatrixInvResult(N, L, X, paraNumber);
	//	NinePartMatrixInversion(NN, L, X, photoNumber,Cmr_ParaNum,ValidSpcPtNum, Spc_ParaNum,IntrParaNum);

	}
	else if(Cmr_ParaNum>0 && Spc_ParaNum>0)			//分为4块: 相机参数   空间坐标
	{
		double *NN = new double [paraNumber*paraNumber];	//Symmetry Normal Equation Matrix
		for(i=0; i<paraNumber; i++)		
		{
			for(j=0; j<paraNumber; j++)
			{
				if(i<=j)
					NN[i*paraNumber + j] = N[i*(2*paraNumber-i-1)/2+j] ;
				else
					NN[i*paraNumber + j] = N[j*(2*paraNumber-j-1)/2+i] ;
			}
		}

		FourPartMatrixInversion(NN, L, X, photoNumber,Cmr_ParaNum,ValidSpcPtNum, Spc_ParaNum);

////////////
		k = 0;
		for(i=0; i<paraNumber; i++)		
		{
			for(j=0; j<paraNumber; j++)
			{
				if(i<=j)
					N[k++] = NN[i*paraNumber+j] ;
			}
		}

//*//////////////////////

		delete NN;
	}
	else if(Cmr_ParaNum>0 && IntrParaNum>0)	//分为4块: 相机参数   内方位元素系统参数
	{
		MatrixInvResult(N, L, X, paraNumber);
	}
  	return TRUE;
}

//法方程系数矩阵由 外方位元素 和 空间坐标未知数 组成时的分块求逆法
//  2001/11/02              张永军
void CMatrix::FourPartMatrixInversion(double *NN, double *L, double *X, int photoNumber, int Cmr_ParaNum, int ValidSpcPtNum, int Spc_ParaNum)
{
	int paraNumber = photoNumber*Cmr_ParaNum + ValidSpcPtNum*Spc_ParaNum;
	int extrParaNum = photoNumber*Cmr_ParaNum;
	int SpPtParaNum = ValidSpcPtNum*Spc_ParaNum;
	int i=0,j=0,k=0;
	double *A1 = NULL,*A2 = NULL,*A3 = NULL,*A4 = NULL,*A5 = NULL;
	double *A6 = NULL,*A7 = NULL,*A8 = NULL,*A9 = NULL,*AA = NULL;
	double *B1 = NULL,*B2 = NULL,*B3 = NULL;

	A1 = new double [extrParaNum*extrParaNum];
	A2 = new double [extrParaNum*SpPtParaNum];
	A3 = new double [SpPtParaNum*extrParaNum];
	A4 = new double [SpPtParaNum*SpPtParaNum];

	A5 = new double [extrParaNum*SpPtParaNum];
	A6 = new double [extrParaNum*extrParaNum];
	A7 = new double [extrParaNum];
	B1 = new double [extrParaNum];
	B2 = new double [SpPtParaNum];
	B3 = new double [SpPtParaNum];


	for(i=0; i<extrParaNum; i++)		//A1
	{
		for(j=0; j<extrParaNum; j++)
		{
			A1[i*extrParaNum + j] = NN[i*paraNumber + j];
		}
		B1[i] = L[i];					//B1
	}

	for(i=0; i<extrParaNum; i++)		//A2
	{
		for(j=0; j<SpPtParaNum; j++)
		{
			A2[i*SpPtParaNum + j] = NN[i*paraNumber + extrParaNum + j];
		}
	}

	MatrixTranspose(A2,A3,extrParaNum,SpPtParaNum);	//A3

	for(i=0; i<SpPtParaNum; i++)		//A4
	{
		for(j=0; j<SpPtParaNum; j++)
		{
			A4[i*SpPtParaNum + j] = NN[(i + extrParaNum)*paraNumber + extrParaNum + j];
		}
		B2[i] = L[extrParaNum + i];				//B2
	}

	double *tmpSpcN = new double [Spc_ParaNum*Spc_ParaNum];	//temp sub normal equation of space point
	int ln1 = 0,ln2 = 0;

	for(k=0; k<ValidSpcPtNum; k++)		//Invert Matrix A4
	{
		for(i=0; i<Spc_ParaNum; i++,ln1++)		
		{
			for(j=0; j<Spc_ParaNum; j++)
			{
				tmpSpcN[i*Spc_ParaNum + j] = A4[ln1*SpPtParaNum + k*Spc_ParaNum + j];
			}
		}
		
		MatrixInversion(tmpSpcN,Spc_ParaNum);

		for(i=0; i<Spc_ParaNum; i++,ln2++)		
		{
			for(j=0; j<Spc_ParaNum; j++)
			{
				A4[ln2*SpPtParaNum + k*Spc_ParaNum + j] = tmpSpcN[i*Spc_ParaNum + j] ;
			}
		}
	}

//	MatrixMulti(A2,A4,A5,extrParaNum,SpPtParaNum,SpPtParaNum);	//A2*A4 = A5
///////////////////////////
	Zero(A5,extrParaNum*SpPtParaNum);

	int spcID = 0;
	for(i=0; i<SpPtParaNum; i++)		
	{
		if(i>=Spc_ParaNum && i%Spc_ParaNum==0)
			spcID++;

		for(j=0; j<extrParaNum; j++)
		{
			for(k=0; k<Spc_ParaNum; k++)
			{
				A5[i*extrParaNum + j] += A4[i*SpPtParaNum + spcID*Spc_ParaNum + k]*A3[(k + spcID*Spc_ParaNum)*extrParaNum + j];
			}
		}
	}
	double *tmpA5 = new double [extrParaNum*SpPtParaNum];
	MatrixTranspose(A5,tmpA5,SpPtParaNum,extrParaNum);	
	for(i=0; i<extrParaNum; i++)		
	{
		for(j=0; j<SpPtParaNum; j++)
		{
			A5[i*SpPtParaNum + j] =tmpA5[i*SpPtParaNum + j];
		}
	}
///////////////////////////

	MatrixMulti(A5,A3,A6,extrParaNum,extrParaNum,SpPtParaNum);	//A5*A3
	
	MatrixMulti(A5,B2,A7,extrParaNum,1,SpPtParaNum);			//A5*B2

	for(i=0; i<extrParaNum; i++)		
	{
		for(j=0; j<extrParaNum; j++)
		{
			A1[i*extrParaNum + j] -= A6[i*extrParaNum + j] ;
		}
		B1[i] -= A7[i];
	}
	MatrixInversion(A1,extrParaNum);
	MatrixMulti(A1,B1,X,extrParaNum,1,extrParaNum);	//X1

//	MatrixMulti(A4,A3,A5,SpPtParaNum,extrParaNum,SpPtParaNum);	//A4*A3 = A5
	MatrixTranspose(tmpA5,A5,extrParaNum,SpPtParaNum);	
	delete tmpA5;
/////////////////////////////////////////////////////////////

	MatrixMulti(A5,X,B3,SpPtParaNum,1,extrParaNum);	//B3

	for(j=0; j<SpPtParaNum; j++)
	{
		X[extrParaNum + j] = -B3[j];	//X2
	}

	MatrixMulti(A4,B2,B3,SpPtParaNum,1,SpPtParaNum);	//A4*B2
	for(j=0; j<SpPtParaNum; j++)
	{
		X[extrParaNum + j] += B3[j];	//X2
	}

	if(ExportCoviaranceMatrix == TRUE)	//输出协因数矩阵
	{

		A8 = new double [extrParaNum*extrParaNum];
		A9 = new double [SpPtParaNum*extrParaNum];
		AA = new double [extrParaNum*SpPtParaNum];

		for(i=0; i<extrParaNum; i++)		//A1
		{
			for(j=0; j<extrParaNum; j++)
			{
				A8[i*extrParaNum + j] = NN[i*paraNumber + j];
			}
		}
		double *tmpCmrN = new double [Cmr_ParaNum*Cmr_ParaNum];	//temp sub normal equation of space point
		int ln1 = 0,ln2 = 0;

		for(k=0; k<photoNumber; k++)		//Invert Matrix A1
		{
			for(i=0; i<Cmr_ParaNum; i++,ln1++)		
			{
				for(j=0; j<Cmr_ParaNum; j++)
				{
					tmpCmrN[i*Cmr_ParaNum + j] = A8[ln1*extrParaNum + k*Cmr_ParaNum + j];
				}
			}
			
			MatrixInversion(tmpCmrN,Cmr_ParaNum);

			for(i=0; i<Cmr_ParaNum; i++,ln2++)		
			{
				for(j=0; j<Cmr_ParaNum; j++)
				{
					A8[ln2*extrParaNum + k*Cmr_ParaNum + j] = tmpCmrN[i*Cmr_ParaNum + j] ;
				}
			}

		}
		delete tmpCmrN;

		MatrixMulti(A5,A1,A9,SpPtParaNum,extrParaNum,extrParaNum);	//A5*A3 = A9

		MatrixMulti(A8,A2,AA,extrParaNum,SpPtParaNum,extrParaNum);	
		MatrixMulti(A3,AA,A4,SpPtParaNum,SpPtParaNum,extrParaNum);	

		for(i=0; i<SpPtParaNum; i++)		//A4
		{
			for(j=0; j<SpPtParaNum; j++)
			{
				A4[i*SpPtParaNum + j] = NN[(i + extrParaNum)*paraNumber + extrParaNum + j] -A4[i*SpPtParaNum + j] ;
			}
		}
		MatrixInversion(A4,SpPtParaNum);
/*///////////////////
		int first = ValidSpcPtNum/2;
	//	int first = 1;
		int second = ValidSpcPtNum - first;
		first = first*Spc_ParaNum;
		second = second*Spc_ParaNum;

		Sub2by2Matrix_Inversion(A4,first,second);
//*//////////////////
		MatrixMulti(AA,A4,A2,extrParaNum,SpPtParaNum,SpPtParaNum);	
/////////////////////////
		for(i=0; i<extrParaNum; i++)		//A1
		{
			for(j=0; j<extrParaNum; j++)
			{
				NN[i*paraNumber + j] = A1[i*extrParaNum + j] ;
			}
		}

		for(i=0; i<extrParaNum; i++)		//A2
		{
			for(j=0; j<SpPtParaNum; j++)
			{
				NN[i*paraNumber + extrParaNum + j] = -A2[i*SpPtParaNum + j];
			}
		}

		for(i=0; i<SpPtParaNum; i++)		//A3
		{
			for(j=0; j<extrParaNum; j++)
			{
				NN[(i + extrParaNum)*paraNumber + j] = -A9[i*extrParaNum + j];
			}
		}

		for(i=0; i<SpPtParaNum; i++)		//A4
		{
			for(j=0; j<SpPtParaNum; j++)
			{
				NN[(i + extrParaNum)*paraNumber + extrParaNum + j] = A4[i*SpPtParaNum + j];
			}
		}
		delete A8;	delete A9; 	delete AA; 
	}
	delete A1;	delete A2;	delete A3;	delete A4; delete A5;	delete A6; delete A7;
	delete B1;	delete B2;	delete B3;	delete tmpSpcN;	 
}


//病态矩阵求解
void CMatrix::Ill_Condition_Matrix_Sovle(double *N, double *L, double *X,double *Qxx, int n,int times, double espAngle,double espPoint)
{
	int i,j;
	int redoTimes = 0;
	double maxError = 0;
	double *Last_X = new double [n];
	double *Last_Q = new double [n*n];
	double *Curr_Q = new double [n*n];
	double *Q = new double [n*n];
	
	for(i=0;i<n;i++)	
	{
		for(j=0;j<n;j++)
		{
			Q[i*n + j] = N[i*n + j];	//Initialize  Q
			Last_Q[i*n + j] = 0.0;	//Initialize Last_Q
			Curr_Q[i*n + j] = 0.0;	//Initialize Curr_Q
		}
		Last_Q[i*n + i] = 1.0;

		Last_X[i] = 0.0;
		Q[i*n+i] += 1.0;	//岭参数
	}

	MatrixInversion(Q, n);	//法方程系数矩阵求逆

	for(i=0;i<n;i++)	
		Last_X[i] = L[i]; //未知数初值  Y=U
	for(i=0;i<n;i++)	
	{
		for(j=0;j<n;j++)
			Last_Q[i*n + j] = Q[i*n + j];	//Initialize Last_Q  M=q
	}
	do
	{
		maxError = 0.0;
		MatrixMulti(Last_Q,Q,Curr_Q,n,n,n);	

		for(i=0;i<n;i++)	
			for(j=0;j<n;j++)
				Last_Q[i*n + j] = Curr_Q[i*n + j] + Q[i*n + j];

		MatrixMulti(Last_Q,N,Curr_Q,n,n,n);	
		MatrixMulti(Curr_Q,Last_Q,Qxx,n,n,n);	

		MatrixMulti(Qxx ,L,X,n,1,n);	
		for(i=0;i<n;i++)
		{
			if(fabs(X[i] - Last_X[i])>fabs(maxError))
				maxError = X[i] - Last_X[i];
		}

		for(i=0;i<n;i++)
			Last_X[i] = X[i]; //备份上一次未知数近似值

		redoTimes++;

	}while( (espPoint<fabs(maxError))&&(redoTimes<times));

/*
	TRACE("X= ");
	for(i=0;i<n;i++)	 TRACE("%10.5lf ",X[i]);
	TRACE("redoTimes = %3d\n",redoTimes);
	TRACE("Qxx=\n");
	for(i=0;i<n;i++)	
	{
		for(j=0;j<n;j++)
			 TRACE("%10.5lf ",Qxx[i*n + j]);
		TRACE("\n");
	}
*/
	MatrixInversion(N, n);	//法方程系数矩阵求逆

	delete Last_X;delete Q;
	delete Last_Q;delete Curr_Q;

}


//矩阵奇异值分解
BOOL  CMatrix::SingularValueDecomposition(double *a, int m,int  n, double *u,double * v, double eps, int ka)

{
    int        i, j, k, l, it, ll, kk, ix, iy, mm, nn, iz, m1, ks;
    double     d, dd, t, sm, sm1, em1, sk, ek, b, c, shh, fg[2], cs[2];
    double     *s, *e, *w;
    
//    s = malloc(ka * sizeof(double));
//    e = malloc(ka * sizeof(double));
//    w = malloc(ka * sizeof(double));
	s = new double [ka];
	e = new double [ka];
	w = new double [ka];
    it = 60;
    k = n;
    
    if(m - 1 < n)
        k = m - 1;
    l = m;
    
    if(n - 2 < m)
        l = n - 2;
    
    if(l < 0)
        l = 0;
    ll = k;
    
    if(l > k)
        ll = l;
    
    if(ll >= 1)
    {
        for(kk = 1; kk <= ll; kk++)
        {
            if(kk <= k)
            {
                d = 0.0;
                
                for(i = kk; i <= m; i++)
                {
                    ix = (i - 1) * n + kk - 1;
                    d = d + a[ix] * a[ix];
                }
                s[kk - 1] = sqrt(d);
                
                if(s[kk - 1] != 0.0)
                {
                    ix = (kk - 1) * n + kk - 1;
                    
                    if(a[ix] != 0.0)
                    {
                        s[kk - 1] = fabs(s[kk - 1]);
                        
                        if(a[ix] < 0.0)
                            s[kk - 1] = -s[kk - 1];
                    }
                    
                    for(i = kk; i <= m; i++)
                    {
                        iy = (i - 1) * n + kk - 1;
                        a[iy] = a[iy] / s[kk - 1];
                    }
                    a[ix] = 1.0 + a[ix];
                }
                s[kk - 1] = -s[kk - 1];
            }
            
            if(n >= kk + 1)
            {
                for(j = kk + 1; j <= n; j++)
                {
                    if((kk <= k) && (s[kk - 1] != 0.0))
                    {
                        d = 0.0;
                        
                        for(i = kk; i <= m; i++)
                        {
                            ix = (i - 1) * n + kk - 1;
                            iy = (i - 1) * n + j - 1;
                            d = d + a[ix] * a[iy];
                        }
                        d = -d / a[(kk - 1) * n + kk - 1];
                        
                        for(i = kk; i <= m; i++)
                        {
                            ix = (i - 1) * n + j - 1;
                            iy = (i - 1) * n + kk - 1;
                            a[ix] = a[ix] + d * a[iy];
                        }
                    }
                    e[j - 1] = a[(kk - 1) * n + j - 1];
                }
            }
            
            if(kk <= k)
            {
                for(i = kk; i <= m; i++)
                {
                    ix = (i - 1) * m + kk - 1;
                    iy = (i - 1) * n + kk - 1;
                    u[ix] = a[iy];
                }
            }
            
            if(kk <= l)
            {
                d = 0.0;
                
                for(i = kk + 1; i <= n; i++)
                    d = d + e[i - 1] *e[i - 1];
                e[kk - 1] = sqrt(d);
                
                if(e[kk - 1] != 0.0)
                {
                    if(e[kk] != 0.0)
                    {
                        e[kk - 1] = fabs(e[kk - 1]);
                        
                        if(e[kk] < 0.0)
                            e[kk - 1] = -e[kk - 1];
                    }
                    
                    for(i = kk + 1; i <= n; i++)
                        e[i - 1] = e[i - 1] / e[kk - 1];
                    e[kk] = 1.0 + e[kk];
                }
                e[kk - 1] = -e[kk - 1];
                
                if((kk + 1 <= m) && (e[kk - 1] != 0.0))
                {
                    for(i = kk + 1; i <= m; i++)
                        w[i - 1] = 0.0;
                    
                    for(j = kk + 1; j <= n; j++)
                        for(i = kk + 1; i <= m; i++)
                            w[i - 1] = w[i - 1] +e[j - 1] *a[(i - 1) * n + j - 1];
                    
                    for(j = kk + 1; j <= n; j++)
                        for(i = kk + 1; i <= m; i++)
                        {
                            ix = (i - 1) * n + j - 1;
                            a[ix] = a[ix] - w[i - 1] *e[j - 1] / e[kk];
                        }
                }
                
                for(i = kk + 1; i <= n; i++)
                    v[(i - 1) * n + kk - 1] = e[i - 1];
            }
        }
    }
    mm = n;
    
    if(m + 1 < n)
        mm = m + 1;
    
    if(k < n)
        s[k] = a[k * n + k];
    
    if(m < mm)
        s[mm - 1] = 0.0;
    
    if(l + 1 < mm)
        e[l] = a[l * n + mm - 1];
    e[mm - 1] = 0.0;
    nn = m;
    
    if(m > n)
        nn = n;
    
    if(nn >= k + 1)
    {
        for(j = k + 1; j <= nn; j++)
        {
            for(i = 1; i <= m; i++)
                u[(i - 1) * m + j - 1] = 0.0;
            u[(j - 1) * m + j - 1] = 1.0;
        }
    }
    
    if(k >= 1)
    {
        for(ll = 1; ll <= k; ll++)
        {
            kk = k - ll + 1;
            iz = (kk - 1) * m + kk - 1;
            
            if(s[kk - 1] != 0.0)
            {
                if(nn >= kk + 1)
                    for(j = kk + 1; j <= nn; j++)
                    {
                        d = 0.0;
                        
                        for(i = kk; i <= m; i++)
                        {
                            ix = (i - 1) * m + kk - 1;
                            iy = (i - 1) * m + j - 1;
                            d = d + u[ix] * u[iy] / u[iz];
                        }
                        d = -d;
                        
                        for(i = kk; i <= m; i++)
                        {
                            ix = (i - 1) * m + j - 1;
                            iy = (i - 1) * m + kk - 1;
                            u[ix] = u[ix] + d * u[iy];
                        }
                    }
                
                for(i = kk; i <= m; i++)
                {
                    ix = (i - 1) * m + kk - 1;
                    u[ix] = -u[ix];
                }
                u[iz] = 1.0 + u[iz];
                
                if(kk - 1 >= 1)
                    for(i = 1; i <= kk - 1; i++)
                        u[(i - 1) * m + kk - 1] = 0.0;
            }
            else
            {
                for(i = 1; i <= m; i++)
                    u[(i - 1) * m + kk - 1] = 0.0;
                u[(kk - 1) * m + kk - 1] = 1.0;
            }
        }
    }
    
    for(ll = 1; ll <= n; ll++)
    {
        kk = n - ll + 1;
        iz = kk * n + kk - 1;
        
        if((kk <= l) && (e[kk - 1] != 0.0))
        {
            for(j = kk + 1; j <= n; j++)
            {
                d = 0.0;
                
                for(i = kk + 1; i <= n; i++)
                {
                    ix = (i - 1) * n + kk - 1;
                    iy = (i - 1) * n + j - 1;
                    d = d + v[ix] * v[iy] / v[iz];
                }
                d = -d;
                
                for(i = kk + 1; i <= n; i++)
                {
                    ix = (i - 1) * n + j - 1;
                    iy = (i - 1) * n + kk - 1;
                    v[ix] = v[ix] + d * v[iy];
                }
            }
        }
        
        for(i = 1; i <= n; i++)
            v[(i - 1) * n + kk - 1] = 0.0;
        v[iz - n] = 1.0;
    }
    
    for(i = 1; i <= m; i++)
        for(j = 1; j <= n; j++)
            a[(i - 1) * n + j - 1] = 0.0;
    m1 = mm;
    it = 60;
    
    while(1 == 1)
    {
        if(mm == 0)
        {
            a_ppp(a, e, s, v, m, n);
            free(s);
            free(e);
            free(w);
            return(1);
        }
        
        if(it == 0)
        {
            a_ppp(a, e, s, v, m, n);
            free(s);
            free(e);
            free(w);
            return(-1);
        }
        kk = mm - 1;
        
        while((kk != 0) && (fabs(e[kk -1]) != 0.0))
        {
            d = fabs(s[kk-1])+fabs(s[kk]);
            dd = fabs(e[kk - 1]);
            
            if(dd > eps * d)
                kk = kk - 1;
            else
                e[kk - 1] = 0.0;
        }
        
        if(kk == mm - 1)
        {
            kk = kk + 1;
            
            if(s[kk - 1] < 0.0)
            {
                s[kk - 1] = -s[kk - 1];
                
                for(i = 1; i <= n; i++)
                {
                    ix = (i - 1) * n + kk - 1;
                    v[ix] = -v[ix];
                }
            }
            
            while((kk != m1) && (s[kk - 1] < s[kk]))
            {
                d = s[kk - 1];
                s[kk - 1] = s[kk];
                s[kk] = d;
                
                if(kk < n)
                    for(i = 1; i <= n; i++)
                    {
                        ix = (i - 1) * n + kk - 1;
                        iy = (i - 1) * n + kk;
                        d = v[ix];
                        v[ix] = v[iy];
                        v[iy] = d;
                    }
                
                if(kk < m)
                    for(i = 1; i <= m; i++)
                    {
                        ix = (i - 1) * m + kk - 1;
                        iy = (i - 1) * m + kk;
                        d = u[ix];
                        u[ix] = u[iy];
                        u[iy] = d;
                    }
                kk = kk + 1;
            }
            it = 60;
            mm = mm - 1;
        }
        else
        {
            ks = mm;
            
            while((ks > kk) && (fabs(s[ks -1]) != 0.0))
            {
                d = 0.0;
                
                if(ks != mm)
                    d = d + fabs(e[ks - 1]);
                
                if(ks != kk + 1)
                    d = d + fabs(e[ks - 2]);
                dd = fabs(s[ks - 1]);
                
                if(dd > eps * d)
                    ks = ks - 1;
                else
                    s[ks - 1] = 0.0;
            }
            
            if(ks == kk)
            {
                kk = kk + 1;
                d = fabs(s[mm - 1]);
                t = fabs(s[mm - 2]);
                
                if(t > d)
                    d = t;
                t = fabs(e[mm - 2]);
                
                if(t > d)
                    d = t;
                t = fabs(s[kk - 1]);
                
                if(t > d)
                    d = t;
                t = fabs(e[kk - 1]);
                
                if(t > d)
                    d = t;
                sm = s[mm - 1] / d;
                sm1 = s[mm - 2] / d;
                em1 = e[mm - 2] / d;
                sk = s[kk - 1] / d;
                ek = e[kk - 1] / d;
                b = ((sm1 + sm) * (sm1 - sm) + em1 * em1) / 2.0;
                c = sm * em1;
                c = c * c;
                shh = 0.0;
                
                if((b != 0.0) || (c != 0.0))
                {
                    shh = sqrt(b * b + c);
                    
                    if(b < 0.0)
                        shh = -shh;
                    shh = c / (b + shh);
                }
                fg[0] = (sk + sm) * (sk - sm) - shh;
                fg[1] = sk * ek;
                
                for(i = kk; i <= mm - 1; i++)
                {
                    a_sss(fg, cs);
                    
                    if(i != kk)
                        e[i - 2] = fg[0];
                    fg[0] = cs[0] * s[i - 1] +cs[1] * e[i - 1];
                    e[i - 1] = cs[0] * e[i - 1] -cs[1] * s[i - 1];
                    fg[1] = cs[1] * s[i];
                    s[i] = cs[0] * s[i];
                    
                    if((cs[0] != 1.0) || (cs[1] != 0.0))
                        for(j = 1; j <= n; j++)
                        {
                            ix = (j - 1) * n + i - 1;
                            iy = (j - 1) * n + i;
                            d = cs[0] * v[ix] + cs[1] * v[iy];
                            v[iy] = -cs[1] * v[ix] + cs[0] * v[iy];
                            v[ix] = d;
                        }
                    a_sss(fg, cs);
                    s[i - 1] = fg[0];
                    fg[0] = cs[0] * e[i - 1] +cs[1] * s[i];
                    s[i] = -cs[1] * e[i - 1] +cs[0] * s[i];
                    fg[1] = cs[1] * e[i];
                    e[i] = cs[0] * e[i];
                    
                    if(i < m)
                        if((cs[0] != 1.0) || (cs[1] != 0.0))
                            for(j = 1; j <= m; j++)
                            {
                                ix = (j - 1) * m + i - 1;
                                iy = (j - 1) * m + i;
                                d = cs[0] * u[ix] + cs[1] * u[iy];
                                u[iy] = -cs[1] * u[ix] + cs[0] * u[iy];
                                u[ix] = d;
                            }
                }
                e[mm - 2] = fg[0];
                it = it - 1;
            }
            else
            {
                if(ks == mm)
                {
                    kk = kk + 1;
                    fg[1] = e[mm - 2];
                    e[mm - 2] = 0.0;
                    
                    for(ll = kk; ll <= mm - 1; ll++)
                    {
                        i = mm + kk - ll - 1;
                        fg[0] = s[i - 1];
                        a_sss(fg, cs);
                        s[i - 1] = fg[0];
                        
                        if(i != kk)
                        {
                            fg[1] = -cs[1] * e[i - 2];
                            e[i - 2] = cs[0] * e[i - 2];
                        }
                        
                        if((cs[0] != 1.0) || (cs[1] != 0.0))
                            for(j = 1; j <= n; j++)
                            {
                                ix = (j - 1) * n + i - 1;
                                iy = (j - 1) * n + mm - 1;
                                d = cs[0] * v[ix] + cs[1] * v[iy];
                                v[iy] = -cs[1] * v[ix] + cs[0] * v[iy];
                                v[ix] = d;
                            }
                    }
                }
                else
                {
                    kk = ks + 1;
                    fg[1] = e[kk - 2];
                    e[kk - 2] = 0.0;
                    
                    for(i = kk; i <= mm; i++)
                    {
                        fg[0] = s[i - 1];
                        a_sss(fg, cs);
                        s[i - 1] = fg[0];
                        fg[1] = -cs[1] * e[i - 1];
                        e[i - 1] = cs[0] * e[i - 1];
                        
                        if((cs[0] != 1.0) || (cs[1] != 0.0))
                            for(j = 1; j <= m; j++)
                            {
                                ix = (j - 1) * m + i - 1;
                                iy = (j - 1) * m + kk - 2;
                                d = cs[0] * u[ix] + cs[1] * u[iy];
                                u[iy] = -cs[1] * u[ix] + cs[0] * u[iy];
                                u[ix] = d;
                            }
                    }
                }
            }
        }
    }
    return(1);
}
//奇异值分解子函数
void  CMatrix::a_ppp(double *a, double *e, double *s, double *v,int m, int n)
{
    int        i, j, p, q;
    double     d;
    
    if(m >= n)
        i = n;
    else
        i = m;
    
    for(j = 1; j <= i - 1; j++)
    {
        a[(j - 1) * n + j - 1] = s[j - 1];
        a[(j - 1) * n + j] = e[j - 1];
    }
    a[(i - 1) * n + i - 1] = s[i - 1];
    
    if(m < n)
        a[(i - 1) * n + i] = e[i - 1];
    
    for(i = 1; i <= n - 1; i++)
        for(j = i + 1; j <= n; j++)
        {
            p = (i - 1) * n + j - 1;
            q = (j - 1) * n + i - 1;
            d = v[p];
            v[p] = v[q];
            v[q] = d;
        }
    return;
}

//奇异值分解子函数
void  CMatrix::a_sss(double  fg [2], double cs[2])
{
    double     r, d;
    
    if((fabs(fg[0])+fabs(fg[1])) == 0.0)
    {
        cs[0] = 1.0;
        cs[1] = 0.0;
        d = 0.0;
    }
    else
    {
        d = sqrt(fg[0] * fg[0] + fg[1] * fg[1]);
        
        if(fabs(fg[0]) > fabs(fg[1]))
        {
            d = fabs(d);
            
            if(fg[0] < 0.0)
                d = -d;
        }
        
        if(fabs(fg[1]) >= fabs(fg[0]))
        {
            d = fabs(d);
            
            if(fg[1] < 0.0)
                d = -d;
        }
        cs[0] = fg[0] / d;
        cs[1] = fg[1] / d;
    }
    r = 1.0;
    
    if(fabs(fg[0]) > fabs(fg[1]))
        r = cs[1];
    else if(cs[0] != 0.0)
        r = 1.0 / cs[0];
    fg[0] = d;
    fg[1] = r;
    return;
}

//矩阵条件数计算
double CMatrix::Matrix_Condition(double *matrix, int m, int n)
{
	double *U = new double [m*m];
	double *V = new double [n*n];
	double eps = 1e-15;
	int i,k;
	SingularValueDecomposition(matrix,  m,  n, U,V,eps, max(m,n)+1);
	double min = 2000000.0,max=0.000001;
	for( k=0;k<m;k++)
	{
		for(i=0;i<n;i++)
		{
			if(fabs(matrix[k*m+i])>=fabs(max))
			{
				max = matrix[k*m+i];
			}

			if(fabs(matrix[k*m+i])>0.00 && fabs(matrix[k*m+i])<=fabs(min))
			{
				min = matrix[k*m+i];
			}
		}
	}
	delete U;
	delete V;
	return max/min;
}

//////////////////////////////////////////////////////
//
//矩阵填充
//
//a矩阵为m*n阶，b为p*q阶（小块矩阵）,
//将b放在a的第(r,c)个元素处(r=0,1……m-1，c=0,1,……n-1)
//
//////////////////////////////////////////////////////
void CMatrix::FillMatrix(double *a,double *b,int m,int n,int p,int q,int r,int c)
{
	for(int i=0;i<p;i++)
	{
		for(int j=0;j<q;j++)		
		{
             *(a+(r+i)*n+c+j) += *(b+i*q+j);
		}
	}
}

//////////////////////////////////////////////////////
//
//矩阵填充--------相减
//
//a矩阵为m*n阶，b为p*q阶（小块矩阵）,
//将b放在a的第(r,c)个元素处(r=0,1……m-1，c=0,1,……n-1)
//
//////////////////////////////////////////////////////
void CMatrix::FillMatrix_MINUS(double *a,double *b,int m,int n,int p,int q,int r,int c)
{
	for(int i=0;i<p;i++)
	{
		for(int j=0;j<q;j++)		
		{
             *(a+(r+i)*n+c+j) -= *(b+i*q+j);
		}
	}
}

/////////////////////////////////////////////////////////////
//功  能：矩阵相加
//输  入：矩阵a（mxn阶)加上矩阵b（mxn阶）放在矩阵a（mxn阶）
//输  出：void，结果在矩阵a中
/////////////////////////////////////////////////////////////
void CMatrix::MatrixAdd(double *a, double *b,int m, int n)
{
	int i,j;
	for(i=0;i<m;i++)
	{
		for(j=0;j<n;j++)
		{
			*(a+i*n+j) += *(b+i*n+j);
		}
	}
}

