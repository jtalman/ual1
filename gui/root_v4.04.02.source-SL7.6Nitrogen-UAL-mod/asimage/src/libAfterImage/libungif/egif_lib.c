/******************************************************************************
*   "Gif-Lib" - Yet another gif library.				      *
*									      *
* Written by:  Gershon Elber				Ver 1.1, Aug. 1990    *
*******************************************************************************
* The kernel of the GIF Encoding process can be found here.		      *
*******************************************************************************
* History:								      *
* 14 Jun 89 - Version 1.0 by Gershon Elber.				      *
*  3 Sep 90 - Version 1.1 by Gershon Elber (Support for Gif89, Unique names). *
* 26 Jun 96 - Version 3.0 by Eric S. Raymond (Full GIF89 support)
******************************************************************************/

#ifdef _WIN32
#include "../win32/config.h"
#else
#include "../config.h"
#endif

#ifdef __MSDOS__
#include <alloc.h>
#include <sys\stat.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#ifdef R6000
/* FIXME: What is sys/mode.h?  Can we substitute a check for this file rather
 * than a check based on machine type?
 */
#include <sys/mode.h>
#endif
#endif /* __MSDOS__ */

#include <fcntl.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <stdio.h>
#include <string.h>
#include "gif_lib.h"
#include "gif_lib_private.h"

/* #define DEBUG_NO_PREFIX		          Dump only compressed data. */

/* Masks given codes to BitsPerPixel, to make sure all codes are in range: */
static GifPixelType CodeMask[] = {
    0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff
};

static unsigned char GifVersionPrefix[GIF_STAMP_LEN + 1] = GIF87_STAMP;
static int GifVersionPrefixLen = GIF_STAMP_LEN ;

#define WRITE(_gif,_buf,_len)   \
  (((GifFilePrivateType*)_gif->Private)->Write ?    \
   ((GifFilePrivateType*)_gif->Private)->Write(_gif,(unsigned char *)_buf,(int)_len) :    \
   fwrite(_buf, 1, _len, ((GifFilePrivateType*)_gif->Private)->File))

static int EGifPutWord(int Word, GifFileType *GifFile);
static int EGifSetupCompress(GifFileType *GifFile);
static int EGifCompressLine(GifFileType *GifFile, GifPixelType *Line,
								int LineLen);
static int EGifCompressOutput(GifFileType *GifFile, int Code);
static int EGifBufferedOutput(GifFileType *GifFile, GifByteType *Buf, int c);

/******************************************************************************
*   Update a new gif file, given its file handle, which must be opened for    *
* write in binary mode.							      *
*   Returns GifFileType pointer dynamically allocated which serves as the gif *
* info record. _GifError is cleared if succesfull.			      *
******************************************************************************/
GifFileType *EGifOpenFileHandle(int FileHandle)
{
    GifFileType *GifFile;
    GifFilePrivateType *Private;
    FILE *f;

    if ((GifFile = (GifFileType *) malloc(sizeof(GifFileType))) == NULL) {
        _GifError = E_GIF_ERR_NOT_ENOUGH_MEM;
        return NULL;
    }

    memset(GifFile, '\0', sizeof(GifFileType));

    if ((Private = (GifFilePrivateType *)
                   malloc(sizeof(GifFilePrivateType))) == NULL) {
        free(GifFile);
        _GifError = E_GIF_ERR_NOT_ENOUGH_MEM;
        return NULL;
    }

#ifdef __MSDOS__
    setmode(FileHandle, O_BINARY);      /* Make sure it is in binary mode. */
#endif /* __MSDOS__ */

    f = fdopen(FileHandle, "wb");           /* Make it into a stream: */

#ifdef __MSDOS__
    setvbuf(f, NULL, _IOFBF, GIF_FILE_BUFFER_SIZE);   /* And inc. stream buffer. */
#endif /* __MSDOS__ */

    GifFile->Private = (VoidPtr) Private;
    Private->FileHandle = FileHandle;
    Private->File = f;
    Private->FileState = FILE_STATE_WRITE;
    
    Private->Write = (OutputFunc)0; /* No user write routine (MRB) */
    GifFile->UserData = (VoidPtr)0; /* No user write handle (MRB) */
    
    _GifError = 0;

    return GifFile;
}

