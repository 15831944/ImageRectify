#include <afx.h>
#include <stdio.h>
#include <iostream>
#include <Shlwapi.h>
#include "GDAL\gdal.h"
#include "GDAL\gdal_priv.h"
#include "geomagic.hpp"
#include <omp.h>

#pragma comment(lib,"gdal_i.lib")


//获取文件目录
CString FunGetFileFolder(CString strFullPath)
{
	CString strFolder = strFullPath.Left(strFullPath.ReverseFind('\\'));
	return strFolder;
}
//获取文件名
CString FunGetFileName(CString strFullPath, bool bExt)
{
	CString strName; int nPos1 = 0;
	nPos1 = strFullPath.ReverseFind('\\');
	if (bExt)
	{
		strName = strFullPath.Right(strFullPath.GetLength() - nPos1 - 1);
	}
	else
	{
		int nPos2 = strFullPath.ReverseFind('.');
		if (nPos2 == -1) nPos2 = strFullPath.GetLength();
		strName = strFullPath.Mid(nPos1 + 1, nPos2 - nPos1 - 1);
	}
	return strName;
}
//获取文件后缀
CString FunGetFileExt(CString strFullPath)
{
	CString strExt = strFullPath.Right(strFullPath.GetLength() - strFullPath.ReverseFind('.') - 1);
	return strExt;
}
//分割字符串
vector<CString> FunStrTok(CString str, CString strDot)
{
	vector<CString> vPart;
	char strInfo[1024]; strcpy(strInfo, str);
	char* p = strtok(strInfo, strDot);
	while (p != NULL)
	{
		vPart.push_back(p);
		p = strtok(NULL, strDot);
	}
	return vPart;
}
//生成文件目录
void FunCreateDir4Path(CString strPath)
{
	if (PathFileExists(strPath))	return;

	char tmpPath[MAX_PATH];
	//	strPath = FunGetFileFolder(strPath);
	const char* pCur = strPath;

	memset(tmpPath, 0, sizeof(tmpPath));

	int pos = 0;
	while (*pCur++ != '\0')
	{
		tmpPath[pos++] = *(pCur - 1);

		if (*pCur == '/' || *pCur == '\\' || *pCur == '\0')
		{
			if (!PathFileExists(tmpPath) && strlen(tmpPath) > 0)
			{
				CreateDirectory(tmpPath, NULL);
			}
		}
	}
}
CString FunGetThisExePath()
{
	char strExePath[1024]; memset(strExePath, 0, 1024);
	GetModuleFileName(NULL, strExePath, sizeof(strExePath));
	return strExePath;
}
bool   FunSearchFile(CString strFolderPath, CString strExt, vector<CString>& vecFilePath)
{
	if (!PathFileExists(strFolderPath))
	{
		return false;
	}
	CString path = strFolderPath;
	CString  strFileName, strFileExt;
	if (path.Right(1) != _T("\\"));//路径要以/结尾 
	path += _T("\\");
	path += _T("*.*");//CFileFind 类查找路径/*.gz时，指查找路径下所有文件 
					  //path += _T("*.tif");//CFileFind 类查找路径/*.tiff时，指查找路径下所有文件 
	CFileFind ff;
	BOOL res = ff.FindFile(path);
	while (res)
	{
		res = ff.FindNextFile();
		//如果不是. ..（.表示当前目录，..表示上一级目录，也是以文件形式存在的)
		//也不是路径/文件夹，即如果是文件，就删除，你的操作就是获取文件名然后和要查找的文件进行比较
		if (!ff.IsDots() && !ff.IsDirectory())
		{
			if (ff.IsReadOnly())
			{
				::SetFileAttributes(ff.GetFilePath(), FILE_ATTRIBUTE_NORMAL);
			}
			//strFilePath是当前搜索到的文件的完整路径，含文件名和后缀
			CString strFilePath;
			strFilePath = ff.GetFilePath();

			//取得当前文件路径及文件名
			for (int j = strFilePath.GetLength(); j>0; j--)
			{
				if (strFilePath.GetAt(j) == '\\')
				{
					strFileName = strFilePath.Right(strFilePath.GetLength() - j - 1);//截取路径中的文件名，含扩展名
					strFileExt = strFilePath.Right(strExt.GetLength());
					if (strFileExt.MakeLower() == strExt.MakeLower())
					{
						//strFilePathArray[NumFile] = strFilePath;//将搜索到的文件路径添加到文件路径数组中
						//m_FileList.InsertString(m_FileList.GetCount(),strFilePath);//添加到列表框中
						//strFileName = strFileName.Left(strFileName.GetLength()-5);//截取文件名不含扩展名
						vecFilePath.push_back(strFilePath);//文件名依次存入到文件名数组中
														   //NameFile.push_back(strFileName);
														   //n++;//计数搜索到的文件个数
														   /*FileNum++;*/
					}
					break;
				}
			}
		}
		else   if (ff.IsDots())   //如果是. ..，就继续。    
			continue;
		else    if (ff.IsDirectory())///*如果是文件夹，就递归进入此方法继续删除，你的操作就是递归进入此方法继续对比文件名，并缓存起来。    */
		{
			path = ff.GetFilePath();
			FunSearchFile(path, strExt, vecFilePath);
		}
	}

}

