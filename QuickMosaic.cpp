#include "QuickMosaic.h"

#include "GL/freeglut.h"

#include "GL/glew.h"

#include "GL/vmath.h"

#pragma comment(lib, "free_glut.lib")

#pragma comment(lib, "glew32.lib")

char orthovert[] =
{
	"#version 400										\n"
	"layout(location = 0) in vec4 in_ver_position;		\n"
	"layout(location = 1) in vec2 in_tex_position;		\n"
	"uniform mat4 mvmatrix;								\n"
	"out vec2  vs_tex_coord;							\n"
	"void main(void)									\n"
	"{													\n"
	"	gl_Position = mvmatrix * in_ver_position;		\n"
	"	vs_tex_coord = in_tex_position;					\n"
	"}													\n"
};

char orthofrag[] =
{
	"#version 400										\n"
	"uniform sampler2D tex;								\n"
	"in vec2 vs_tex_coord;								\n"
	"layout(location = 0) out vec4 color;				\n"
	"void main(void)									\n"
	"{													\n"
	"	color = texture(tex, vs_tex_coord);				\n"
	"}													\n"
};

GLuint  LoadShaders(const char* strvert, const char* strfrag)
{
	GLuint v = glCreateShader(GL_VERTEX_SHADER);
	GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(v, 1, &strvert, NULL);
	glShaderSource(f, 1, &strfrag, NULL);
	GLint success;
	glCompileShader(v);
	glGetShaderiv(v, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char info_log[4096];
		glGetShaderInfoLog(v, 4096, NULL, info_log);
		printf("%s\n", info_log);
		//AfxMessageBox(info_log);
		return 0;
	}
	glCompileShader(f);
	glGetShaderiv(f, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char info_log[4096];
		glGetShaderInfoLog(f, 4096, NULL, info_log);
		//AfxMessageBox(info_log);
		printf("%s\n", info_log);
		return 0;
	}
	GLuint prg = glCreateProgram();
	glAttachShader(prg, v);
	glAttachShader(prg, f);
	glLinkProgram(prg);
	return prg;
}