/******************************************************************************
* Output constructor that takes user supplied output function.                *
* Basically just a copy of EGifOpenFileHandle. (MRB)                          *
******************************************************************************/
GifFileType* EGifOpen(void* userData, OutputFunc writeFunc)
{
    GifFileType* GifFile;
    GifFilePrivateType* Private;

     if ((GifFile = (GifFileType *) malloc(sizeof(GifFileType))) == NULL) {
        _GifError = E_GIF_ERR_NOT_ENOUGH_MEM;
        return NULL;
    }

    memset(GifFile, '\0', sizeof(GifFileType));

    if ((Private = (GifFilePrivateType *)
                   malloc(sizeof(GifFilePrivateType))) == NULL) {
        free(GifFile);
        _GifError = E_GIF_ERR_NOT_ENOUGH_MEM;
        return NULL;
    }

    GifFile->Private = (VoidPtr) Private;
    Private->FileHandle = 0;
    Private->File = (FILE *)0;
    Private->FileState = FILE_STATE_WRITE;
    
    Private->Write = writeFunc; /* User write routine (MRB) */
    GifFile->UserData = userData; /* User write handle (MRB) */
    
    _GifError = 0;

    return GifFile;
}

/******************************************************************************
*   Routine to set current GIF version. All files open for write will be      *
* using this version until next call to this routine. Version consists of     *
* 3 characters as "87a" or "89a". No test is made to validate the version.    *
******************************************************************************/
void EGifSetGifVersion(const char *Version)
{
    memcpy(GifVersionPrefix + GIF_VERSION_POS, Version, 3);
}

/******************************************************************************
*   This routine should be called before any other EGif calls, immediately    *
* follows the GIF file openning.					      *
******************************************************************************/
int EGifPutScreenDesc(GifFileType *GifFile,
	int Width, int Height, int ColorRes, int BackGround,
	const ColorMapObject *ColorMap)
{
    int i;
    GifByteType Buf[3];
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile->Private;

    if (Private->FileState & FILE_STATE_SCREEN) {
	/* If already has screen descriptor - something is wrong! */
	_GifError = E_GIF_ERR_HAS_SCRN_DSCR;
	return GIF_ERROR;
    }
    if (!IS_WRITEABLE(Private)) {
	/* This file was NOT open for writing: */
	_GifError = E_GIF_ERR_NOT_WRITEABLE;
	return GIF_ERROR;
    }

/* First write the version prefix into the file. */
#ifndef DEBUG_NO_PREFIX
    if (WRITE(GifFile, GifVersionPrefix, GifVersionPrefixLen) !=
						GifVersionPrefixLen) {
	_GifError = E_GIF_ERR_WRITE_FAILED;
	return GIF_ERROR;
    }
#endif /* DEBUG_NO_PREFIX */

    GifFile->SWidth = Width;
    GifFile->SHeight = Height;
    GifFile->SColorResolution = ColorRes;
    GifFile->SBackGroundColor = BackGround;
    if(ColorMap)
      GifFile->SColorMap=MakeMapObject(ColorMap->ColorCount,ColorMap->Colors);
    else
      GifFile->SColorMap=NULL;

    /* Put the screen descriptor into the file: */
    EGifPutWord(Width, GifFile);
    EGifPutWord(Height, GifFile);
    Buf[0] = (ColorMap ? 0x80 : 0x00) |
	     ((ColorRes - 1) << 4) |
	     (ColorMap->BitsPerPixel - 1);
    Buf[1] = BackGround;
    Buf[2] = 0;
#ifndef DEBUG_NO_PREFIX
    WRITE(GifFile, Buf, 3);
#endif /* DEBUG_NO_PREFIX */

    /* If we have Global color map - dump it also: */
#ifndef DEBUG_NO_PREFIX
    if (ColorMap != NULL)
	for (i = 0; i < ColorMap->ColorCount; i++) {
	    /* Put the ColorMap out also: */
	    Buf[0] = ColorMap->Colors[i].Red;
	    Buf[1] = ColorMap->Colors[i].Green;
	    Buf[2] = ColorMap->Colors[i].Blue;
	    if (WRITE(GifFile, Buf, 3) != 3) {
	        _GifError = E_GIF_ERR_WRITE_FAILED;
		return GIF_ERROR;
	    }
	}
#endif /* DEBUG_NO_PREFIX */

    /* Mark this file as has screen descriptor, and no pixel written yet: */
    Private->FileState |= FILE_STATE_SCREEN;

    return GIF_OK;
}