enum ErrorType
{
	ePara = 0,
	ePath = 1,
	eOpen = 2,
	eRead = 3,
	eIOpen = 4,
	eIRead = 5,
	eISave = 6
};
void errMsg(ErrorType eType)
{
	switch (eType)
	{
	case ePara:
		printf("Failed: exe parameters error!\n");
		printf("Please add the task path!\n");
		break;
	case ePath:
		printf("Failed: Task file path error!\n");
		break;
	case eOpen:
		printf("Failed: Task file open error!\n");
		break;
	case eRead:
		printf("Failed: Task file read error!\n");
		break;
	case eIOpen:
		printf("Failed: Image open error!\n");
		break;
	case eIRead:
		printf("Failed: Image read error!\n");
		break;
	case eISave:
		printf("Failed: Image save error!\n");
		break;
	default:
		break;
	}
}
// 分割字符串
// vector<CString> FunStrTok(CString str, CString strDot)
// {
// 	vector<CString> vPart;
// 	char strInfo[1024]; strcpy(strInfo, str);
// 	char* p = strtok(strInfo, strDot);
// 	while (p != NULL)
// 	{
// 		vPart.push_back(p);
// 		p = strtok(NULL, strDot);
// 	}
// 	return vPart;
// }
/************************************************************************/
/* 纠正影像像点与原始影像像点之间的转换                                    */
/* nCols, nRows为原始影像宽高；pos 为原始影像pos信息-一般通过下视影像转换*/
/************************************************************************/
Point2D imgPtOri2Rec(Point2D ptOri, int nCols, int nRows, cameraInfo pos, double resolution, double z)
{
	Point3D p1 = Img2XYZ(nCols, nRows, Point2D(0, 0), z, pos);
	Point3D p2 = Img2XYZ(nCols, nRows, Point2D(nCols, nRows), z, pos);
	Point3D p3 = Img2XYZ(nCols, nRows, Point2D(nCols, 0), z, pos);
	Point3D p4 = Img2XYZ(nCols, nRows, Point2D(0, nRows), z, pos);

	double max_x = max(max(p1.X, p2.X), max(p3.X, p4.X));
	double min_x = min(min(p1.X, p2.X), min(p3.X, p4.X));
	double max_y = max(max(p1.Y, p2.Y), max(p3.Y, p4.Y));
	double min_y = min(min(p1.Y, p2.Y), min(p3.Y, p4.Y));

	Point3D ptGeo = Img2XYZ(nCols, nRows, ptOri, z, pos);
	double x = (ptGeo.X - min_x) / resolution;
	double y = (ptGeo.Y - min_y) / resolution;
	return Point2D(x, y);
}
Point2D imgPtRec2Ori(Point2D ptRec, int nCols, int nRows, cameraInfo pos, double resolution, double z)
{
	Point3D p1 = Img2XYZ(nCols, nRows, Point2D(0, 0), z, pos);
	Point3D p2 = Img2XYZ(nCols, nRows, Point2D(nCols, nRows), z, pos);
	Point3D p3 = Img2XYZ(nCols, nRows, Point2D(nCols, 0), z, pos);
	Point3D p4 = Img2XYZ(nCols, nRows, Point2D(0, nRows), z, pos);

	double max_x = max(max(p1.X, p2.X), max(p3.X, p4.X));
	double min_x = min(min(p1.X, p2.X), min(p3.X, p4.X));
	double max_y = max(max(p1.Y, p2.Y), max(p3.Y, p4.Y));
	double min_y = min(min(p1.Y, p2.Y), min(p3.Y, p4.Y));

	Point3D ptGeo = Point3D(0, 0, z);
	ptGeo.X = min_x + ptRec.x*resolution;
	ptGeo.Y = min_y + ptRec.y*resolution;
	Point2D ptImg = XYZ2Img(nCols, nRows, ptGeo, pos);
	return ptImg;
}
//////////////////////////////////////////////////////////

