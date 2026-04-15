///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ZzzTexture.h"
#include "./Utilities/Log/ErrorReport.h"
#include "WSclient.h"
#include "DSPlaySound.h"
#ifndef __ANDROID__
#include <setjmp.h>
#include <turbojpeg/jpeglib.h>
#else
#include <vector>
#include <turbojpeg/stb_image.h>
#endif
#include "ProtocolSend.h"
#ifdef __ANDROID__
#include <android/log.h>
#endif

CGlobalBitmap Bitmaps;

#ifndef __ANDROID__
struct my_error_mgr
{
	struct jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr* my_error_ptr;

METHODDEF(void) my_error_exit(j_common_ptr cinfo)
{
	my_error_ptr myerr = (my_error_ptr)cinfo->err;
	(*cinfo->err->output_message) (cinfo);
	longjmp(myerr->setjmp_buffer, 1);
}
#endif // !__ANDROID__

bool WriteJpeg(char* filename, int Width, int Height, unsigned char* Buffer, int quality)
{
#ifdef __ANDROID__
	(void)filename;
	(void)Width;
	(void)Height;
	(void)Buffer;
	(void)quality;
	return false;
#else
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE* outfile;
	JSAMPROW row_pointer[1];
	int row_stride;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	if ((outfile = fopen(filename, "wb")) == NULL)
	{
		//fprintf(stderr, "can't open %s\n", filename);
		//exit(1);
		return FALSE;
	}
	jpeg_stdio_dest(&cinfo, outfile);

	cinfo.image_width = Width;
	cinfo.image_height = Height;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE);
	jpeg_start_compress(&cinfo, TRUE);
	row_stride = cinfo.image_width * 3;