/******************************************************************************
*   This routine should be called before any attemp to dump an image - any    *
* call to any of the pixel dump routines.				      *
******************************************************************************/
int EGifPutImageDesc(GifFileType *GifFile,
	int Left, int Top, int Width, int Height, int Interlace,
	const ColorMapObject *ColorMap)
{
    int i;
    GifByteType Buf[3];
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile->Private;

    if (Private->FileState & FILE_STATE_IMAGE &&
#if defined(__MSDOS__) || defined(__GNUC__)
	Private->PixelCount > 0xffff0000UL) {
#else
	Private->PixelCount > 0xffff0000) {
#endif /* __MSDOS__ */
	/* If already has active image descriptor - something is wrong! */
	_GifError = E_GIF_ERR_HAS_IMAG_DSCR;
	return GIF_ERROR;
    }
    if (!IS_WRITEABLE(Private)) {
	/* This file was NOT open for writing: */
	_GifError = E_GIF_ERR_NOT_WRITEABLE;
	return GIF_ERROR;
    }
    GifFile->Image.Left = Left;
    GifFile->Image.Top = Top;
    GifFile->Image.Width = Width;
    GifFile->Image.Height = Height;
    GifFile->Image.Interlace = Interlace;
    if(ColorMap)
      GifFile->Image.ColorMap =MakeMapObject(ColorMap->ColorCount,ColorMap->Colors);
    else
      GifFile->Image.ColorMap = NULL;

    /* Put the image descriptor into the file: */
    Buf[0] = ',';			       /* Image seperator character. */
#ifndef DEBUG_NO_PREFIX
    WRITE(GifFile, Buf, 1);
#endif /* DEBUG_NO_PREFIX */
    EGifPutWord(Left, GifFile);
    EGifPutWord(Top, GifFile);
    EGifPutWord(Width, GifFile);
    EGifPutWord(Height, GifFile);
    Buf[0] = (ColorMap ? 0x80 : 0x00) |
	  (Interlace ? 0x40 : 0x00) |
	  (ColorMap ? ColorMap->BitsPerPixel - 1 : 0);
#ifndef DEBUG_NO_PREFIX
    WRITE(GifFile, Buf, 1);
#endif /* DEBUG_NO_PREFIX */

    /* If we have Global color map - dump it also: */
#ifndef DEBUG_NO_PREFIX
    if (ColorMap != NULL)
	for (i = 0; i < ColorMap->ColorCount; i++) {
	    /* Put the ColorMap out also: */
	    Buf[0] = ColorMap->Colors[i].Red;
	    Buf[1] = ColorMap->Colors[i].Green;
	    Buf[2] = ColorMap->Colors[i].Blue;
	    if (WRITE(GifFile, Buf, 3) != 3) {
	        _GifError = E_GIF_ERR_WRITE_FAILED;
		return GIF_ERROR;
	    }
	}
#endif /* DEBUG_NO_PREFIX */
    if (GifFile->SColorMap == NULL && GifFile->Image.ColorMap == NULL)
    {
	_GifError = E_GIF_ERR_NO_COLOR_MAP;
	return GIF_ERROR;
    }

    /* Mark this file as has screen descriptor: */
    Private->FileState |= FILE_STATE_IMAGE;
    Private->PixelCount = (long) Width * (long) Height;

    EGifSetupCompress(GifFile);      /* Reset compress algorithm parameters. */

    return GIF_OK;
}

/******************************************************************************
*  Put one full scanned line (Line) of length LineLen into GIF file.	      *
******************************************************************************/
int EGifPutLine(GifFileType *GifFile, GifPixelType *Line, int LineLen)
{
    int i;
    GifPixelType Mask;
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile->Private;

    if (!IS_WRITEABLE(Private)) {
	/* This file was NOT open for writing: */
	_GifError = E_GIF_ERR_NOT_WRITEABLE;
	return GIF_ERROR;
    }

    if (!LineLen)
      LineLen = GifFile->Image.Width;
    if (Private->PixelCount < (unsigned)LineLen) {
	_GifError = E_GIF_ERR_DATA_TOO_BIG;
	return GIF_ERROR;
    }
    Private->PixelCount -= LineLen;

    /* Make sure the codes are not out of bit range, as we might generate    */
    /* wrong code (because of overflow when we combine them) in this case:   */
    Mask = CodeMask[Private->BitsPerPixel];
    for (i = 0; i < LineLen; i++) Line[i] &= Mask;

    return EGifCompressLine(GifFile, Line, LineLen);
}