bool readTask(CString strTask, CString &strInputPath, CString &strOutputPath, cameraInfo &pos, cameraInfo &dposImg2N, double &resolution, double &z)
{
	if (!PathFileExists(strTask))
	{
		errMsg(ePath);
		return false;
	}
	FILE *pR = fopen(strTask, "r");
	if (pR == NULL)
	{
		errMsg(eOpen);
		return false;
	}
	char line[1024]; memset(line, 0, 1024);
	CString str1, str2, str3, str4; int nCount = 0;
	while (fgets(line, 1024, pR))
	{
		if(line[strlen(line) - 1]=='\n')
			line[strlen(line) - 1] = '\0';
		nCount++;
		if (nCount == 1) str1 = line; else
			if (nCount == 2) str2 = line; else
				if (nCount == 3) str3 = line;else
					if (nCount == 4) str4 = line; else
						break;
	}
	if (nCount < 4)
	{
		errMsg(eRead);
		return false;
	}
	strInputPath = str1;
	strOutputPath = str2;
	vector<CString> strToks;
	strToks = FunStrTok(str3, " \t,");
	if (strToks.size() != 10)
	{
		errMsg(eRead);
		return false;
	}
	pos.Xs = atof(strToks[0]);
	pos.Ys = atof(strToks[1]);
	pos.Zs = atof(strToks[2]);
	pos.phi = atof(strToks[3]);
	pos.omg = atof(strToks[4]);
	pos.kap = atof(strToks[5]);
	pos.f = atof(strToks[6])/ atof(strToks[7]);
	resolution = atof(strToks[8]);
	z = atof(strToks[9]);
	pos.CalRotMatrixByPOK();
	strToks = FunStrTok(str4, " \t,");
	if (strToks.size() != 6)
	{
		errMsg(eRead);
		return false;
	}
	dposImg2N.Xs = -atof(strToks[0]);
	dposImg2N.Ys = -atof(strToks[1]);
	dposImg2N.Zs = -atof(strToks[2]);
	dposImg2N.phi = -atof(strToks[3]);
	dposImg2N.omg = -atof(strToks[4]);
	dposImg2N.kap = -atof(strToks[5]);
	vector<CString>().swap(strToks);
	return true;
}
void getImgGray(BYTE *data, int nbands, int nCols, int nRows,Point2D ptImg, int &value_r, int &value_g, int &value_b)
{
	float c = ptImg.x;
	float r = ptImg.y;
	if (c < 0 || r < 0 || c >= nCols || r >= nRows)
	{
		value_r = value_g = value_b = 0;
		return;
	}
	int r1, r2, c1, c2;
	float p1, p2, q1, q2;
	r1 = r;  p2 = r - r1;
	r2 = min(nRows-1,r + 1); p1 = 1 - p2;
	c1 = c;  q2 = c - c1;
	c2 = min(nCols-1,c + 1); q1 = 1 - q2;
	if (nbands == 1)
	{
		value_r = 
			*(data + r1*nCols + c1)*p1*q1 + 
			*(data + r2*nCols + c1)*p2*q1 +
			*(data + r2*nCols + c2)*p2*q2 +
			*(data + r1*nCols + c2)*p1*q2;
		value_g = value_r;
		value_b = value_r;
	}else
		if (nbands == 3)
		{

			value_r = 
				*(data + r1*nCols*nbands + c1*nbands +0)*p1*q1 +
				*(data + r2*nCols*nbands + c1*nbands +0)*p2*q1 +
				*(data + r2*nCols*nbands + c2*nbands +0)*p2*q2 +
				*(data + r1*nCols*nbands + c2*nbands +0)*p1*q2;

			value_g = 
				*(data + r1*nCols*nbands + c1*nbands + 1)*p1*q1 +
				*(data + r2*nCols*nbands + c1*nbands + 1)*p2*q1 +
				*(data + r2*nCols*nbands + c2*nbands + 1)*p2*q2 +
				*(data + r1*nCols*nbands + c2*nbands + 1)*p1*q2;

			value_b = 
				*(data + r1*nCols*nbands + c1*nbands + 2)*p1*q1 +
				*(data + r2*nCols*nbands + c1*nbands + 2)*p2*q1 +
				*(data + r2*nCols*nbands + c2*nbands + 2)*p2*q2 +
				*(data + r1*nCols*nbands + c2*nbands + 2)*p1*q2;
// 			if (value_r > 255 || value_g > 255 || value_b > 255)
// 			{
// 				printf("%lf %lf\n", c, r);
// 			}
		}
}
bool SaveImg(CString strSavePath, int nCols, int nRows, int nBands, BYTE*pMem, double *geo, CString strWTK, const char *pszFormat /* = JPEG */)
{
	if (strcmp(pszFormat, "GTiff") == 0)
	{
		GDALDriver *poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
		if (poDriver == NULL) return false;
		GDALDataset *WriteDataSet = poDriver->Create(strSavePath, nCols, nRows, nBands, GDT_Byte, NULL);
		int pBandMap[3] = { 1,2,3 };
		if (WriteDataSet->RasterIO(GF_Write, 0, 0, nCols, nRows, pMem, nCols, nRows, GDT_Byte, nBands, NULL, nBands * 1, nCols*nBands * 1, 1) == CE_Failure)
		{
			return false;
		}
		GDALClose(poDriver);
		GDALClose(WriteDataSet); WriteDataSet = NULL;
		return true;
	}
	else
	{
		if (strSavePath.IsEmpty() || pMem == NULL || nCols < 1 || nRows < 1 || nBands < 1)
		{
			return false;
		}

		CString GType = pszFormat;
		if (GType.IsEmpty()) { return false; }
		GDALDriver *pMemDriver = NULL;
		pMemDriver = GetGDALDriverManager()->GetDriverByName("MEM");
		if (pMemDriver == NULL) { return false; }

		GDALDataset * pMemDataSet = pMemDriver->Create("", nCols, nRows, 3, GDT_Byte, NULL);
		GDALRasterBand *pBand = NULL;
		int nLineCount = nCols * 3;
		int pBandMap[3] = { 1,2,3 };
		CPLErr err = pMemDataSet->RasterIO(GF_Write, 0, 0, nCols, nRows, pMem, nCols, nRows, GDT_Byte, 3, pBandMap, nBands * 1, nLineCount, 1);
		if (err == CE_Failure) return false;
		GDALDriver *pDstDriver = NULL;
		pDstDriver = (GDALDriver *)GDALGetDriverByName(GType);
		if (pDstDriver == NULL) { return false; }
		pMemDataSet->SetGeoTransform(geo);
		pMemDataSet->SetProjection(strWTK);
		pDstDriver->CreateCopy(strSavePath, pMemDataSet, FALSE, NULL, NULL, NULL);
		GDALClose(pMemDataSet);
		return true;
	}
}
/************************************************************************/
/* 影像纠正                                                                     */
/************************************************************************/
void imageRectify(CString strOri, CString strRes,  cameraInfo posDown, cameraInfo dposS2D, double resolutin, double z)
{
	GDALDataset *pDataset = (GDALDataset*)GDALOpen(strOri, GA_ReadOnly);
	if (!pDataset)
	{
		errMsg(eIOpen);
		return;
	}
	int nBands = pDataset->GetRasterCount();
	int nCols = pDataset->GetRasterXSize();
	int nRows = pDataset->GetRasterYSize();
	int nBits = GDALGetDataTypeSize(pDataset->GetRasterBand(1)->GetRasterDataType()) / 8;
	int *pMap = new int[nBands];
	for (int i = 0; i < nBands; i++)
	{
		pMap[i] = i + 1;
	}
	BYTE*data = new BYTE[nBands*nCols*nRows];
	memset(data, 0, nBands*nCols*nRows);
	if (GDT_ERROR == pDataset->RasterIO(GF_Read, 0, 0, nCols, nRows, data, nCols, nRows, GDT_Byte, nBands, pMap, nBands*nBits, nBands*nBits*nCols, nBits))
	{
		errMsg(eIRead);
		delete[]data;
		delete[]pMap;
		GDALClose(pDataset);
		return;
	}
	posDown.CalRotMatrixByPOK();
	cameraInfo posSide = posDown + dposS2D;
	/************************************************************************/
	/* 验证纠正与原始之间的互转                                                                     */
	/************************************************************************/
	Point2D pt1 = imgPtOri2Rec(Point2D(0, 0), nCols, nRows, posSide, resolutin, z);
	Point2D pt2 = imgPtRec2Ori(pt1, nCols, nRows, posSide, resolutin, z);
	/****************************************************************************/

 	Point3D p1 = Img2XYZ(nCols, nRows, Point2D(0, 0), z, posSide);
 	Point3D p2 = Img2XYZ(nCols, nRows, Point2D(nCols, nRows), z, posSide);
 	Point3D p3 = Img2XYZ(nCols, nRows, Point2D(nCols, 0), z, posSide);
 	Point3D p4 = Img2XYZ(nCols, nRows, Point2D(0, nRows), z, posSide);
 
	double max_x = max(max(p1.X, p2.X), max(p3.X, p4.X));
	double min_x = min(min(p1.X, p2.X), min(p3.X, p4.X));
	double max_y = max(max(p1.Y, p2.Y), max(p3.Y, p4.Y));
	double min_y = min(min(p1.Y, p2.Y), min(p3.Y, p4.Y));
	double xlength = max_x - min_x;
	double ylength = max_y - min_y;
	int nCols2 = xlength / resolutin;
	int nRows2 = ylength / resolutin;
	int nBands2 = max(nBands, 3);
	BYTE * data2 = new BYTE[nBands2*nCols2*nRows2];
	memset(data2, 0, nBands2*nCols2*nRows2);
	for (int i = 0; i<nRows2; i++)
	{
		for (int j = 0; j<nCols2; j++)
		{
//			Point3D ptGeo = Img2XYZ(nCols2, nRows2, Point2D(j, i), z, posDown);
			Point3D ptGeo = Point3D(min_x + j*resolutin, min_y + i*resolutin, z);
			Point2D ptImg2Side = XYZ2Img(nCols, nRows, ptGeo, posSide);
			if (!(ptImg2Side.x > 0 && ptImg2Side.x < nCols&&ptImg2Side.y>0 && ptImg2Side.y < nRows))
			{
				continue;
			}
			int r, g, b;
			getImgGray(data, nBands, nCols, nRows, ptImg2Side, r, g, b);
			if (nBands2 == 1)
			{
				*(data2 + i*nCols2 + j) = (r + g + b) / 3;
			}else
				if (nBands2 == 3)
				{
					*(data2 + i*nCols2*nBands2 + j*nBands2 + 0) = r;
					*(data2 + i*nCols2*nBands2 + j*nBands2 + 1) = g;
					*(data2 + i*nCols2*nBands2 + j*nBands2 + 2) = b;
				}
		}
	}
	if (!SaveImg(strRes, nCols2, nRows2, nBands2, data2, 0,"0","JPEG"))
	{
		errMsg(eISave);
	}
	else
	{
		CString strSaveTfwPath = FunGetFileFolder(strRes) + "\\" + FunGetFileName(strRes, false) + ".tfw";
		FILE* fp_tfw = fopen(strSaveTfwPath, "w");
		if (fp_tfw)
		{
			fprintf(fp_tfw, "%lf\n", resolutin);	//写入：分辨率
			fprintf(fp_tfw, "%lf\n", 0.0);			//写入：0.0	
			fprintf(fp_tfw, "%lf\n", 0.0);			//写入：0.0
			fprintf(fp_tfw, "%lf\n", -resolutin);	//写入：分辨率的负数
			fprintf(fp_tfw, "%lf\n", min_x);			//写入：正射影像左上角X坐标
			fprintf(fp_tfw, "%lf\n", min_y + (nRows2 - 1)*resolutin);	//写入：正射影像左上角Y坐标
			fclose(fp_tfw);
		}
		printf("%s image saving successful!\n",strRes);
	}
	delete[]data;
	delete[]pMap;
	GDALClose(pDataset);
	delete[]data2;
	return;
}
void imageRectify(CString strOri, CString strRes, cameraInfo pos, double resolution, double z)
{
	GDALDataset *pDataset = (GDALDataset*)GDALOpen(strOri, GA_ReadOnly);
	if (!pDataset)
	{
		errMsg(eIOpen);
		return;
	}
	int nBands = pDataset->GetRasterCount();
	int nCols = pDataset->GetRasterXSize();
	int nRows = pDataset->GetRasterYSize();
	int nBits = GDALGetDataTypeSize(pDataset->GetRasterBand(1)->GetRasterDataType()) / 8;
	int *pMap = new int[nBands];
	for (int i = 0; i < nBands; i++)
	{
		pMap[i] = i + 1;
	}
	BYTE*data = new BYTE[nBands*nCols*nRows];
	memset(data, 0, nBands*nCols*nRows);
	if (GDT_ERROR == pDataset->RasterIO(GF_Read, 0, 0, nCols, nRows, data, nCols, nRows, GDT_Byte, nBands, pMap, nBands*nBits, nBands*nBits*nCols, nBits))
	{
		errMsg(eIRead);
		delete[]data;
		delete[]pMap;
		GDALClose(pDataset);
		return;
	}
	cameraInfo posSide = pos;
	Point3D p1 = Img2XYZ(nCols, nRows, Point2D(0, 0), z, posSide);
	Point3D p2 = Img2XYZ(nCols, nRows, Point2D(nCols, nRows), z, posSide);
	Point3D p3 = Img2XYZ(nCols, nRows, Point2D(nCols, 0), z, posSide);
	Point3D p4 = Img2XYZ(nCols, nRows, Point2D(0, nRows), z, posSide);


	double max_x = max(max(p1.X, p2.X), max(p3.X, p4.X));
	double min_x = min(min(p1.X, p2.X), min(p3.X, p4.X));
	double max_y = max(max(p1.Y, p2.Y), max(p3.Y, p4.Y));
	double min_y = min(min(p1.Y, p2.Y), min(p3.Y, p4.Y));
	double xlength = max_x - min_x;
	double ylength = max_y - min_y;
	int nCols2 = xlength / resolution;
	int nRows2 = ylength / resolution;
	int nBands2 = max(nBands, 3);
	BYTE * data2 = new BYTE[nBands2*nCols2*nRows2];
	memset(data2, 0, nBands2*nCols2*nRows2);
	for (int i = 0; i < nRows2; i++)
	{
		for (int j = 0; j < nCols2; j++)
		{
			Point3D ptGeo = Point3D(min_x + j*resolution, min_y + i*resolution, z);
			Point2D ptImg2Side = XYZ2Img(nCols, nRows, ptGeo, posSide);
			if (!(ptImg2Side.x > 0 && ptImg2Side.x < nCols&&ptImg2Side.y>0 && ptImg2Side.y < nRows))
			{
				continue;
			}
			ptImg2Side.x = nCols- ptImg2Side.x;
			int r, g, b;
			getImgGray(data, nBands, nCols, nRows, ptImg2Side, r, g, b);
			if (nBands2 == 1)
			{
				*(data2 + i*nCols2 + j) = (r + g + b) / 3;
			}
			else
				if (nBands2 == 3)
				{
					*(data2 + i*nCols2*nBands2 + j*nBands2 + 0) = r;
					*(data2 + i*nCols2*nBands2 + j*nBands2 + 1) = g;
					*(data2 + i*nCols2*nBands2 + j*nBands2 + 2) = b;
				}
		}
	}
	double geo[6] = { 0 };
	//geo[0] = min_x;
	//geo[1] = resolution;
	//geo[2] = 0;
	//geo[3] = min_y + (nRows2 - 1)*resolution;
	//geo[4] = 0;
	//geo[5] = -resolution;
	if (!SaveImg(strRes, nCols2, nRows2, nBands2, data2,geo,"0", "JPEG"))
	{
		errMsg(eISave);
	}
	else
	{
		CString strSaveTfwPath = FunGetFileFolder(strRes) + "\\" + FunGetFileName(strRes, false) + ".tfw";
		FILE* fp_tfw = fopen(strSaveTfwPath, "w");
		if (fp_tfw)
		{
			fprintf(fp_tfw, "%lf\n", resolution);	//写入：分辨率
			fprintf(fp_tfw, "%lf\n", 0.0);			//写入：0.0	
			fprintf(fp_tfw, "%lf\n", 0.0);			//写入：0.0
			fprintf(fp_tfw, "%lf\n", -resolution);	//写入：分辨率的负数
			fprintf(fp_tfw, "%lf\n", min_x);			//写入：正射影像左上角X坐标
			fprintf(fp_tfw, "%lf\n", min_y + (nRows2 - 1)*resolution);	//写入：正射影像左上角Y坐标
			fclose(fp_tfw);
		}
		printf("%s image saving successful!\n", strRes);
	}
	//	iamgeRectifyInv(strRes + ".jpg", data2, nBands, nCols, nRows, posSide, posDown, z);
	delete[]data;
	delete[]pMap;
	GDALClose(pDataset);
	delete[]data2;
	return;
}