void QuickProjection(const vector<string>& strSrcFileNames, const vector<string>& strDstFileNames, OGRPoint* pFootPoint, double lfGSD, ofstream* pLog = NULL, GDALProgressFunc pProg = NULL, void* pProgPara = NULL)
{

	GDALAllRegister();

	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");

	GDALDriver* pTifDriver = GetGDALDriverManager()->GetDriverByName("GTiff");

	if (pTifDriver == NULL)
	{
		if (pLog)
		{
			(*pLog) << _T("获取GDAL的GTiff驱动器失败.") << endl;
		}
		return;
	}

	int nFileCount = strSrcFileNames.size();

	if (nFileCount <= 0)
	{
		if (pLog)
		{
			(*pLog) << _T("影像数量为0.\n") << endl;
		}
		return;
	}

	pProg(0.0, _T("初始化..."), pProgPara);


	/************************************************************************/
	/*				 初始化GLUT,实际上只借助了GLUT的窗口生成FBO               */
	/************************************************************************/
	int argc = 0;
	char* argv[] = { NULL };

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(512, 512);
	glutCreateWindow("Ortho");
	if (glewInit())
	{
		if (pLog)
		{
			(*pLog) << _T("GLEW初始化失败.\n") << endl;
		}
		return;
	}


	/************************************************************************/
	/*		            VBO,VAO,FBO,SHADER的初始化                          */
	/************************************************************************/
	GLuint  texvboid = 0;
	glGenBuffers(1, &texvboid);
	glBindBuffer(GL_ARRAY_BUFFER, texvboid);
	glBufferData(GL_ARRAY_BUFFER, 96, NULL, GL_DYNAMIC_DRAW);
	GLuint vaoid;
	glGenVertexArrays(1, &vaoid);
	GLuint uiFBID = 0;
	glGenFramebuffersEXT(1, &uiFBID);
	glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, uiFBID);
	GLuint program = LoadShaders(orthovert, orthofrag);
	glUseProgram(program);
	GLuint matrixUniformID = glGetUniformLocation(program, "mvmatrix");

	const int MAXWIDTH = 8192;
	int nBandMap[3] = { 1, 2, 3 };

	BYTE* pData     = new BYTE[MAXWIDTH * MAXWIDTH * 3];
	BYTE* pDataCopy = new BYTE[MAXWIDTH * MAXWIDTH * 3];

	GDALDataset* pDataset = NULL;

	int nCols = 0, nRows = 0;

	for (int i = 0; i < nFileCount; ++i)
	{
		pDataset = (GDALDataset*)GDALOpen(strSrcFileNames[i].c_str(), GA_ReadOnly);

		if (pDataset == NULL)
		{
			if (pLog) (*pLog) << _T("打开文件：") << strSrcFileNames[i] << _T("失败.") << endl;
			continue;
		}

		nCols = pDataset->GetRasterXSize();
		nRows = pDataset->GetRasterYSize();

		memset(pData, 0x00, MAXWIDTH * MAXWIDTH * 3);
		memset(pDataCopy, 0x00, MAXWIDTH * MAXWIDTH * 3);

		pDataset->RasterIO(GF_Read, 0, 0, nCols, nRows, pData, nCols, nRows, GDT_Byte, 3, nBandMap, 3, 3 * nCols, 1);

		for (int j = 0; j < nRows; ++j)
		{
			memcpy(pDataCopy + (nRows - j - 1) * 3 * nCols, pData + j * 3 * nCols, 3 * nCols); //倒置数据
		}

		GLuint uiTempTexID;
		glGenTextures(1, &uiTempTexID); //原始影像制作成纹理
		glBindTexture(GL_TEXTURE_2D, uiTempTexID);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexStorage2D(GL_TEXTURE_2D, 0, GL_UNSIGNED_BYTE, nCols, nRows);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, nCols, nRows, 0, GL_RGB, GL_UNSIGNED_BYTE, pDataCopy);

		OGRPoint FootPoints[4];
		FootPoints[0] = pFootPoint[i * 4 + 0];
		FootPoints[1] = pFootPoint[i * 4 + 1];
		FootPoints[2] = pFootPoint[i * 4 + 2];
		FootPoints[3] = pFootPoint[i * 4 + 3];

		double lfMinX = min(min(FootPoints[0].getX(), FootPoints[1].getX()), min(FootPoints[1].getX(), FootPoints[2].getX()));
		double lfMaxX = max(max(FootPoints[0].getX(), FootPoints[1].getX()), max(FootPoints[1].getX(), FootPoints[2].getX()));
		double lfMinY = min(min(FootPoints[0].getY(), FootPoints[1].getY()), min(FootPoints[1].getY(), FootPoints[2].getY()));
		double lfMaxY = max(max(FootPoints[0].getY(), FootPoints[1].getY()), max(FootPoints[1].getY(), FootPoints[2].getY()));

		double lfWid = lfMaxX - lfMinX;
		double lfHei = lfMaxY - lfMinY;
		int nOrthoCols = ceil(lfWid / lfGSD);
		int nOrthoRows = ceil(lfHei / lfGSD);

		if (nOrthoCols > nCols || nOrthoRows > nRows)
		{
			if (pLog) (*pLog) << _T("设置的地面分辨率过高.") << endl;
			continue;
		}

		GLuint uiFboTBID = 0;
		glGenTextures(1, &uiFboTBID);
		glBindTexture(GL_TEXTURE_2D, uiFboTBID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, nOrthoCols, nOrthoRows, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
		glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, uiFboTBID, 0);

		glClearColor(0.0, 0.0, 0.0, 0.0); //在FBO中绘制
		glViewport(0, 0, nOrthoCols, nOrthoRows);
		vmath::mat4 translatematrix;
		vmath::mat4 scalematrix;
		vmath::mat4 transformmatrix;
		translatematrix = vmath::translate<float>(-(lfMinX + lfMaxX) / 2.0, -(lfMinY + lfMaxY) / 2.0, 0.0);  //这个地方并不严谨,理论上应该给相机的投影矩阵
		scalematrix = vmath::scale<float>(2.0 / (lfMaxX - lfMinX), 2.0 / (lfMaxY - lfMinY), 0.0);
		transformmatrix = scalematrix * translatematrix;
		glUniformMatrix4fv(matrixUniformID, 1, GL_FALSE, transformmatrix);

		float texcoord[24] = { lfMinX, lfMaxY, 0.0, 1.0f,
			lfMinX, lfMinY, 0.0, 1.0f,
			lfMaxX, lfMinY, 0.0, 1.0f,
			lfMaxX, lfMaxY, 0.0, 1.0f,
			0.0, 1.0,
			0.0, 0.0,
			1.0, 0.0,
			1.0, 1.0 };

		glBindTexture(GL_TEXTURE_2D, uiTempTexID);
		glBufferData(GL_ARRAY_BUFFER, 96, texcoord, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)64);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		glBindTexture(GL_TEXTURE_2D, uiFboTBID); //从FBO中读取数据
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pData);

		for (int j = 0; j < nOrthoRows; ++j)
		{
			memcpy(pDataCopy + (nOrthoRows - j - 1) * 3 * nOrthoCols, pData + j * 3 * nOrthoCols, 3 * nOrthoCols);
		}

		CPLStringList opList;
		opList.SetNameValue("TFW", "YES");
		GDALDataset* pOrthoDataSet = pTifDriver->Create(strDstFileNames[i].c_str(), nOrthoCols, nOrthoRows, 3, GDT_Byte, opList.List());

		double GeoTrans[6] = { lfMinX, lfGSD, 0.0, lfMaxY, 0.0, -lfGSD };
		pOrthoDataSet->SetGeoTransform(GeoTrans);
		pOrthoDataSet->RasterIO(GF_Write, 0, 0, nOrthoCols, nOrthoRows, pDataCopy, nOrthoCols, nOrthoRows, GDT_Byte, 3, nBandMap, 3, 3 * nOrthoCols, 1);

		/************************************************************************/
		/*							清理本帧所需纹理                             */
		/************************************************************************/
		glDeleteTextures(1, &uiTempTexID);
		glDeleteTextures(1, &uiFboTBID);

		GDALClose(pDataset);
		GDALClose(pOrthoDataSet);

		/************************************************************************/
		/*							     刷新进度                                */
		/************************************************************************/

		pProg((double)i / nFileCount, _T("初始化..."), pProgPara);
	}

	/************************************************************************/
	/*						         清理显存                                */
	/************************************************************************/
	
	glDeleteBuffers(1, &texvboid);

	glDeleteVertexArrays(1, &vaoid);

	glDeleteFramebuffers(1, &uiFBID);
	
	glDeleteShader(program);

	/************************************************************************/
	/*								清理内存                                */
	/************************************************************************/

	delete pData;     pData = NULL;

	delete pDataCopy; pDataCopy = NULL;

	pProg(1.0, _T("完成."), pProgPara);
}


void QuickMosaic(const vector<string>& strSrcFileNames, const string& strDstFileName, ofstream* pLog = NULL, GDALProgressFunc pProg = NULL, void* pProgPara = NULL)
{

}