/******************************************************************************
* Put a comment into GIF file using the GIF89 comment extension block.        *
******************************************************************************/
int EGifPutComment(GifFileType *GifFile, const char *Comment)
{
    return EGifPutExtension(GifFile, COMMENT_EXT_FUNC_CODE, strlen(Comment),
								Comment);
}

/******************************************************************************
*   Put a first extension block (see GIF manual) into gif file.  Here more    *
* extensions can be dumped using EGifPutExtensionMid until		      *
* EGifPutExtensionLast is invoked.					      *
******************************************************************************/
int EGifPutExtensionFirst(GifFileType *GifFile, int ExtCode, int ExtLen,
							const VoidPtr Extension)
{
    GifByteType Buf[3];
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile->Private;

    if (!IS_WRITEABLE(Private)) {
	/* This file was NOT open for writing: */
	_GifError = E_GIF_ERR_NOT_WRITEABLE;
	return GIF_ERROR;
    }

    if (ExtCode == 0)
	fwrite(&ExtLen, 1, 1, Private->File);
    else
    {
	Buf[0] = '!';
	Buf[1] = ExtCode;
	Buf[2] = ExtLen;
	fwrite(Buf, 1, 3, Private->File);
    }
    fwrite(Extension, 1, ExtLen, Private->File);

    return GIF_OK;
}

/******************************************************************************
*   Put a middle extension block (see GIF manual) into gif file.	      *
******************************************************************************/
int EGifPutExtensionNext(GifFileType *GifFile, int ExtCode, int ExtLen,
							const VoidPtr Extension)
{
    GifByteType Buf;
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile->Private;

    if (!IS_WRITEABLE(Private)) {
	/* This file was NOT open for writing: */
	_GifError = E_GIF_ERR_NOT_WRITEABLE;
	return GIF_ERROR;
    }

    Buf = ExtLen;
    fwrite(&Buf, 1, 1, Private->File);
    fwrite(Extension, 1, ExtLen, Private->File);

    return GIF_OK;
}

/******************************************************************************
*   Put a last extension block (see GIF manual) into gif file.		      *
******************************************************************************/
int EGifPutExtensionLast(GifFileType *GifFile, int ExtCode, int ExtLen,
							const VoidPtr Extension)
{
    GifByteType Buf;
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile->Private;

    if (!IS_WRITEABLE(Private)) {
	/* This file was NOT open for writing: */
	_GifError = E_GIF_ERR_NOT_WRITEABLE;
	return GIF_ERROR;
    }

    Buf = ExtLen;
    fwrite(&Buf, 1, 1, Private->File);
    fwrite(Extension, 1, ExtLen, Private->File);

    Buf = 0;
    fwrite(&Buf, 1, 1, Private->File);

    return GIF_OK;
}

/******************************************************************************
*   Put an extension block (see GIF manual) into gif file.		      *
******************************************************************************/
int EGifPutExtension(GifFileType *GifFile, int ExtCode, int ExtLen,
						  const VoidPtr Extension)
{
    GifByteType Buf[3];
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile->Private;

    if (!IS_WRITEABLE(Private)) {
	    /* This file was NOT open for writing: */
	    _GifError = E_GIF_ERR_NOT_WRITEABLE;
	    return GIF_ERROR;
    }

    if (ExtCode == 0)
        WRITE(GifFile, (GifByteType*)&ExtLen, 1);
    else {
	    Buf[0] = '!';
	    Buf[1] = ExtCode;
	    Buf[2] = ExtLen;
        WRITE(GifFile, Buf, 3);
    }
    WRITE(GifFile, Extension, ExtLen);
    Buf[0] = 0;
    WRITE(GifFile, Buf, 1);

    return GIF_OK;
}