// double* calHpMatrix(int nCols, int nRows, cameraInfo posSide, double z)
// {
// 	double Kside[9] = { posSide.f,0,nCols / 2,0,posSide.f,nRows / 2,0,0,1 };
// 	double Rsidecv[9], RTside[9], Rmt[9] = { 1,0,0,0,-1,0,0,0,-1 };
// 	CMatrix mt;
// 	mt.MatrixTranspose(posSide.R, RTside, 3, 3);
// 	mt.MatrixMulti(Rmt, RTside, Rsidecv, 3, 3, 3);
// 	double ICside[3 * 4] = { 1,0,0,-posSide.Xs,0,1,0,-posSide.Ys,0,0,1,-posSide.Zs };
// 	double Pside[3 * 4], mtTmp[9], mtTmp2[9];
// 	mt.MatrixMulti(Kside, Rsidecv, mtTmp, 3, 3, 3);
// 	mt.MatrixMulti(mtTmp, ICside, Pside, 3, 3, 4);//投影矩阵Pside
// 	
// 	double Hpside[9], t[3], C[3] = { posSide.Xs, posSide.Ys, posSide.Zs };
// 	mt.MatrixMulti(Rsidecv, C, t, 3, 3, 1); 
// 	for (int i = 0; i < 3; i++) t[i] *= -1;
// 	mtTmp[0] = 0; mtTmp[1] = 0; mtTmp[2] = z;
// 	mtTmp[3] = 0; mtTmp[4] = 0; mtTmp[5] = z;
// 	mtTmp[6] = 0; mtTmp[7] = 0; mtTmp[8] = z;
// 	mt.MatrixMulti(Rsidecv, mtTmp, mtTmp2, 3, 3, 3);
// 	mtTmp[0] = Rsidecv[0]; mtTmp[1] = Rsidecv[1]; mtTmp[2] = t[0];
// 	mtTmp[3] = Rsidecv[3]; mtTmp[4] = Rsidecv[4]; mtTmp[5] = t[1];
// 	mtTmp[6] = Rsidecv[6]; mtTmp[7] = Rsidecv[7]; mtTmp[8] = t[2];
// 	mt.MatrixAdd(mtTmp2, mtTmp, 3, 3); return mtTmp2;
// 	mt.MatrixMulti(Kside, mtTmp2, Hpside, 3, 3, 3);
// 	return Hpside;
// }
// void imageRectifyCV(CString strOri, CString strRes, cameraInfo posDown, cameraInfo posS2D, double z)
// {
// 	GDALDataset *pDataset = (GDALDataset*)GDALOpen(strOri, GA_ReadOnly);
// 	if (!pDataset)
// 	{
// 		errMsg(eIOpen);
// 		return;
// 	}
// 	int nBands = pDataset->GetRasterCount();
// 	int nCols = pDataset->GetRasterXSize();
// 	int nRows = pDataset->GetRasterYSize();
// 	int nBits = GDALGetDataTypeSize(pDataset->GetRasterBand(1)->GetRasterDataType()) / 8;
// 	int *pMap = new int[nBands];
// 	for (int i = 0; i < nBands; i++)
// 	{
// 		pMap[i] = i + 1;
// 	}
// 	BYTE*data = new BYTE[nBands*nCols*nRows];
// 	memset(data, 0, nBands*nCols*nRows);
// 	if (GDT_ERROR == pDataset->RasterIO(GF_Read, 0, 0, nCols, nRows, data, nCols, nRows, GDT_Byte, nBands, pMap, nBands*nBits, nBands*nBits*nCols, nBits))
// 	{
// 		errMsg(eIRead);
// 		delete[]data;
// 		delete[]pMap;
// 		GDALClose(pDataset);
// 		return;
// 	}
// 	posDown.CalRotMatrixByPOK();
// 	cameraInfo posSide = posDown + posS2D;
// 	double *Hpside = calHpMatrix(nCols, nRows, posSide, z);  //投影矩阵
// 	double XYZ[3], xy[3] = { 0,0,1 };
// 	CMatrix mt; double HpsideInv[9];
// 	mt.MatrixInversion_General(Hpside, 3);
// 	mt.MatrixMulti(Hpside, xy, XYZ, 3, 3, 1);
// 	Point3D ptGeo = Img2XYZ(nCols, nRows, Point2D(0,0), z, posSide);
// 	Point2D pt = XYZ2Img(nCols, nRows, Point3D(XYZ[0], XYZ[1], XYZ[2]), posSide);
// 
// }

