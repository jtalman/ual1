/* @(#)root/base:$Name:  $:$Id: Windows4Root.h,v 1.6 2004/01/23 18:50:03 brun Exp $ */

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_Windows4Root
#define ROOT_Windows4Root


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// This include file is necessary to solve a problem with the original  //
// windows.h file from Microsoft.                                       //
// The native windows.h redefines a.o. the following names:             //
//     RemoveDirectory                                                  //
//     GetClassName                                                     //
//     GetTextAlign                                                     //
//     GetTextColor                                                     //
//                                                                      //
// This include file references the original windows.h file             //
// and undefines these symbols.                                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef __CINT__

#include <windows.h>


#undef OpenSemaphore

#undef RemoveDirectory
#undef GetClassName
#undef GetTextAlign
#undef GetTextColor

#undef SetTextAlign
#undef SetTextColor
#undef UpdateWindow
#undef SetClipRegion

#undef ClearWindow
#undef ClosePixmap
#undef CloseWindow
#undef CopyPixmap
#undef CopyFile
#undef DrawBox
#undef DrawCellArray
#undef DrawFillArea
#undef DrawLine
#undef DrawPolyLine
#undef DrawPolyMarker
#undef DrawText
#undef GetCharacterUp

#undef GetDoubleBuffer
#undef GetPixel
#undef GetPlanes
#undef GetRGB
#undef GetTextExtent
#undef InitWindow
#undef AddWindow
#undef RemoveWindow
#undef MoveWindow
#undef OpenPixmap
#undef PutByte
#undef QueryPointer
#undef RescaleWindow
#undef ResizePixmap
#undef ResizeWindow
#undef SelectWindow
#undef SetCharacterUp
#undef SetClipOFF
#undef SetClipRegion
#undef SetCursor
#undef SetDrawMode
#undef SetFillColor
#undef SetFillStyle
#undef SetLineColor
#undef SetLineType
#undef SetLineStyle
#undef SetLineWidth
#undef SetMarkerColor
#undef SetMarkerSize
#undef SetMarkerStyle
#undef SetRGB
#undef SetTextAlign
#undef SetTextColor
#undef SetTextFont
#undef SetTextFont
#undef SetTextSize
#undef UpdateWindow
#undef Warp
#undef WritePixmap
#undef CreateWindow
#undef CreateRegion
#undef DestroyRegion
#undef UnionRectWithRegion
#undef PolygonRegion
#undef UnionRegion
#undef IntersectRegion
#undef SubtractRegion
#undef XorRegion
#undef EmptyRegion
#undef PointInRegion
#undef EqualRegion
#undef GetRegionBox
#undef GetCurrentTime

#if !defined(ROOT_TGWin32Object) && !defined(ROOT_TGWin32)
#   undef GetObject
#   undef GetClassInfo
#endif

#else
    typedef void * HANDLE;
#endif

#endif