/******************************************************************************
*   This routine should be called last, to close GIF file.		      *
******************************************************************************/
int EGifCloseFile(GifFileType *GifFile)
{
    GifByteType Buf;
    GifFilePrivateType *Private;
    FILE *File;

    if (GifFile == NULL) return GIF_ERROR;

    Private = (GifFilePrivateType *) GifFile->Private;
    if (!IS_WRITEABLE(Private)) {
	/* This file was NOT open for writing: */
	_GifError = E_GIF_ERR_NOT_WRITEABLE;
	return GIF_ERROR;
    }

    File = Private->File;

    Buf = ';';
    WRITE(GifFile, &Buf, 1);

    if (GifFile->Image.ColorMap)
	FreeMapObject(GifFile->Image.ColorMap);
    if (GifFile->SColorMap)
	FreeMapObject(GifFile->SColorMap);
    if (Private) {
	free((char *) Private);
    }
    free(GifFile);

    if (File && fclose(File) != 0) {
	_GifError = E_GIF_ERR_CLOSE_FAILED;
	return GIF_ERROR;
    }
    return GIF_OK;
}

/******************************************************************************
*   Put 2 bytes (word) into the given file:				      *
******************************************************************************/
static int EGifPutWord(int Word, GifFileType *GifFile)
{
    char c[2];

    c[0] = Word & 0xff;
    c[1] = (Word >> 8) & 0xff;
#ifndef DEBUG_NO_PREFIX
    if (WRITE(GifFile, c, 2) == 2)
	return GIF_OK;
    else
	return GIF_ERROR;
#else
    return GIF_OK;
#endif /* DEBUG_NO_PREFIX */
}

/******************************************************************************
*   Setup the LZ compression for this image:				      *
******************************************************************************/
static int EGifSetupCompress(GifFileType *GifFile)
{
    int BitsPerPixel;
    GifByteType Buf;
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile->Private;

    /* Test and see what color map to use, and from it # bits per pixel: */
    if (GifFile->Image.ColorMap)
	BitsPerPixel = GifFile->Image.ColorMap->BitsPerPixel;
    else if (GifFile->SColorMap)
	BitsPerPixel = GifFile->SColorMap->BitsPerPixel;
    else {
	_GifError = E_GIF_ERR_NO_COLOR_MAP;
	return GIF_ERROR;
    }

    Buf = BitsPerPixel = (BitsPerPixel < 2 ? 2 : BitsPerPixel);
    WRITE(GifFile, &Buf, 1);     /* Write the Code size to file. */

    Private->Buf[0] = 0;			  /* Nothing was output yet. */
    Private->BitsPerPixel = BitsPerPixel;
    Private->ClearCode = (1 << BitsPerPixel);
    Private->EOFCode = Private->ClearCode + 1;
    Private->RunningCode = 0;
    Private->RunningBits = BitsPerPixel + 1;	 /* Number of bits per code. */
    Private->MaxCode1 = 1 << Private->RunningBits;	   /* Max. code + 1. */
    Private->CrntCode = FIRST_CODE;	   /* Signal that this is first one! */
    Private->CrntShiftState = 0;      /* No information in CrntShiftDWord. */
    Private->CrntShiftDWord = 0;

    /* Send Clear to make sure the encoder is initialized. */
    if (EGifCompressOutput(GifFile, Private->ClearCode) == GIF_ERROR) {
	_GifError = E_GIF_ERR_DISK_IS_FULL;
	return GIF_ERROR;
    }
    return GIF_OK;
}

/******************************************************************************
*   The LZ compression routine:						      *
*   This version compress the given buffer Line of length LineLen.	      *
*   This routine can be called few times (one per scan line, for example), in *
* order the complete the whole image.					      *
******************************************************************************/
static int EGifCompressLine(GifFileType *GifFile, GifPixelType *Line,
								int LineLen)
{
    int i = 0, CrntCode;
    GifPixelType Pixel;
    GifFilePrivateType *Private = (GifFilePrivateType *) GifFile->Private;

    if (Private->CrntCode == FIRST_CODE)		  /* Its first time! */
	CrntCode = Line[i++];
    else
        CrntCode = Private->CrntCode;     /* Get last code in compression. */

    while (i < LineLen) {			    /* Decode LineLen items. */
	Pixel = Line[i++];		      /* Get next pixel from stream. */
    if (EGifCompressOutput(GifFile, CrntCode)
        == GIF_ERROR) {
        _GifError = E_GIF_ERR_DISK_IS_FULL;
        return GIF_ERROR;
    }
    Private->RunningCode++;
    CrntCode = Pixel;
    if (Private->RunningCode >= (1 << (Private->BitsPerPixel)) - 2) {
        if (EGifCompressOutput(GifFile, Private->ClearCode)
            == GIF_ERROR) {
            _GifError = E_GIF_ERR_DISK_IS_FULL;
            return GIF_ERROR;
        }
        Private->RunningCode=0;
    }
    }

    /* Preserve the current state of the compression algorithm: */
    Private->CrntCode = CrntCode;

    if (Private->PixelCount == 0)
    {
	/* We are done - output last Code and flush output buffers: */
	if (EGifCompressOutput(GifFile, CrntCode)
	    == GIF_ERROR) {
	    _GifError = E_GIF_ERR_DISK_IS_FULL;
	    return GIF_ERROR;
	}
	if (EGifCompressOutput(GifFile, Private->EOFCode)
	    == GIF_ERROR) {
	    _GifError = E_GIF_ERR_DISK_IS_FULL;
	    return GIF_ERROR;
	}
	if (EGifCompressOutput(GifFile, FLUSH_OUTPUT) == GIF_ERROR) {
	    _GifError = E_GIF_ERR_DISK_IS_FULL;
	    return GIF_ERROR;
	}
    }

    return GIF_OK;
}