void matchPointRectify(CString strModel)
{
	FILE *fR = fopen(strModel, "r");
	CString str1, str2; int ptNum = 0;
	char line[1024]; memset(line, 0, 1024);
	fgets(line, 1024, fR); line[strlen(line) - 1] = '\0';
	str1 = FunStrTok(line, "= ")[1];
	fgets(line, 1024, fR); line[strlen(line) - 1] = '\0';
	str2 = FunStrTok(line, "= ")[1];
	str1 = "E:\\2_Learning\\4_Code\\ImageRectify\\ImageRectify\\016BR021.jpg";  //原始影像
	str2 = "E:\\2_Learning\\4_Code\\ImageRectify\\ImageRectify\\016BR022.jpg";  //原始影像
	GDALDataset *pDataset = (GDALDataset*)GDALOpen(str1, GA_ReadOnly);
	if (!pDataset)
	{
		errMsg(eIOpen);
		return;
	}
	int nBands1 = pDataset->GetRasterCount();
	int nCols1 = pDataset->GetRasterXSize();
	int nRows1 = pDataset->GetRasterYSize();
	GDALClose(pDataset);
	pDataset = (GDALDataset*)GDALOpen(str2, GA_ReadOnly);
	if (!pDataset)
	{
		errMsg(eIOpen);
		return;
	}
	int nBands2 = pDataset->GetRasterCount();
	int nCols2 = pDataset->GetRasterXSize();
	int nRows2 = pDataset->GetRasterYSize();
	GDALClose(pDataset);


	fgets(line, 1024, fR); line[strlen(line) - 1] = '\0';
	ptNum = atoi(line);

	CString strTask1 = "016BR021task.txt";
	CString strTask2 = "016BR022task.txt";
	CString strOri, strRes1, strRes2;
	double meanZ = 0, resolution = 0.1;
	cameraInfo posDown1, dposS2D1, posDown2, dposS2D2, posImg1, posImg2;
	if (!readTask(strTask1, strOri, strRes1, posDown1, dposS2D1, resolution, meanZ)) return;
	if (resolution <= 0)
	{
		printf("resolution in task file error(<=0), reset resolution=0.1\n");
		resolution = 0.1;
	}
	posDown1.CalRotMatrixByPOK();
	posImg1 = posDown1 + dposS2D1;
	if (!readTask(strTask2, strOri, strRes2, posDown2, dposS2D2, resolution, meanZ)) return;
	if (resolution <= 0)
	{
		printf("resolution in task file error(<=0), reset resolution=0.1\n");
		resolution = 0.1;
	}
	posDown2.CalRotMatrixByPOK();
	posImg2 = posDown2 + dposS2D2;
// 	imageRectify(str1, str1+"rec.jpg", posDown1, dposS2D1, resolution, meanZ);
// 	imageRectify(str2, str2 + "rec.jpg", posDown1, dposS2D1, resolution, meanZ);
//	vector<Point2D> vptl, vptr;
	FILE* fw = fopen(strModel + "_rec.txt", "w");
	fprintf(fw, "leftimage= %s\n", str1 /*+ "rec.jpg"*/);
	fprintf(fw, "rightimage= %s\n", str2 /*+ "rec.jpg"*/);
	fprintf(fw, "%d\n", ptNum);
	Point2D pl, pr, pl2,pr2;
	for (int i = 0; i<ptNum; i++)
	{
		fgets(line, 1024, fR); line[strlen(line) - 1] = '\0';
		vector<CString> vToks;
		vToks = FunStrTok(line, " \t");
		pl.x = atof(vToks[1]);
		pl.y = atof(vToks[2]);
		pr.x = atof(vToks[3]);
		pr.y = atof(vToks[4]);
		pl2 = imgPtRec2Ori(pl, nCols1, nRows1, posImg1, resolution, meanZ);
		pr2 = imgPtRec2Ori(pr, nCols2, nRows2, posImg2, resolution, meanZ);
		fprintf(fw, "%s %10.2lf %10.2lf %10.2lf %10.2lf %s %d\n", vToks[0], pl2.x, pl2.y, pr2.x, pr2.y, vToks[0], 0);
	}
	fclose(fw);
	fclose(fR);
}