	while (cinfo.next_scanline < cinfo.image_height)
	{
		row_pointer[0] = &Buffer[(Height - 1 - cinfo.next_scanline) * row_stride];
		(void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	fclose(outfile);
	jpeg_destroy_compress(&cinfo);
	return TRUE;
#endif
}

void SaveImage(int HeaderSize, char* Ext, char* filename, BYTE* PakBuffer, int Size)
{
	if (PakBuffer == NULL || Size == 0)
	{
		char OpenFileName[256];
		strcpy(OpenFileName, "Data2\\");
		strcat(OpenFileName, filename);
		FILE* fp = fopen(OpenFileName, "rb");
		if (fp == NULL)
		{
			return;
		}
		fseek(fp, 0, SEEK_END);
		Size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		PakBuffer = new unsigned char[Size];
		fread(PakBuffer, 1, Size, fp);
		fclose(fp);
	}

	char Header[24];
	memcpy(Header, PakBuffer, HeaderSize);

	char NewFileName[256];
	int iTextcnt = 0;
	for (int i = 0; i < (int)strlen(filename); i++)
	{
		iTextcnt = i;
		NewFileName[i] = filename[i];
		if (filename[i] == '.') break;
	}
	NewFileName[iTextcnt + 1] = NULL;
	strcat(NewFileName, Ext);
	char SaveFileName[256];
	strcpy(SaveFileName, "Data\\");
	strcat(SaveFileName, NewFileName);
	FILE* fp = fopen(SaveFileName, "wb");
	if (fp == NULL) return;
	fwrite(Header, 1, HeaderSize, fp);
	fwrite(PakBuffer, 1, Size, fp);
	fclose(fp);

	if (PakBuffer == NULL || Size == 0)
	{
		SAFE_DELETE_ARRAY(PakBuffer);
	}
}

bool OpenJpegBuffer(char* filename, float* BufferFloat)
{
#ifdef __ANDROID__
	if (filename == nullptr || BufferFloat == nullptr)
	{
		return false;
	}

	char FileName[256];
	char NewFileName[256];
	int iTextcnt = 0;
	for (int i = 0; i < (int)strlen(filename); i++)
	{
		iTextcnt = i;
		NewFileName[i] = filename[i];
		if (filename[i] == '.') break;
	}
	NewFileName[iTextcnt + 1] = '\0';
	strcpy(FileName, "Data\\");
	strcat(FileName, NewFileName);
	strcat(FileName, "OZJ");

	FILE* infile = fopen(FileName, "rb");
	if (infile == NULL)
	{
		char Text[256];
		sprintf(Text, "%s - File not exist.", FileName);
		g_ErrorReport.Write(Text);
		g_ErrorReport.Write("\r\n");
		MessageBox(gwinhandle->GethWnd(), Text, NULL, MB_OK);
		SendMessage(gwinhandle->GethWnd(), WM_DESTROY, 0, 0);
		return false;
	}

	fseek(infile, 0, SEEK_END);
	long fileSize = ftell(infile);
	if (fileSize <= 24)
	{
		fclose(infile);
		return false;
	}

	fseek(infile, 24, SEEK_SET);
	std::vector<BYTE> encoded((size_t)(fileSize - 24));
	if (fread(encoded.data(), 1, encoded.size(), infile) != encoded.size())
	{
		fclose(infile);
		return false;
	}
	fclose(infile);

	int outputWidth = 0;
	int outputHeight = 0;
	int outputComponents = 0;
	unsigned char* decoded = stbi_load_from_memory(
		encoded.data(),
		static_cast<int>(encoded.size()),
		&outputWidth,
		&outputHeight,
		&outputComponents,
		3);

	if (decoded == nullptr)
	{
		return false;
	}

	const size_t rowStride = static_cast<size_t>(outputWidth) * 3;
	size_t index = 0;
	for (int y = 0; y < outputHeight; ++y)
	{
		const unsigned char* row = decoded + (outputHeight - 1 - y) * rowStride;
		for (int x = 0; x < outputWidth; ++x)
		{
			BufferFloat[index + 0] = static_cast<float>(row[x * 3 + 0]) / 255.f;
			BufferFloat[index + 1] = static_cast<float>(row[x * 3 + 1]) / 255.f;
			BufferFloat[index + 2] = static_cast<float>(row[x * 3 + 2]) / 255.f;
			index += 3;
		}
	}

	stbi_image_free(decoded);
	return true;
#else
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;
	FILE* infile;
	JSAMPARRAY buffer;
	int row_stride;

	char FileName[256];

	char NewFileName[256];
	int iTextcnt = 0;
	for (int i = 0; i < (int)strlen(filename); i++)
	{
		iTextcnt = i;
		NewFileName[i] = filename[i];
		if (filename[i] == '.') break;
	}
	NewFileName[iTextcnt + 1] = NULL;
	strcpy(FileName, "Data\\");
	strcat(FileName, NewFileName);
	strcat(FileName, "OZJ");

	if ((infile = fopen(FileName, "rb")) == NULL)
	{
		char Text[256];
		sprintf(Text, "%s - File not exist.", FileName);
		g_ErrorReport.Write(Text);
		g_ErrorReport.Write("\r\n");
		MessageBox(gwinhandle->GethWnd(), Text, NULL, MB_OK);
		SendMessage(gwinhandle->GethWnd(), WM_DESTROY, 0, 0);
		return false;
	}

	fseek(infile, 24, SEEK_SET);

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	if (setjmp(jerr.setjmp_buffer))
	{
		jpeg_destroy_decompress(&cinfo);
		fclose(infile);
		return false;
	}
	jpeg_create_decompress(&cinfo);

	jpeg_stdio_src(&cinfo, infile);

	(void)jpeg_read_header(&cinfo, TRUE);

	(void)jpeg_start_decompress(&cinfo);
	row_stride = cinfo.output_width * cinfo.output_components;
	buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

	size_t buff_size = (cinfo.output_width * cinfo.output_height * cinfo.output_components);

	unsigned char* Buffer = new BYTE[buff_size];

	while (cinfo.output_scanline < cinfo.output_height)
	{
		(void)jpeg_read_scanlines(&cinfo, buffer, 1);
		memcpy(Buffer + (cinfo.output_height - cinfo.output_scanline) * row_stride, buffer[0], row_stride);
	}

	UINT Index = 0;

	if (Buffer != NULL)
	{
		for (unsigned int y = 0; y < cinfo.output_height; y++)
		{
			for (unsigned int x = 0; x < cinfo.output_width; x++)
			{
				if ((Index + 2) < buff_size)
				{
					BufferFloat[Index + 0] = (float)Buffer[Index + 0] / 255.f;
					BufferFloat[Index + 1] = (float)Buffer[Index + 1] / 255.f;
					BufferFloat[Index + 2] = (float)Buffer[Index + 2] / 255.f;
				}
				Index += 3;
			}
		}
	}
	SAFE_DELETE_ARRAY(Buffer);

	(void)jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(infile);
	return true;
#endif
}

bool LoadBitmap(const char* szFileName, GLuint uiTextureIndex, GLuint uiFilter, GLuint uiWrapMode, bool bCheck, bool bFullPath)
{
	char szFullPath[256] = { 0, };

	if (bFullPath == true)
	{
		strcpy(szFullPath, szFileName);
	}
	else
	{
		strcpy(szFullPath, "Data\\");
		strcat(szFullPath, szFileName);
	}

	if (bCheck)
	{
		if (false == Bitmaps.LoadImage(uiTextureIndex, szFullPath, uiFilter, uiWrapMode))
		{
#ifdef __ANDROID__
			__android_log_print(ANDROID_LOG_ERROR,
				"MUTexture",
				"LoadBitmap failed idx=%u path=%s filter=0x%x wrap=0x%x",
				uiTextureIndex,
				szFullPath,
				uiFilter,
				uiWrapMode);
#endif
			char szErrorMsg[256] = { 0, };
			sprintf(szErrorMsg, "LoadBitmap Failed: %s", szFullPath);
			PopUpErrorCheckMsgBox(szErrorMsg, true);
			return false;
		}
		return true;
	}
	return Bitmaps.LoadImage(uiTextureIndex, szFullPath, uiFilter, uiWrapMode);
}

void DeleteBitmap(GLuint uiTextureIndex, bool bForce)
{
	Bitmaps.UnloadImage(uiTextureIndex, bForce);
}

void PopUpErrorCheckMsgBox(const char* szErrorMsg, bool bForceDestroy)
{
	char szMsg[1024] = { 0, };
	strcpy(szMsg, szErrorMsg);

	if (bForceDestroy)
	{
		MessageBox(gwinhandle->GethWnd(), szErrorMsg, "ErrorCheckBox", MB_OK | MB_ICONERROR);
	}
	else
	{
		int iResult = MessageBox(gwinhandle->GethWnd(), szMsg, "ErrorCheckBox", MB_YESNO | MB_ICONERROR);

		if (IDYES == iResult)
		{
			return;
		}
	}

	//SocketClient.Close();
	//KillGLWindow();
	//DestroySound();
	//DestroyWindow();
	//CloseMainExe();
	//ExitProcess(0);
}