/******************************************************************************
*   The LZ compression output routine:                                        *
*   This routine is responsible for the compression of the bit stream into    *
*   8 bits (bytes) packets.                                                   *
*   Returns GIF_OK if written succesfully.                                    *
******************************************************************************/
static int EGifCompressOutput(GifFileType *GifFile, int Code)
{
    GifFilePrivateType *Private = (GifFilePrivateType *)GifFile->Private;
    int retval = GIF_OK;

    if (Code == FLUSH_OUTPUT) {
	while (Private->CrntShiftState > 0) {
	    /* Get Rid of what is left in DWord, and flush it. */
	    if (EGifBufferedOutput(GifFile, Private->Buf,
		Private->CrntShiftDWord & 0xff) == GIF_ERROR)
		    retval = GIF_ERROR;
	    Private->CrntShiftDWord >>= 8;
	    Private->CrntShiftState -= 8;
	}
	Private->CrntShiftState = 0;			   /* For next time. */
	if (EGifBufferedOutput(GifFile, Private->Buf,
	    FLUSH_OUTPUT) == GIF_ERROR)
    	        retval = GIF_ERROR;
    }
    else {
	Private->CrntShiftDWord |= ((long) Code) << Private->CrntShiftState;
	Private->CrntShiftState += Private->RunningBits;
	while (Private->CrntShiftState >= 8) {
	    /* Dump out full bytes: */
	    if (EGifBufferedOutput(GifFile, Private->Buf,
		Private->CrntShiftDWord & 0xff) == GIF_ERROR)
		    retval = GIF_ERROR;
	    Private->CrntShiftDWord >>= 8;
	    Private->CrntShiftState -= 8;
	}
    }

    return retval;
}

/******************************************************************************
*   This routines buffers the given characters until 255 characters are ready *
* to be output. If Code is equal to -1 the buffer is flushed (EOF).	      *
*   The buffer is Dumped with first byte as its size, as GIF format requires. *
*   Returns GIF_OK if written succesfully.				      *
******************************************************************************/
static int EGifBufferedOutput(GifFileType *GifFile, GifByteType *Buf, int c)
{
    if (c == FLUSH_OUTPUT) {
	/* Flush everything out. */
	if (Buf[0] != 0 && WRITE(GifFile, Buf, Buf[0]+1) != (unsigned)(Buf[0] + 1))
	{
	    _GifError = E_GIF_ERR_WRITE_FAILED;
	    return GIF_ERROR;
	}
	/* Mark end of compressed data, by an empty block (see GIF doc): */
	Buf[0] = 0;
	if (WRITE(GifFile, Buf, 1) != 1)
	{
	    _GifError = E_GIF_ERR_WRITE_FAILED;
	    return GIF_ERROR;
	}
    }
    else {
	if (Buf[0] == 255) {
	    /* Dump out this buffer - it is full: */
	    if (WRITE(GifFile, Buf, Buf[0] + 1) != (unsigned)(Buf[0] + 1))
	    {
		_GifError = E_GIF_ERR_WRITE_FAILED;
		return GIF_ERROR;
	    }
	    Buf[0] = 0;
	}
	Buf[++Buf[0]] = c;
    }

    return GIF_OK;
}