void imageMosic(vector<CString> &vecImgPath, vector<cameraInfo>& vecPosInfo, float fscale)
{
}


bool ReadCameraFile(CString strCameraFile, vector<cameraInfo>&vecCmr)
{
	FILE *pfR = fopen(strCameraFile, "r");
	if (!pfR)return false;
	char line[2048]; memset(line, 0, 2048);
	while (fgets(line,2048,pfR))
	{
		cameraInfo cmr;
		vector<CString>vvvv;
		vvvv = FunStrTok(line, " \t");
		cmr.strLabel = vvvv[0];
		cmr.Xs = atof(vvvv[1]);
		cmr.Ys = atof(vvvv[2]);
		cmr.Zs = atof(vvvv[3]);
		cmr.phi = atof(vvvv[4]);
		cmr.omg = atof(vvvv[5]);
		cmr.kap = atof(vvvv[6]);
		vecCmr.push_back(cmr);
	}
	fclose(pfR);
	return true;
}

cameraInfo calCameraInfo()
{
	cameraInfo cmr;
	cmr.phi = 0;
	cmr.omg = -0.5;
	cmr.kap = 0;
	
	cmr.f = 161.6; //单位pix
	cmr.Xs = 0;
	cmr.Ys = 0;
	cmr.Zs = 0.4;
	cmr.CalRotMatrixByPOK();
	return cmr;
}
void main(int argc, char **argv)   //几何精纠正
{
	//if (argc != 4)
	//{
	//	cout << "parameter error: exe inFolder inPosFile outFolder" << endl;
	//	return;
	//}
	GDALAllRegister();         //利用GDAL读取图片，先要进行注册  
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");   //设置支持中文路径
	
	CString strInput = "E:/2_Learning/4_Code/ImageRectify/ImageRectify/IMG_4378.JPG";
	CString strOutput = "E:/2_Learning/4_Code/ImageRectify/ImageRectify/IMG_4378_rec.JPG";
	cameraInfo cmr = calCameraInfo();
	imageRectify(strInput, strOutput, cmr, 0.01, 0);

}

