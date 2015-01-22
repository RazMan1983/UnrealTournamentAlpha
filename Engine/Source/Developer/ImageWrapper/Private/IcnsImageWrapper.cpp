// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "ImageWrapperPrivatePCH.h"
#include "BmpImageSupport.h"

/**
 * ICNS image wrapper class.
 */
FIcnsImageWrapper::FIcnsImageWrapper()
	: FImageWrapperBase()
{
}

void FIcnsImageWrapper::Compress(int32 Quality)
{
	checkf(false, TEXT("ICNS compression not supported"));
}

void FIcnsImageWrapper::Uncompress(const ERGBFormat::Type InFormat, const int32 InBitDepth)
{
#if PLATFORM_MAC
	SCOPED_AUTORELEASE_POOL;

	NSData* ImageData = [NSData dataWithBytesNoCopy:CompressedData.GetData() length:CompressedData.Num() freeWhenDone:NO];
	NSImage* Image = [[NSImage alloc] initWithData:ImageData];
	if (Image)
	{
		NSBitmapImageRep* Bitmap = [NSBitmapImageRep imageRepWithData:[Image TIFFRepresentation]];
		if (Bitmap)
		{
			// @todo: Only 8-bit BGRA supported currently
			check(InFormat == ERGBFormat::BGRA);
			check(InBitDepth == 8);

			RawData.Empty();
			RawData.Append([Bitmap bitmapData], [Bitmap bytesPerPlane]);

			RawFormat = Format = InFormat;
			RawBitDepth = BitDepth = InBitDepth;

			Width = [Bitmap pixelsWide];
			Height = [Bitmap pixelsHigh];
		}
		[Image release];
	}
#else
	checkf(false, TEXT("ICNS uncompressing not supported on this platform"));
#endif
}
