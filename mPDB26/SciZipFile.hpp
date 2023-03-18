// CodeGear C++Builder
// Copyright (c) 1995, 2008 by CodeGear
// All rights reserved

// (DO NOT EDIT: machine generated header) 'Scizipfile.pas' rev: 20.00

#ifndef ScizipfileHPP
#define ScizipfileHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member functions
#pragma pack(push,8)
#include <System.hpp>	// Pascal unit
#include <Sysinit.hpp>	// Pascal unit
#include <Dialogs.hpp>	// Pascal unit
#include <Sysutils.hpp>	// Pascal unit
#include <Classes.hpp>	// Pascal unit
#include <Types.hpp>	// Pascal unit
#include <Zlib.hpp>	// Pascal unit
#include <Windows.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Scizipfile
{
//-- type declarations -------------------------------------------------------
#pragma pack(push,1)
struct TCommonFileHeader
{
	
public:
	System::Word VersionNeededToExtract;
	System::Word GeneralPurposeBitFlag;
	System::Word CompressionMethod;
	unsigned LastModFileTimeDate;
	unsigned Crc32;
	unsigned CompressedSize;
	unsigned UncompressedSize;
	System::Word FilenameLength;
	System::Word ExtraFieldLength;
};
#pragma pack(pop)


#pragma pack(push,1)
struct TLocalFile
{
	
public:
	unsigned LocalFileHeaderSignature;
	TCommonFileHeader CommonFileHeader;
	System::AnsiString filename;
	System::AnsiString extrafield;
	System::AnsiString CompressedData;
};
#pragma pack(pop)


#pragma pack(push,1)
struct TFileHeader
{
	
public:
	unsigned CentralFileHeaderSignature;
	System::Word VersionMadeBy;
	TCommonFileHeader CommonFileHeader;
	System::Word FileCommentLength;
	System::Word DiskNumberStart;
	System::Word InternalFileAttributes;
	unsigned ExternalFileAttributes;
	unsigned RelativeOffsetOfLocalHeader;
	System::AnsiString filename;
	System::AnsiString extrafield;
	System::AnsiString fileComment;
};
#pragma pack(pop)


#pragma pack(push,1)
struct TEndOfCentralDir
{
	
public:
	unsigned EndOfCentralDirSignature;
	System::Word NumberOfThisDisk;
	System::Word NumberOfTheDiskWithTheStart;
	System::Word TotalNumberOfEntriesOnThisDisk;
	System::Word TotalNumberOfEntries;
	unsigned SizeOfTheCentralDirectory;
	unsigned OffsetOfStartOfCentralDirectory;
	System::Word ZipfileCommentLength;
};
#pragma pack(pop)


class DELPHICLASS TZipFile;
class PASCALIMPLEMENTATION TZipFile : public System::TObject
{
	typedef System::TObject inherited;
	
private:
	typedef DynamicArray<TLocalFile> _TZipFile__1;
	
	typedef DynamicArray<TFileHeader> _TZipFile__2;
	
	
public:
	_TZipFile__1 Files;
	_TZipFile__2 CentralDirectory;
	TEndOfCentralDir EndOfCentralDirectory;
	System::AnsiString ZipFileComment;
	bool Compress;
	
private:
	System::AnsiString __fastcall GetUncompressed(int i);
	void __fastcall SetUncompressed(int i, const System::AnsiString Value);
	System::TDateTime __fastcall GetDateTime(int i);
	void __fastcall SetDateTime(int i, const System::TDateTime Value);
	int __fastcall GetCount(void);
	System::AnsiString __fastcall GetName(int i);
	void __fastcall SetName(int i, const System::AnsiString Value);
	
public:
	__property int Count = {read=GetCount, nodefault};
	void __fastcall AddFile(const System::AnsiString Name, unsigned FAttribute = (unsigned)(0x0));
	void __fastcall SaveToFile(const Sysutils::TFileName filename);
	void __fastcall SaveToStream(Classes::TStream* ZipFileStream);
	void __fastcall LoadFromFile(const Sysutils::TFileName filename);
	void __fastcall LoadFromStream(const Classes::TStream* ZipFileStream);
	__property System::AnsiString Uncompressed[int i] = {read=GetUncompressed, write=SetUncompressed};
	__property System::AnsiString Data[int i] = {read=GetUncompressed, write=SetUncompressed};
	__property System::TDateTime DateTime[int i] = {read=GetDateTime, write=SetDateTime};
	__property System::AnsiString Name[int i] = {read=GetName, write=SetName};
public:
	/* TObject.Create */ inline __fastcall TZipFile(void) : System::TObject() { }
	/* TObject.Destroy */ inline __fastcall virtual ~TZipFile(void) { }
	
};


class DELPHICLASS EZipFileCRCError;
class PASCALIMPLEMENTATION EZipFileCRCError : public Sysutils::Exception
{
	typedef Sysutils::Exception inherited;
	
public:
	/* Exception.Create */ inline __fastcall EZipFileCRCError(const System::UnicodeString Msg) : Sysutils::Exception(Msg) { }
	/* Exception.CreateFmt */ inline __fastcall EZipFileCRCError(const System::UnicodeString Msg, System::TVarRec const *Args, const int Args_Size) : Sysutils::Exception(Msg, Args, Args_Size) { }
	/* Exception.CreateRes */ inline __fastcall EZipFileCRCError(int Ident)/* overload */ : Sysutils::Exception(Ident) { }
	/* Exception.CreateResFmt */ inline __fastcall EZipFileCRCError(int Ident, System::TVarRec const *Args, const int Args_Size)/* overload */ : Sysutils::Exception(Ident, Args, Args_Size) { }
	/* Exception.CreateHelp */ inline __fastcall EZipFileCRCError(const System::UnicodeString Msg, int AHelpContext) : Sysutils::Exception(Msg, AHelpContext) { }
	/* Exception.CreateFmtHelp */ inline __fastcall EZipFileCRCError(const System::UnicodeString Msg, System::TVarRec const *Args, const int Args_Size, int AHelpContext) : Sysutils::Exception(Msg, Args, Args_Size, AHelpContext) { }
	/* Exception.CreateResHelp */ inline __fastcall EZipFileCRCError(int Ident, int AHelpContext)/* overload */ : Sysutils::Exception(Ident, AHelpContext) { }
	/* Exception.CreateResFmtHelp */ inline __fastcall EZipFileCRCError(System::PResStringRec ResStringRec, System::TVarRec const *Args, const int Args_Size, int AHelpContext)/* overload */ : Sysutils::Exception(ResStringRec, Args, Args_Size, AHelpContext) { }
	/* Exception.Destroy */ inline __fastcall virtual ~EZipFileCRCError(void) { }
	
};


//-- var, const, procedure ---------------------------------------------------
extern PACKAGE unsigned __fastcall ZipCRC32(const System::AnsiString Data);

}	/* namespace Scizipfile */
using namespace Scizipfile;
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// ScizipfileHPP