void main0(int argc, char **argv)   //几何精纠正
{
	if (argc != 4)
	{
		cout << "parameter error: exe inFolder inPosFile outFolder" << endl;
		return;
	}
	GDALAllRegister();         //利用GDAL读取图片，先要进行注册  
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");   //设置支持中文路径
														 //	imageRectify(strOri, strRes, posDown, dposS2D, resolution, meanZ);

	vector<CString> vecImagePath;
	vector<cameraInfo> vecCameraInfo;
	CString strOutFolder = argv[3];
	FunSearchFile(argv[1], "jpg", vecImagePath);
	FunSearchFile(argv[1], "tif", vecImagePath);
	if (vecImagePath.size() == 0)
	{
		cout << "image error!" << endl;
		return;
	}
	cout << "image number: " << vecImagePath.size() << endl;
	if (!ReadCameraFile(argv[2], vecCameraInfo))
	{
		cout << "camera error!" << endl;
		return;
	}
	vector<int> vecImageCmrIdx(vecImagePath.size(), -1);
	for (int i = 0; i<vecImagePath.size(); i++)
	{
		for (int j = 0; j<vecCameraInfo.size(); j++)
		{
			if (FunGetFileName(vecImagePath[i], false).MakeLower() ==
				FunGetFileName(vecCameraInfo[j].strLabel, false).MakeLower())
			{
				vecImageCmrIdx[i] = j;
				break;;
			}

		}
	}
#pragma omp parallel for
	for (int i = 0; i<vecImagePath.size(); i++)
	{
		if(vecImageCmrIdx[i]==-1) continue;
		cout << i << "   " + FunGetFileName(vecImagePath[i], true) << endl;
		CString strRes = strOutFolder + "\\" + FunGetFileName(vecImagePath[i], false) + "_ref.jpg";
		imageRectify(vecImagePath[i], strRes, vecCameraInfo[vecImageCmrIdx[i]], 0.2, 0);
	}

}
void main1(int argc, char **argv)
{
//  	if (argc != 2)
//  	{
//  		errMsg(ePara);
//  		return;
//  	}
//  	CString strTask = argv[2];
// 	CString strTask = "016BR021task.txt";
// 	CString strOri, strRes; 
// 	double meanZ = 0,  resolution = 0.1;
// 	cameraInfo posDown, dposS2D;
// 	if (!readTask(strTask, strOri, strRes, posDown, dposS2D, resolution, meanZ)) return;
// 	if (resolution <= 0)
// 	{
// 		printf("resolution in task file error(<=0), reset resolution=0.1\n");
// 		resolution = 0.1;
// 
// 	}
	GDALAllRegister();         //利用GDAL读取图片，先要进行注册  
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");   //设置支持中文路径
//	imageRectify(strOri, strRes, posDown, dposS2D, resolution, meanZ);

CString strModel = "E:\\2_Learning\\4_Code\\ImageRectify\\ImageRectify\\rec\\model\\016BR021.jpgrec=016BR022.jpgrec.model";
matchPointRectify(strModel);
}

void main2()
{
	int width = 1269;
	int height = 940;
	cameraInfo pos1, pos2;
	pos1.Xs = -12314906.611;
	pos1.Ys = 338349927.83;
	pos1.Zs = 0;
	pos1.phi = 0;
	pos1.omg = 0;
	pos1.kap = 0;
	pos2.Xs = -31182438.16;
	pos2.Ys = 18003189.068;
	pos2.Zs = 0;
	pos2.phi = 0;
	pos2.omg = 0;
	pos2.kap = 0;

	Point3D p1 = Img2XYZ(width, height, Point2D(0, 0),36000000,pos1);
	Point3D p2 = Img2XYZ(width, height, Point2D(width, 0), 36000000, pos1);
	Point3D p3 = Img2XYZ(width, height, Point2D(width, height), 36000000, pos1);
	Point3D p4 = Img2XYZ(width, height, Point2D(0, height), 36000000, pos1);

	double max_x = max(max(p1.X, p2.X), max(p3.X, p4.X));
	double min_x = min(min(p1.X, p2.X), min(p3.X, p4.X));
	double max_y = max(max(p1.Y, p2.Y), max(p3.Y, p4.Y));
	double min_y = min(min(p1.Y, p2.Y), min(p3.Y, p4.Y));
	double xlength = max_x - min_x;
	double ylength = max_y - min_y;
}