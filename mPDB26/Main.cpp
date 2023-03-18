#include <vcl.h>
#pragma hdrstop

#include "Main.h"
#include "MPanel.h"

#include <tchar.h>
#include <memory>
#include <string.h>
#include <stdio.h>
#include <System.hpp>
#include <FileCtrl.hpp>
#include <Sysutils.hpp>

#include <IdGlobal.hpp>
#include <Dialogs.hpp>

#include "SciZipFile.hpp"
TZipFile *Zip;
int ZipCount;

UnicodeString ProgramTitle = L"�nŪ mPDB 2.6";

char PdbHeader[78];

UnicodeString ExePath;
UnicodeString Buffer;

TBytes BOM;   // FF FE little endian byte order
TBytes Bytes;

UnicodeString PdbName;
UnicodeString SetupFile;

UnicodeString Directory;
UnicodeString Author;
UnicodeString Title;
UnicodeString Info;
UnicodeString Secret;
UnicodeString EditFile;

UnicodeString ePub;
UnicodeString OEBPS;
UnicodeString PRC;
UnicodeString MetaInf;
UnicodeString ChapterTitle;

int   SetupChanged = false;

char  PdbVersion = 2;
char* PdbType = "BOOK";
bool  Big5;
bool  Big5txtLoaded;
bool  ConvertToBig5 = false;
char* PdbCreator = "MTIU";  // Unicode: MTIU; Big5: MTIT
char* IniStr = "mPDB.ini";

int   RecordCount;
char  PdbList[4100]; // max = 499 chapters
TBytes PdbBookInfo;

int ChapterCount = 0;
int ChapterNumber;
int ChapterNumberInitial;
int ChapterNumberStart;
int ChapterNumberSaved = 0;;
int ChapterSize[500];

TCursor SaveCursor;
int SavingSetupFile = false;
int SetupFileChanged = false;

UnicodeString EditChapter;
UnicodeString EditFileSaved;
UnicodeString EditFileTemp;
UnicodeString EditFilePrev;

UnicodeString SourceStr;
UnicodeString TargetStr;

int ResumePosition = 0;
int SelStartSaved;
int SelLengthSaved;
int SelStartTemp;
int SelStartPrev;
int SelLengthTemp;
int SelLengthPrev;

UnicodeString PREditFileTemp;
UnicodeString PREditFilePrev;
int PRSelStartTemp;
int PRSelStartPrev;
int PRSelLengthTemp;
int PRSelLengthPrev;
int PRIndexTemp;
int PRIndexPrev;

UnicodeString CNEditFileTemp;
UnicodeString CNEditFilePrev;
int CNSelStartTemp;
int CNSelStartPrev;
int CNSelLengthTemp;
int CNSelLengthPrev;

bool isPRC;
bool VerticalEpub;
bool AutoReplace;
bool SearchOnly;
UnicodeString ExcludeNext;
UnicodeString ExcludePrev;

int PRResumePosition = 0;
int PRResumeIndex = 0;
UnicodeString PRFile;
TStringList* PRList;
TStringList* PRSource;
TStringList* PRTarget;

int SkipChineseZero = false;

TMemo *Memo;
MPanel* Panel = NULL; // 2006/5/30
int Preview = false;
int PlateNumber = 0;
int FlipPageNumber = 1;
int PlatePageNumber[4];
int PlatePageLoaded[4];

TButton* Plate1Button[72];
TButton* Plate2Button[72];
TButton* Plate3Button[48];
int Plate1ButtonsCreated = false;
int Plate2ButtonsCreated = false;
int Plate3ButtonsCreated = false;

int WindowHeight = 737; // 735;
int WindowWidth = 762; // 756;
int PreviewWidth = 620;

int PlateModeSwitched = false;


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
THome *Home;
//---------------------------------------------------------------------------

/*--------------*/
/* THome::THome */
/*--------------*/

__fastcall THome::THome(TComponent* Owner) : TForm(Owner) {

   Panel2->Hide();
   Panel3->Hide();
   Panel4->Hide();
   btnSwitch->Hide();
   TabControl2->Hide();
   fldSetupFile->Hide();

   Height = WindowHeight;
   Width = WindowWidth;
   GetExePath();
   Caption = ProgramTitle;
   PopupMenu1->AutoPopup = false;

//   ReportMemoryLeaksOnShutdown = true;
   BOM = TEncoding::Unicode->GetPreamble(); // FF FE little endian byte order

   LoadIni();
   Memo = Memo1;

   Panel2->Top = Panel1->Top;
   Panel2->Left = Panel1->Left;
   Panel2->Width = Panel1->Width;
   Panel2->Height = Panel1->Height;

   Panel3->Top = Panel1->Top;
   Panel3->Left = Panel1->Left;
   Panel3->Width = Panel1->Width;
   Panel3->Height = Panel1->Height;

   Panel4->Top = Panel1->Top;
   Panel4->Left = Panel1->Left;
   Panel4->Width = Panel1->Width;
   Panel4->Height = Panel1->Height;

   for (int i = 0; i <= 3; i++) {
      PlatePageNumber[i] = 1;
      PlatePageLoaded[i] = 0;
      }

   }


/*---------------*/
/* THome::~THome */
/*---------------*/

__fastcall THome::~THome() {

//   if (Encoding) Encoding->Free();

   if (Panel) delete Panel;
   if (Plate1ButtonsCreated) {
	  for (int i = 0; i < 72; i++)  delete Plate1Button[i];
      }
   if (Plate2ButtonsCreated) {
      for (int i = 0; i < 72; i++) delete Plate2Button[i];
      }
   if (Plate3ButtonsCreated) {
      for (int i = 0; i < 48; i++) delete Plate3Button[i];
      }

   }


/*------------------*/
/* THome::InitPanel */
/*------------------*/

void __fastcall THome::InitPanel() {

   Panel = new MPanel(this);
   Panel->Parent = this;
   Panel->TabOrder = 0;
   Panel->SetPageFont();

   }


/*----------------------*/
/* THome::CreateButtons */
/*----------------------*/

void __fastcall THome::CreateButtons() {

   if (PlateNumber == 1) {
      for (int i = 0; i < 72; i++) {
         TButton* button = new TButton(Panel2);
         button->Height = 25;
         button->Parent = Panel2;
         button->OnClick = btnInsertChar;
         Plate1Button[i] = button;
         }
      Plate1ButtonsCreated = true;
      }
   else if (PlateNumber == 2) {
      for (int i = 0; i < 72; i++) {
         TButton* button = new TButton(Panel3);
         button->Height = 25;
         button->Parent = Panel3;
         button->OnClick = btnInsertChar;
         Plate2Button[i] = button;
         }
      Plate2ButtonsCreated = true;
      }
   else if (PlateNumber == 3) {
      for (int i = 0; i < 48; i++) {
         TButton* button = new TButton(Panel4);
         button->Height = 25;
         button->Parent = Panel4;
         button->OnClick = btnInsertChar;
         Plate3Button[i] = button;
         }
      Plate3ButtonsCreated = true;
      }
   }


/*-------------------*/
/* THome::GetExePath */
/*-------------------*/

void __fastcall THome::GetExePath() {

   ExePath = ExtractFilePath(Application->ExeName);

   }


/*----------------------*/
/* THome::RestoreCursor */
/*----------------------*/

void __fastcall THome::RestoreCursor() {

   Screen->Cursor = SaveCursor;

   }


/*-----------------------*/
/* THome::ShowWaitCursor */
/*-----------------------*/

void __fastcall THome::ShowWaitCursor() {

   SaveCursor = Screen->Cursor;
   Screen->Cursor = crHourGlass;

   }


/*-----------------*/
/* THome::LoadFile */
/*-----------------*/

int __fastcall THome::LoadFile(const UnicodeString name) {

   TEncoding *Encoding = NULL;
   std::auto_ptr<TFileStream> FileStream(new TFileStream(name, fmOpenRead));
   TBytes Nothing;
   Nothing.set_length(FileStream->Size);
   std::auto_ptr<TBytesStream> BytesStream(new TBytesStream(Nothing));
   BytesStream->CopyFrom(FileStream.get(), FileStream->Size);

   TEncoding::GetBufferEncoding(BytesStream->Bytes, Encoding);
   if (Encoding != TEncoding::Unicode)
	  TEncoding::Convert(Encoding, TEncoding::Unicode,
		 BytesStream->Bytes);
   Buffer = Encoding->GetString(BytesStream->Bytes);
   if (Encoding == TEncoding::Unicode) Buffer.Delete(1, 1);

   }


/*------------------*/
/* THome::ShowAlert */
/*------------------*/

void __fastcall THome::ShowAlert(const UnicodeString msg) {

   Application->MessageBox(msg.w_str(), L"mPDB", MB_OK);

   }


/*-------------------*/
/* THome::YesNoAlert */
/*-------------------*/

int __fastcall THome::YesNoAlert(const UnicodeString msg) {

   return Application->MessageBox(msg.w_str(), L"mPDB", MB_YESNO);

   }


/*---------------------*/
/* THome::ConfirmAlert */
/*---------------------*/

int __fastcall THome::ConfirmAlert(const UnicodeString msg) {

   return Application->MessageBox(msg.w_str(), L"mPDB", MB_OKCANCEL);

   }


/*--------------------*/
/* THome::ChoiceAlert */
/*--------------------*/

int __fastcall THome::ChoiceAlert(const UnicodeString msg) {

   return Application->MessageBox(msg.w_str(), L"mPDB", MB_YESNOCANCEL);

   }


/*-----------------------*/
/* THome::GetChapterSize */
/*-----------------------*/

int __fastcall THome::GetChapterSize(int i) {

   UnicodeString name = Directory + '\\' + i + ".txt";
   if (!FileExists(name)) {
      ChapterSize[i] = 0;
      return false;
      }

   std::auto_ptr<TFileStream> FileStream(new TFileStream(name, fmOpenRead));
   int size = FileStream->Size;
   if (size >= 2) {
	  char bom[2];
	  FileStream->Read(bom, 2);
	  if ((bom[0] = BOM[0]) && (bom[1] = BOM[1])) {
		 size = size - 2;
		 }
	  }
/*
   if (size > 65500) {
	  name = name + L" �W�L64K�A�b Palm PDA �W�\Ū�|����I";
	  ShowAlert(name);
	  }
*/

   ChapterSize[i] = size;
   return true;

   }


/*------------------------*/
/* THome::GetChapterSizes */
/*------------------------*/

int __fastcall THome::GetChapterSizes() {

   for (int i = 1; i <= ChapterCount; i++) {
      GetChapterSize(i);
      }

   return true;

   }


/*--------------------*/
/* THome::LoadChapter */
/*--------------------*/

bool __fastcall THome::LoadChapter(int i) {

   EditChapter = Directory + '\\' + i + ".txt";
   Buffer.SetLength(0);
	ChapterNumber = i;
   if (!FileExists(EditChapter)) return false;

	LoadFile(EditChapter);
   Buffer.TrimRight();
	ScanBufferIn();
   return true;

   }


/*--------------------*/
/* THome::SaveChapter */
/*--------------------*/

void __fastcall THome::SaveChapter(int i) {

   UnicodeString name = Directory + '\\' + i + ".txt";

   std::auto_ptr<TFileStream> FileStream(new TFileStream(name, fmCreate));
   FileStream->Write(&BOM[0], BOM.Length);
   TBytes out = WideBytesOf(Buffer);
   FileStream->Write(&out[0], out.Length);

   }


/*---------------------*/
/* THome::SetPdbHeader */
/*---------------------*/

void __fastcall THome::SetPdbHeader() {

   Init:

	  memset(PdbHeader, 0, 78);

/* 2010/6/27
   Set_Pdb_Name:

	  TBytes out = WideBytesOf(PdbName);
	  ShowAlert(out.Length);
	  memcpy(PdbHeader, &out[0], out.Length);
*/

   Set_Author:

      TBytes outAuthor = WideBytesOf(Author);
      TEncoding *Encoding = NULL;
      TEncoding::GetBufferEncoding(outAuthor, Encoding);
      if (ConvertToBig5) {
	      outAuthor = TEncoding::Convert(TEncoding::Unicode,
		      Encoding->GetEncoding(950), outAuthor);
         }
      int size = outAuthor.Length;
      if (size > 32) size = 32;
      memcpy(&PdbHeader[0], &outAuthor[0], size);

   Set_File_Attributes:

      PdbHeader[33] = 0; // Do not backup

   Set_Version:
   
      PdbHeader[35] = PdbVersion;

   Set_Creation_Modification_Date:

      PdbHeader[36] = PdbHeader[40] = 59;
      PdbHeader[37] = PdbHeader[41] = 41;
      PdbHeader[38] = PdbHeader[42] = 155;
      PdbHeader[39] = PdbHeader[43] = 229;

   Set_Database_Type:

      strcpy(&PdbHeader[60], PdbType);

   Set_Creator_ID:

	  strcpy(&PdbHeader[64], PdbCreator);
	  if (ConvertToBig5) PdbHeader[67] = 'T';

   Set_Record_Count:

      int total = 2 + ChapterCount;
      char* ptr = (char*) &total;
      PdbHeader[76] = ptr[1];
      PdbHeader[77] = ptr[0];

   }


/*-----------------------*/
/* THome::SetPdbBookInfo */
/*-----------------------*/

void __fastcall THome::SetPdbBookInfo() {

   LoadChapter(0);
   TBytes outTitle = WideBytesOf(Title);
   TBytes outBuffer = WideBytesOf(Buffer);
   TEncoding *Encoding = NULL;
   TEncoding::GetBufferEncoding(outBuffer, Encoding);
   if (ConvertToBig5) {
	  outTitle = TEncoding::Convert(TEncoding::Unicode,
		 Encoding->GetEncoding(950), outTitle);
	  outBuffer = TEncoding::Convert(TEncoding::Unicode,
		 Encoding->GetEncoding(950), outBuffer);
	  }

   AnsiString count = ChapterCount;
   int i = 8 // 8 spaces
	  + outTitle.Length // The Length of Title
	  + 8  // 4 (27, 0)
	  + count.Length()
	  + outBuffer.Length;
   if (ConvertToBig5) i -= 3; // remove three 0s
   PdbBookInfo.set_length(i);

   for (i = 0; i < 8; i++) PdbBookInfo[i] = ' ';
//   if (CheckBox1->Checked) PdbBookInfo[1] = 1;

   memcpy(&PdbBookInfo[8], &outTitle[0], outTitle.Length);
   i += outTitle.Length;
   PdbBookInfo[i++] = 27;
   if (!ConvertToBig5) PdbBookInfo[i++] = 0;

//   strcpy(&PdbBookInfo[i], Info.c_str());
   PdbBookInfo[i++] = 27;
   if (!ConvertToBig5) PdbBookInfo[i++] = 0;

//   strcpy(&PdbBookInfo[i], Secret.c_str());
   PdbBookInfo[i++] = 27;
   if (!ConvertToBig5) PdbBookInfo[i++] = 0;

   strcpy(&PdbBookInfo[i], count.c_str());
   i += count.Length();
   PdbBookInfo[i++] = 27;
   if (!ConvertToBig5) PdbBookInfo[i++] = 0;
   if (outBuffer.Length == 0) return;
   if (!ConvertToBig5) {
	  memcpy(&PdbBookInfo[i], &outBuffer[0], outBuffer.Length);
	  return;
	  }
   for (int k = 0; k < outBuffer.Length; k++) {
	  char c = outBuffer[k];
      if (c == 13) PdbBookInfo[i++] = 27;
      else if (c != 10) PdbBookInfo[i++] = c;
      }
   PdbBookInfo[i] = 0;
   PdbBookInfo.set_length(i+1);

   }


/*----------------------------*/
/* THome::GetChapterBig5Sizes */
/*----------------------------*/

void __fastcall THome::GetChapterBig5Sizes() {

   for (int i = 1; i <= ChapterCount; i++) {
	  if (!LoadChapter(i)) {
        ChapterSize[i] = 0;
        continue;
        }

     ScanBufferOut();
	  TBytes outBuffer = WideBytesOf(Buffer);
	  TEncoding *Encoding = NULL;
	  TEncoding::GetBufferEncoding(outBuffer, Encoding);
	  outBuffer = TEncoding::Convert(TEncoding::Unicode,
		 Encoding->GetEncoding(950), outBuffer);
	  int size = outBuffer.Length + 1;
	  if (size > 65500) {
         UnicodeString name = Directory + '\\' + i + ".txt";
		 name = name + L" �W�L64K�A�b Palm PDA �W�\Ū�|����I";
		 ShowAlert(name);
		 }
	  ChapterSize[i] = size;
	  }

   }

/*---------------------*/
/* THome::ScanBufferIn */
/*---------------------*/

void __fastcall THome::ScanBufferIn() {

   int size = Buffer.Length();
   for (int i = 1; i <= size; i++) {
	   if (Buffer[i] == L'�x') Buffer[i] = 0xFE19; //0xFFE4;
	   }

   int zero = 0;
   for (int i = size; i >= 1; i--) {
      if (Buffer[i] != 0) break;
      zero++;
      }
   if (zero > 0) {
//      ShowAlert(EditChapter); // 2011/4/13 Debug Should remove
//      ShowAlert(zero);        // 2011/4/13 Debug
      Buffer.SetLength(size-zero);
      }

   }

/*----------------------*/
/* THome::ScanBufferOut */
/*----------------------*/

void __fastcall THome::ScanBufferOut() {

	for (int i = 1; i <= Buffer.Length(); i++) {
	   if (Buffer[i] == 0xFFE4) Buffer[i] = 0x2502;
	   }
   if (!Big5txtLoaded) return;

   for (int d = 0; d < PRSource->Count; d++) {
      UnicodeString source = PRSource->Strings[d];
      int length1 = source.Length();
      UnicodeString target = PRTarget->Strings[d];
      int length2 = target.Length();
      for (int j = 1; j <= Buffer.Length(); j++) {
         if (j + length1 - 1 > Buffer.Length()) break;
         int k = 0;
         while (k < length1) {
             if (Buffer[j+k] != source[k+1]) break;
             k++;
            }
         if (k < length1) continue;
         LogBig5Replacement(j, length1, target);
         Buffer.Delete(j, length1);
         Buffer.Insert(target, j);
         j = j + length2 - 1;
         }
      }

   }


/*-----------------*/
/* THome::WritePdb */
/*-----------------*/

void __fastcall THome::WritePdb() {

   Open_File:

	  UnicodeString name = Directory + '\\' + PdbName;
	  if (ConvertToBig5) name = name + ".pdb";
		 else name = name + ".updb";
	  std::auto_ptr<TFileStream> FileStream(new TFileStream(name, fmCreate));

   Write_Header:

	  FileStream->Write(PdbHeader, 78);

   Load_Big_Table:

      Big5txtLoaded = false;
      if (ConvertToBig5) {
        UnicodeString name = ExePath + L"big5.txt";
        if (FileExists(name)) {
           PRFile = name;
           Big5txtLoaded = LoadPRList();
           }
        }

   Write_Record_List:

      if (ConvertToBig5) GetChapterBig5Sizes(); else GetChapterSizes();
      RecordCount = 2 + ChapterCount;
      int size = RecordCount * 8;
      int offset = 78 + size;
      memset(PdbList, 0, size);
      int length = PdbBookInfo.Length; //strlen(PdbBookInfo) + 1;

      int p = 0;
      char* ptr = (char*) &offset;
      PdbList[p++] = ptr[3];
      PdbList[p++] = ptr[2];
      PdbList[p++] = ptr[1];
      PdbList[p++] = ptr[0];
      offset += length;
      p += 4;

      int i;
      for (i = 1; i <= ChapterCount + 1; i++) {
         ptr = (char*) &offset;
         PdbList[p++] = ptr[3];
         PdbList[p++] = ptr[2];
         PdbList[p++] = ptr[1];
         PdbList[p++] = ptr[0];
		 if (i <= ChapterCount) offset += ChapterSize[i];
         p += 4;
         }

	  FileStream->Write(PdbList, size);

   Write_Book_Info:

	  FileStream->Write(&PdbBookInfo[0], length);



   Write_Chapters:

      for (i = 1; i <= ChapterCount; i++) {
         if (!LoadChapter(i)) continue;
	      if (ConvertToBig5) ScanBufferOut();
		   TBytes outBuffer = WideBytesOf(Buffer);
         if (ConvertToBig5) {
			   TEncoding *Encoding = NULL;
			   TEncoding::GetBufferEncoding(outBuffer, Encoding);
			   outBuffer = TEncoding::Convert(TEncoding::Unicode,
			   Encoding->GetEncoding(950), outBuffer);
			   int size = outBuffer.Length + 1;
			   outBuffer.set_length(size);
			   outBuffer[size - 1] = 0;
			   }
		    FileStream->Write(&outBuffer[0], outBuffer.Length);
		    }

   Free_Big_Table:

     if (Big5txtLoaded) {
        Big5txtLoaded = false;
        FreePRList();
        }

   Write_Bookmark_Tail:

      char tail[2];
      p = -1;
      ptr = (char*) &p;
      tail[0] = ptr[1];
      tail[1] = ptr[0];
	   FileStream->Write(&tail[0], 2);

   }


/*---------------------------*/
/* THome::fldSetupFileChange */
/*---------------------------*/

void __fastcall THome::fldSetupFileChange(TObject *Sender) {

   if (SetupFile.Length()) SetupChanged = true;
   SetupFile = fldSetupFile->Text;
   Caption = ProgramTitle;
   if (SetupFile.Length()) {
	  Caption = Caption + " - " + SetupFile;
	  }

   if (!SavingSetupFile) LoadSetupFile();
   else {
      SavingSetupFile = false;
      SaveSetupFile();
      }

   }


/*---------------------------*/
/* THome::fldDirectoryChange */
/*---------------------------*/

void __fastcall THome::fldDirectoryChange(TObject *Sender) {

   Directory = fldDirectory->Text;
   int length = Directory.Length();
   if (length > 0 && Directory[length] == '\\') {
      Directory.SetLength(length-1);
      }
   SetupFileChanged = true;

   }


/*------------------------------*/
/* THome::fldChapterCountChange */
/*------------------------------*/

void __fastcall THome::fldChapterCountChange(TObject *Sender) {

   if (fldChapterCount->Text.IsEmpty()) ChapterCount = 0;
   else ChapterCount = fldChapterCount->Text.ToInt();
   SetupFileChanged = true;

   }


/*------------------------*/
/* THome::fldAuthorChange */
/*------------------------*/

void __fastcall THome::fldAuthorChange(TObject *Sender) {

   Author = fldAuthor->Text;
   SetupFileChanged = true;

   }


/*-----------------------*/
/* THome::fldTitleChange */
/*-----------------------*/

void __fastcall THome::fldTitleChange(TObject *Sender) {

   Title = fldTitle->Text;
   PdbName = Title;
   SetupFileChanged = true;

   }


/*----------------*/
/* THome::LoadIni */
/*----------------*/

void __fastcall THome::LoadIni() {

   UnicodeString name = ExePath + IniStr;
   if (!FileExists(name)) return;

   Buffer.SetLength(0);
   LoadFile(name);
   TStringList* list = new TStringList();
   list->Text = Buffer;

   name = list->Values[L"�]�w��"];
   fldSetupFile->Text = name;
   Memo1->Font->Name = list->Values[L"�r��"];
   Memo1->Font->Size = list->Values[L"�r��"].ToInt();
   int bold = list->Values[L"����"].ToInt();
   if (bold) Memo1->Font->Style = TFontStyles()<< fsBold;

   delete list;

   }


/*----------------*/
/* THome::SaveIni */
/*----------------*/

void __fastcall THome::SaveIni() {

   if (!SetupFile.Length()) return;
   if (!SetupChanged) return;

   TStringList* list = new TStringList();
   list->Values[L"�]�w��"] = SetupFile;
   list->Values[L"�r��"] = Memo1->Font->Name;
   list->Values[L"�r��"] = Memo1->Font->Size;
   list->Values[L"����"] = Memo1->Font->Style.Contains(fsBold) ? 1 : 0;

   UnicodeString name = ExePath + IniStr;
   try { list->SaveToFile(name, TEncoding::Unicode); } catch (EFOpenError&) { }
   delete list;

   }


/*----------------------*/
/* THome::LoadSetupFile */
/*----------------------*/

void __fastcall THome::LoadSetupFile() {

   if (!SetupFile.Length()) return;
   if (!FileExists(SetupFile)) return;

   Buffer.SetLength(0);
   LoadFile(SetupFile);
   TStringList* list = new TStringList();
   list->Text = Buffer;

   ChapterCount = 0;
   if (list->Count) {
	  if (list->Strings[0][1] == 'D') {
		 Directory = list->Values[L"Directory"];
		 Title = list->Values[L"Title"];
		 ChapterCount = list->Values[L"ChapterCount"].ToInt();
		 ChapterNumber = 1;
		 }
	  else {
		 Directory = list->Values[L"�ؿ�"];
       Author = list->Values[L"�@��"];
		 Title = list->Values[L"�ѦW"];
		 ChapterCount = list->Values[L"����"].ToInt();
		 UnicodeString str = list->Values[L"��"];
		 if (str.IsEmpty()) ChapterNumber = 1;
			else ChapterNumber = str.ToInt();
		 }
	  if (ChapterNumber <= 0) ChapterNumber = 1;
	  else if (ChapterNumber > ChapterCount) ChapterNumber = ChapterCount;
//	  int length = Directory.Length();
//     if (length == 0) {
	     int x = SetupFile.LastDelimiter("\\");
	     if (x > 0) {
           Directory = SetupFile;
		     Directory.SetLength(x-1);
		     }
//        }
//	  else if (Directory[length] == '\\') Directory.SetLength(length-1);

	  UnicodeString edit = fldEditFile->Text;
	  fldEditFile->Text = Directory + '\\' + ChapterNumber + ".txt";
	  if (edit == fldEditFile->Text) LoadEditFile();
	  ChapterNumberInitial = ChapterNumber;
	  }
   fldDirectory->Text = Directory;
   fldTitle->Text = Title;
   fldAuthor->Text = Author;
   fldChapterCount->Text = ChapterCount;

   delete list;

   SetupFileChanged = false;
//   SetupFileSaved.SetLength(0);
   EditFileSaved.SetLength(0);
   EditFilePrev.SetLength(0);
   EditFileTemp.SetLength(0);
   PREditFilePrev.SetLength(0);
   PREditFileTemp.SetLength(0);
   CNEditFilePrev.SetLength(0);
   CNEditFileTemp.SetLength(0);
   PRFile.SetLength(0);
   PRList = NULL;
   PdbName = Title;

   }


/*----------------------*/
/* THome::SaveSetupFile */
/*----------------------*/

void __fastcall THome::SaveSetupFile() {

   if (SetupFile.IsEmpty()) return;

   if (ChapterNumber != ChapterNumberInitial) SetupFileChanged = true;
   if (!SetupFileChanged) return;

   TStringList* list = new TStringList();
//   list->Values[L"�ؿ�"] = Directory;
   list->Values[L"�@��"] = Author;
   list->Values[L"�ѦW"] = Title;
   list->Values[L"����"] = ChapterCount;
   list->Values[L"��"] = ChapterNumber;

   try { list->SaveToFile(SetupFile, TEncoding::Unicode); } catch (EFOpenError&) { }
   delete list;

   SetupFileChanged = false;

   }


/*--------------------*/
/* THome::ValidateINF */
/*--------------------*/

int __fastcall THome::ValidateINF(UnicodeString& name) {

   int x = name.Length();
   if (!x) return false;

   int d = name.LastDelimiter(".");
   if (d < 1) {
//	  ShowAlert(L" �S�����ɮצW");
//     return false;
     name = name + ".inf";
     return true;
	  }
   if (d == 1) {
	  ShowAlert(L" �S���ɮצW");
	  return false;
	  }
   UnicodeString ext = name.SubString(d + 1, x - d);
   ext = ext.LowerCase();
   if (ext == "inf") return true;
   name = name + L" ���O�nŪ�����ɳ]�w�ɡI";
   ShowAlert(name);
   return false;

   }

//
// PDB functions
//
TFileStream* PDBFile;
TBytes PDBChapter;


//HANDLE PDBHandle;


int   PDBChapterLength;
//int   PDBChapterSize;
int   PDBBookLength;
int   PDBCharIndex;
int   PDBRecordLength;
short PDBRecordCount;
#define MAX_PDB_LIST 4000


/*---------------------*/
/* THome::GetPDBString */
/*---------------------*/

int __fastcall THome::GetPDBString() {

   if (PDBCharIndex >= PDBRecordLength) return PDBRecordLength - 1;

   int position = PDBCharIndex;
   int i = 0;
   while (PDBCharIndex < PDBRecordLength) {
	  char c = PDBChapter[PDBCharIndex++];
	  if (c == 27) {
		 if (Big5) break;
		 if (PDBCharIndex == PDBRecordLength) break;
		 if (PDBChapter[PDBCharIndex] == 0) break;
		 }
     else if (c == 0) { // 2011/4/13
       if (Big5) break;
       }
	  Bytes[i++] = c;
	  }
   return i;

   }


/*---------------------*/
/* THome::GetPDBNumber */
/*---------------------*/

int __fastcall THome::GetPDBNumber() {

   if (PDBCharIndex >= PDBRecordLength) return 0;

   int number = 0;
   while (PDBCharIndex < PDBRecordLength) {
	  char c = PDBChapter[PDBCharIndex++];
	  if (c == 27) {
		 if (Big5) break;
		 if (PDBCharIndex == PDBRecordLength) break;
		 if (PDBChapter[PDBCharIndex] == 0) break;
		 }
      c = c - '0';
      if (0 <= c && c <= 9) {
         number = number * 10 + c;
         }
      }
   return number;

   }


/*---------------------------*/
/* THome::GetPDBRecordOffset */
/*---------------------------*/

int __fastcall THome::GetPDBRecordOffset(int i) {

   i = i * 8;
   char* ptr = &PdbList[i];
   unsigned char c = ptr[0];
   int offset = c;
   for (int k = 1; k <= 3; k++) {
      c = ptr[k];
      offset = offset * 256 + c;
      }
   return offset;

   }


/*-----------------------*/
/* THome::LoadPDBChapter */
/*-----------------------*/

int __fastcall THome::LoadPDBChapter() {

   PDBChapterLength = 0;
   int number = ChapterNumber + 1;
   int offset1 = GetPDBRecordOffset(ChapterNumber);
   int offset2 = GetPDBRecordOffset(number);
   if (number == PDBRecordCount) offset2 = PDBBookLength;

   if (offset1 > PDBBookLength) return false;
   if (offset2 <= offset1) return false;

   PDBChapterLength = offset2 - offset1 - 1;
   if (!Big5) PDBChapterLength++;
   if (PDBChapterLength <= 0) return false;
   int result = PDBFile->Seek(offset1, 0);
   if (result != offset1) return false;

   if (ChapterNumber == 0) if (Big5) PDBChapterLength++;  // To include terminating 0.
   if (ChapterNumber == ChapterCount + 1) PDBChapterLength++; // Bookmarks
   PDBChapter.set_length(PDBChapterLength);
   PDBRecordLength = PDBChapterLength;

   int read = PDBFile->Read(&PDBChapter[0], PDBChapterLength);
   if (read != PDBChapterLength) { PDBChapterLength = 0; return false; }
   return true;

   }


/*-----------------------*/
/* THome::SavePDBChapter */
/*-----------------------*/

void __fastcall THome::SavePDBChapter() {

   TEncoding *Encoding = NULL;
   UnicodeString name = Directory + '\\' + ChapterNumber + ".txt";
   std::auto_ptr<TFileStream> FileStream(new TFileStream(name, fmCreate));
   FileStream->Write(&BOM[0], BOM.Length);

   TEncoding::GetBufferEncoding(PDBChapter, Encoding);
   if (Big5) PDBChapter = TEncoding::Convert(Encoding->GetEncoding(950),
	  TEncoding::Unicode, PDBChapter);
   FileStream->Write(&PDBChapter[0], PDBChapter.Length);

   }


/*-------------------*/
/* THome::LoadAuthor */
/*-------------------*/

void __fastcall THome::LoadAuthor(char* header) {

   Author = "";
   if (header[35] == 1) return;

   Bytes.set_length(32);
   int i = 0;
   int j = 0;
   while (i < 32) {
	  char c = header[i++];
	  if (c == 0) {
		 if (Big5) break;
		 if (header[i] == 0) break;
		 }
	  Bytes[j++] = c;
	  }

   if (j == 0) return;
   if (!Big5) Bytes.set_length(j);
   else {
	   TEncoding *Encoding = NULL;
	   TEncoding::GetBufferEncoding(Bytes, Encoding);
	   Bytes = TEncoding::Convert(Encoding->GetEncoding(950),
			TEncoding::Unicode, Bytes, 0, j);
      }

   Author = TEncoding::Unicode->GetString(Bytes);

   }


/*--------------------*/
/* THome::LoadPDBBook */
/*--------------------*/

int __fastcall THome::LoadPDBBook(UnicodeString name) {

   Get_Book:

	  PDBFile = new TFileStream(name, fmOpenRead);
	  if (!PDBFile) return false;

   Get_Book_Size:

	  PDBBookLength = PDBFile->Size;
     if (PDBBookLength <= 0) return false;

   Get_Record_Count:

	  char header[80];
	  int read = PDBFile->Read(header, 76);
	  if (read != 76) return false;

	  if (header[64] != 'M') return false;
	  if (header[65] != 'T') return false;
	  if (header[66] != 'I') return false;
	  if (header[67] == 'T') Big5 = true;
	  else if (header[67] == 'U') Big5 = false;
	  else return false;

	  unsigned char temp[2];
	  read = PDBFile->Read(temp, 2);
	  if (read != 2) return false;

   Get_Pdb_Record_List:

	  PDBRecordCount = temp[0] * 256 + temp[1];
	  int count = PDBRecordCount * 8;
	  if (count > MAX_PDB_LIST) return false;
	  read = PDBFile->Read(PdbList, count);
	  if (read != count) return false;

   Get_Book_Info:

	  ChapterNumber = 0;
//	  PDBChapterSize = 0;
	  if (!LoadPDBChapter()) return false;

	  PDBCharIndex = 8;
	  Bytes.set_length(PDBChapterLength);
	  int size = GetPDBString();
	  if (size == 0) return false;
	  if (!Big5) Bytes.set_length(size);
	  else {
		 TEncoding *Encoding = NULL;
		 TEncoding::GetBufferEncoding(Bytes, Encoding);
		 Bytes = TEncoding::Convert(Encoding->GetEncoding(950),
			TEncoding::Unicode, Bytes, 0, size);
		 }
	  Title = TEncoding::Unicode->GetString(Bytes);
     LoadAuthor(header);
	  GetPDBString();   // Load Info
	  GetPDBString();   // Load Secret
	  ChapterCount = GetPDBNumber();
     return true;

   }


/*------------------------*/
/* THome::ExportTextFiles */
/*------------------------*/

void __fastcall THome::ExportTextFiles() {

   Export_Text_Files:

	  for (ChapterNumber = 1; ChapterNumber <= ChapterCount; ChapterNumber++) {
		 if (LoadPDBChapter()) SavePDBChapter();
       }

   Export_Text_0:

	  ChapterNumber = 0;
	  if (!LoadPDBChapter()) return;

	  PDBCharIndex = 8;
	  Bytes.set_length(PDBChapterLength);
	  for (int k = 0; k < 4; k++) GetPDBString();
	  ++PDBCharIndex;
	  int size = PDBChapterLength - PDBCharIndex;
	  if (!size) return;

	  UnicodeString name = Directory + "\\0.txt";
	  std::auto_ptr<TFileStream> FileStream(new TFileStream(name, fmCreate));
	  FileStream->Write(&BOM[0], BOM.Length);

	  if (!Big5) {
		 FileStream->Write(&PDBChapter[PDBCharIndex], size);
		 return;
		 }

	  UnicodeString newln = L"\r\n";
     TEncoding *Encoding = NULL;
	  TEncoding::GetBufferEncoding(PDBChapter, Encoding);
	  PDBCharIndex--;
	  for (int i = 1; i <= ChapterCount; i++) {
		 size = GetPDBString();
		 if (size > 0) {
			Bytes = TEncoding::Convert(Encoding->GetEncoding(950),
			   TEncoding::Unicode, Bytes, 0, size);
			FileStream->Write(&Bytes[0], Bytes.Length);
			Bytes.set_length(PDBChapterLength);
			}
		 if (i < ChapterCount) FileStream->Write(&newln.w_str()[0], 4);
		 }

   }


/*---------------------*/
/* THome::btnOpenClick */
/*---------------------*/

void __fastcall THome::btnOpenClick(TObject *Sender) {

   CheckModified();
   TOpenDialog* dialog = new TOpenDialog(this);
   dialog->Title = L"�}�ҦnŪ�s�ѳ]�w�ɡA�ΦnŪ����";
   dialog->FileName = fldSetupFile->Text;
   dialog->Filter = L"�s�ѳ]�w�� (*.inf)�A�nŪ���� (*.pdb,*.updb)|*.INF;*.PDB;*.UPDB";

   bool ret = dialog->Execute();
   UnicodeString name;
   if (ret) name = dialog->FileName;
   delete dialog;
   if (!ret) return;

   if (!FileExists(name)) {
	  name = L" �䤣��" + name;
	  ShowAlert(name);
	  return;
	  }

   int x = name.Length();
   int d = name.LastDelimiter(".");
      if (d < 1) {
	  ShowAlert(L" �S�����ɮצW");
	  return;
	  }
   if (d == 1) {
	  ShowAlert(L" �S���ɮצW");
	  return;
	  }
   UnicodeString ext = name.SubString(d + 1, x - d);
   ext = ext.LowerCase();
   if (ext == "inf") {
	  fldSetupFile->Text = name;
	  SetupFileChanged = true;
	  return;
	  }
   if ((ext == "pdb") || (ext == "updb")) {
	  UnicodeString msg = L"�^�� " + name + L" �����¤�r�ɡH";
	  if (ConfirmAlert(msg) != IDOK) return;
	  x = x + 1;
	  if (!LoadPDBBook(name)) return;

	  x = name.LastDelimiter("\\");
	  if (x > 0) {
		 name[x] = 0;
		 Directory = name;
		 }

	  fldDirectory->Text = Directory;
	  fldTitle->Text = Title;
     fldAuthor->Text = Author;
	  fldChapterCount->Text = ChapterCount;

	  ExportTextFiles();

	  ChapterNumber = 1;
	  UnicodeString edit = fldEditFile->Text;
	  fldEditFile->Text = Directory + '\\' + ChapterNumber + ".txt";
	  if (edit == fldEditFile->Text) LoadEditFile();
	  ChapterNumberInitial = ChapterNumber;

	  name = Directory + '\\' + Title + ".inf";
	  SavingSetupFile = true;
	  fldSetupFile->Text = name;

	  delete PDBFile;
      return;
      }

   name = name + L" ���O�nŪ���s�ѳ]�w�ɡA�]���O���ɡI";
   ShowAlert(name);

   }


/*---------------------*/
/* THome::btnSaveClick */
/*---------------------*/

void __fastcall THome::btnSaveClick(TObject *Sender) {

   CheckModified();
   if (!ValidateSetupData()) return;
   if (!SetupFile.Length()) btnSaveAsClick(Sender);
   else SaveSetupFile();

   }


/*-----------------------*/
/* THome::btnSaveAsClick */
/*-----------------------*/

void __fastcall THome::btnSaveAsClick(TObject *Sender) {

   CheckModified();
   if (!ValidateSetupData()) return;
   TOpenDialog* dialog = new TSaveDialog(this);
   dialog->Title = L"�t�s�nŪ�s�ѳ]�w��";
   dialog->FileName = fldSetupFile->Text;
   dialog->Filter = L"�nŪ�s�ѳ]�w�� (*.inf)|*.INF";
   if (dialog->Execute()) {
      UnicodeString name = dialog->FileName;
      if (ValidateINF(name)) {
         SavingSetupFile = true;
         SetupFileChanged = true;
         fldSetupFile->Text = name;
         int x = name.LastDelimiter("\\");
         name[x] = 0;
         Directory = name;
	      fldDirectory->Text = Directory;
         }
      }
   delete dialog;

   }


/*-------------------------*/
/* THome::btnPrevBookClick */
/*-------------------------*/

void __fastcall THome::btnPrevBookClick(TObject *Sender) {

   CheckModified();
   int length = SetupFile.Length();
   if (length < 5) return;

   AnsiString temp = SetupFile;
   char digit = temp[length - 4];
   if (!('0' <= digit && digit <= '9')) return;
   digit--;
   if (digit >= '1') temp[length - 4] = digit;
   else {
      char c = temp[length - 5];
      if (!('1' <= c && c < '9')) return;
      if (digit == '0') temp[length - 4] = '0';
      else {
         temp[length - 4] = '9';
         if (c >= '2') temp[length - 5] = --c;
         else temp.Delete(length - 5, 1);
         }
      }

   fldSetupFile->Text = temp;

   }


/*-------------------------*/
/* THome::btnNextBookClick */
/*-------------------------*/

void __fastcall THome::btnNextBookClick(TObject *Sender) {

   CheckModified();
   int length = SetupFile.Length();
   if (length < 5) return;

   AnsiString temp = SetupFile;
   char digit = temp[length - 4];
   if (!('0' <= digit && digit <= '9')) return;
   digit++;
   if (digit <= '9') temp[length - 4] = digit;
   else {
      temp[length - 4] = '0';
      char c = temp[length - 5];
      if ('1' <= c && c < '9') temp[length - 5] = ++c;
      else temp.Insert("1", length - 4);
      }
   if (!FileExists(temp)) {
      temp[length - 4] = '1';
      char c = temp[length - 5];
      if ('0' <= c && c <= '9') temp.Delete(length - 5, 1);
      }

   fldSetupFile->Text = temp;

   }


/*-----------------------*/
/* THome::LogReplacement */
/*-----------------------*/

void __fastcall THome::LogReplacement(int j, int length, UnicodeString target) {

   int size = Buffer.Length();
   int head_count = 0;
   if (j > 1) head_count = 2;
   else if (j > 0) head_count = 1;
   int tail_count = 0;
   if (j + length < size) tail_count = 2;
   else if (j + length < size - 1) tail_count = 1;

   UnicodeString source;
   source = Buffer.SubString(j - head_count, head_count + length + tail_count);
   UnicodeString replacement;
   if (head_count > 0) replacement = source.SubString(1, head_count);
   replacement = replacement + target;
   if (tail_count > 0)
      replacement = replacement + source.SubString(head_count + length + 1, tail_count);

   UnicodeString temp;
   temp.sprintf(L"%s/%s\r\n", source.w_str(), replacement.w_str());
   TBytes out = WideBytesOf(temp);

   UnicodeString name = Directory + L"\\log.txt";
   if (!FileExists(name)) {
	  std::auto_ptr<TFileStream> FileStream(new TFileStream(name, fmCreate));
	  FileStream->Write(&BOM[0], BOM.Length);
	  FileStream->Write(&out[0], out.Length);
	  }
   else {
	  std::auto_ptr<TFileStream> FileStream(new TFileStream(name, fmOpenWrite));
	  FileStream->Position = FileStream->Size;
     FileStream->Write(&out[0], out.Length);
	  }

   }

 /*--------------------------*/
/* THome::LogBig5Replacement */
/*---------------------------*/

void __fastcall THome::LogBig5Replacement(int j, int length, UnicodeString target) {

   int size = Buffer.Length();
   int head_count = 0;
   if (j > 1) head_count = 2;
   else if (j > 0) head_count = 1;
   int tail_count = 0;
   if (j + length < size) tail_count = 2;
   else if (j + length < size - 1) tail_count = 1;

   UnicodeString source;
   source = Buffer.SubString(j - head_count, head_count + length + tail_count);
   UnicodeString replacement;
   if (head_count > 0) replacement = source.SubString(1, head_count);
   replacement = replacement + target;
   if (tail_count > 0)
      replacement = replacement + source.SubString(head_count + length + 1, tail_count);

   UnicodeString temp;
   temp.sprintf(L"%s/%s\r\n", source.w_str(), replacement.w_str());
   TBytes out = WideBytesOf(temp);

   UnicodeString name = Directory + L"\\log-big5.txt";
   if (!FileExists(name)) {
	  std::auto_ptr<TFileStream> FileStream(new TFileStream(name, fmCreate));
	  FileStream->Write(&BOM[0], BOM.Length);
	  FileStream->Write(&out[0], out.Length);
	  }
   else {
	  std::auto_ptr<TFileStream> FileStream(new TFileStream(name, fmOpenWrite));
	  FileStream->Position = FileStream->Size;
     FileStream->Write(&out[0], out.Length);
	  }

   }

/*---------------------*/
/* THome::btnExitClick */
/*---------------------*/

void __fastcall THome::btnExitClick(TObject *Sender) {

   Close();

   }


/*---------------------*/
/* THome::LoadEditFile */
/*---------------------*/

void __fastcall THome::LoadEditFile() {

   Buffer.SetLength(0);
   if (EditFile.Length())
	  if (FileExists(EditFile)) {
		 LoadFile(EditFile);
		 ScanBufferIn();
		 }

   Memo1->Text = Buffer;
   Memo1->Modified = false;

   }


/*---------------------*/
/* THome::SaveEditFile */
/*---------------------*/

void __fastcall THome::SaveEditFile() {

   if (!EditFile.Length()) return;

   std::auto_ptr<TFileStream> FileStream(new TFileStream(EditFile, fmCreate));
   FileStream->Write(&BOM[0], BOM.Length);
   TBytes buf = WideBytesOf(Memo1->Text);
   FileStream->Write(&buf[0], buf.Length);
   Memo1->Modified = false;

   }


/*-----------------------------*/
/* THome::btnSaveEditFileClick */
/*-----------------------------*/

void __fastcall THome::btnSaveFileClick(TObject *Sender) {

   SaveEditFile();

   }


/*-------------------------*/
/* THome::SetChapterNumber */
/*-------------------------*/

void __fastcall THome::SetChapterNumber() {

   UnicodeString name = fldEditFile->Text;
   if (name.IsEmpty()) return;
   int length = name.Length();
   name.Delete(1, Directory.Length());
   if (name[1] == '\\') name.Delete(1, 1);
   name.SetLength(name.Length() - 4);
   length = name.Length();
   for (int i = 1; i <= length; i++) {
      if (name[i] < '0') return;
      if (name[i] > '9') return;
      }
   ChapterNumber = name.ToInt();

   }


/*-------------------------*/
/* THome::btnEditFileClick */
/*-------------------------*/

void __fastcall THome::btnEditFileClick(TObject *Sender) {

   CheckModified();
   TOpenDialog* dialog = new TOpenDialog(this);
   dialog->Title = L"��ܤ�r��";
   dialog->FileName = fldEditFile->Text;
   dialog->Filter = L"��r�� (*.txt)|*.TXT";
   if (dialog->Execute()) {
      if (!EditFile.Compare(dialog->FileName)) LoadEditFile();
      fldEditFile->Text = dialog->FileName;
      if (SetupFile.Length()) SetChapterNumber();
      }
   delete dialog;    

   }


/*--------------------------*/
/* THome::fldEditFileChange */
/*--------------------------*/

void __fastcall THome::fldEditFileChange(TObject *Sender) {

   EditFile = fldEditFile->Text;
   SetChapterNumber();
   LoadEditFile();
   SetupFileChanged = true;

   }


/*-----------------------*/
/* THome::btnSwitchClick */
/*-----------------------*/

void __fastcall THome::btnSwitchClick(TObject *Sender) {

   PlateModeSwitched = 1 - PlateModeSwitched;
   TButton* button = (TButton*) Sender;
   if (PlateModeSwitched) button->Caption = L"�r�L";
   else button->Caption = L"�]�w";
   DrawPlateLine();

   }


/*----------------------*/
/* THome::btnInsertChar */
/*----------------------*/

void __fastcall THome::btnInsertChar(TObject *Sender) {

   TButton* button = (TButton*) Sender;
   if (PlateModeSwitched) {
      wchar_t buffer[101];
      Memo1->GetSelTextBuf(buffer, 101);
      button->Caption = buffer;
      SavePlate();
      return;
      }
   Memo1->SetFocus();
   Memo1->SelText = button->Caption;

   }


/*--------------------*/
/* THome::ReplaceChar */
/*--------------------*/

void __fastcall THome::ReplaceChar() {

   int length1 = SourceStr.Length();
   int length2 = TargetStr.Length();
   if (length1 == 0) return;

   int next = ExcludeNext.Length();
   int prev = ExcludePrev.Length();

//   GetChapterSizes();
   int total = 0;
   for (int i = ChapterNumber; i <= ChapterCount; i++) {
      int changed = 0;
      int start = 1;
      int index = 0;
      if (ResumePosition >= 0 && ChapterNumber == i) {
         Buffer = Memo1->Text;
         start = ResumePosition;
		   if (start < 1) start = 1;
         }
      else {
	      if (!LoadChapter(i)) continue;
		   ResumePosition = 0;
		   if (SearchOnly || !AutoReplace) {
		      Memo1->Text = Buffer;
			   Memo1->Refresh();
			   Memo1->Modified = false;
			   fldEditFile->Text = EditChapter;
			   fldEditFile->Refresh();
			   }
		   }
      for (int j = start; j <= Buffer.Length(); j++) {
		 if (j + length1 - 1 > Buffer.Length()) break;
		 int k = 0;
		 while (k < length1) {
			if (Buffer[j+k] != SourceStr[k + 1]) break;
            k++;
            }
		 if (k < length1) continue;
/*
         if (next) {
            char c = Buffer[j+2];
            char d = Buffer[j+3];
            int found = false;
            for (int i = 1; i <= next; i++) {
                if (c == ExcludeNext[i] && d == ExcludeNext[i+1]) {
                   found = true;
                   break;
                   }
                i++;
                }
            if (found) continue;
            }

         if (prev) {
            char c = Buffer[j-2];
            char d = Buffer[j-1];
            int found = false;
            for (int i = 1; i <= prev; i++) {
                if (c == ExcludePrev[i] && d == ExcludePrev[i+1]) {
                   found = true;
                   break;
                   }
                i++;
                }
            if (found) continue;
            }
*/

		 int answer = IDYES;
		 if (SearchOnly) answer = IDCANCEL;
		 if (SearchOnly || !AutoReplace) {
          Memo1->SelStart = j - 1;
          Memo1->SelLength = length1;

          EditFilePrev = EditFileTemp;
          SelStartPrev = SelStartTemp;
          SelLengthPrev = SelLengthTemp;

          EditFileTemp = EditFile;
          SelStartTemp = Memo1->SelStart;
          SelLengthTemp = Memo1->SelLength;

          if (!AutoReplace && !SearchOnly) {
			   UnicodeString msg;
			   msg.sprintf(L"�N %s ���� %s?", SourceStr.w_str(), TargetStr.w_str());
			   answer = ChoiceAlert(msg);
			   }
			}
		 if (answer == IDYES) {
			if (!AutoReplace) Memo1->SetSelTextBuf(TargetStr.w_str());
			   else total++;
			LogReplacement(j, length1, TargetStr);
			changed++;
			Buffer.Delete(j, length1);
			if (length2) {
			   Buffer.Insert(TargetStr, j);
			   j = j + length2 - 1;
			   }
			}
		 else if (answer == IDNO) continue; // Do not replace
		 else {
          if (changed) { // Cancel
			   SaveChapter(i);
            changed = false;
			   Memo1->Modified = false;
			   }
			return;
         }
		 }

	  if (changed) {
		 SaveChapter(i);
		 Memo1->Modified = false;
		 if (AutoReplace) {
			if (i == ChapterNumberStart) Memo1->Text = Buffer;
			}
		 }
	  }

   if (AutoReplace) {
      ChapterNumber = ChapterNumberStart;
	   UnicodeString msg;
      msg.sprintf(L"���N�F %d �B�C", total);
	   ShowAlert(msg);
	   }

   }


/*-----------------------*/
/* THome::btnReplaceChar */
/*-----------------------*/

void __fastcall THome::btnReplaceChar(TObject *Sender) {
/*
   CheckModified();
   TButton* button = (TButton*) Sender;
   Memo1->SetFocus();
   AnsiString& caption = button->Caption;

   int pos = button->Hint.Pos("//");
   if (pos == 0) {
      ExcludeNext = button->Hint;
      ExcludePrev.SetLength(0);
      }
   else {
      ExcludeNext = button->Hint.SubString(1, pos - 1);
      ExcludePrev = button->Hint.SubString(pos + 2, button->Hint.Length() - pos - 1);
      }

   int i = caption.AnsiPos("/");
   if (i == 0) return;

   SourceStr = caption.SubString(1, i-1);
   TargetStr = caption.SubString(i+1, caption.Length() - i);

   if (ChapterCount > 0) {
      ChapterNumber = 1;
      ResumePosition = -1;
      }

   ReplacePrompt = true;
   ReplaceChar();
*/
   }


/*---------------------------*/
/* THome::btnSearchDownClick */
/*---------------------------*/

void __fastcall THome::btnSearchDownClick(TObject *Sender) {

   CheckModified();
   Memo1->SetFocus();

   SourceStr = fldSource->Text;
   TargetStr = fldTarget->Text;

   if (ChapterCount > 0) {
	   ChapterNumberStart = ChapterNumber;
	   ChapterNumber = 1;
      ResumePosition = -1;
      }

   ExcludeNext.SetLength(0);
   ExcludePrev.SetLength(0);
   SearchOnly = (cbSearchOnly->Checked) ? true : false;
   AutoReplace = (cbAutoReplace->Checked) ? true : false;

   ReplaceChar();

   }


/*-------------------------*/
/* THome::btnSearchUpClick */
/*-------------------------*/

void __fastcall THome::btnSearchUpClick(TObject *Sender) {

   CheckModified();
   if (SetupFile.IsEmpty()) return;
   if (EditFilePrev.IsEmpty()) return;

   EditFile = EditFilePrev;
   fldEditFile->Text = EditFile;

   Memo1->SelStart = SelStartPrev;
   Memo1->SelLength = SelLengthPrev;

   }


/*-----------------------*/
/* THome::btnResumeClick */
/*-----------------------*/

void __fastcall THome::btnResumeClick(TObject *Sender) {

   CheckModified();
   if (SetupFile.IsEmpty()) return;
   if (fldEditFile->Text.IsEmpty()) return;

   SourceStr = fldSource->Text;
   TargetStr = fldTarget->Text;

   if (SourceStr.IsEmpty()) return;

   SearchOnly = (cbSearchOnly->Checked) ? true : false;
   AutoReplace = (cbAutoReplace) ? false : true;

   ResumePosition = Memo1->SelStart + 1;
   if (Memo1->SelLength > 0) ResumePosition++;
   ReplaceChar();

   }


/*----------------------*/
/* THome::PreviewWindow */
/*----------------------*/
int LeftbtnSave;
int LeftbtnExit;
int LeftSwitch;
int LeftTabControl2;
int LeftTabControl3;
int LeftPanel;

void __fastcall THome::PreviewWindow() {

//   LeftbtnSave = btnSave->Left;
//   btnSave->Left = 2;
//   btnSave->Hide();

   LeftbtnExit = btnExit->Left;
   btnExit->Left = 2;
   btnExit->Hide();

   if (PlateNumber > 0) {
      LeftSwitch = btnSwitch->Left;
      btnSwitch->Left= 2;
      btnSwitch->Hide();
      LeftTabControl2 = TabControl2->Left;
      TabControl2->Left= 2;
      TabControl2->Hide();
      }

   LeftTabControl3 = TabControl3->Left;
   TabControl3->Left = 2;
   TabControl3->Hide();

   btnSetupFile->Hide();
//   btnDirectory->Hide();
//   btnPrevBook->Hide();
//   btnNextBook->Hide();
//   Button1->Hide();
//   btnSave->Hide();
//   fldSetupFile->Hide();
//   fldDirectory->Hide();
   fldTitle->Hide();
   fldAuthor->Hide();
   fldChapterCount->Hide();

   if (PlateNumber == 0) {
      LeftPanel = Panel1->Left;
      Panel1->Left = 2;
      Panel1->Hide();
      }
   else if (PlateNumber == 1) {
      LeftPanel = Panel1->Left;
      Panel2->Left = 2;
      Panel2->Hide();
      }
   else if (PlateNumber == 2) {
      LeftPanel = Panel1->Left;
      Panel3->Left = 2;
      Panel3->Hide();
      }
   else if (PlateNumber == 3) {
      LeftPanel = Panel1->Left;
      Panel4->Left = 2;
      Panel4->Hide();
      }

   Panel5->Hide();

   if (!Panel) InitPanel();
   Memo1->Hide();
   Width = PreviewWidth;
   Panel->ShowChapter(false);
   Panel->Show();
   Panel->SetFocus();

   }


/*--------------------*/
/* THome::RestoreMemo */
/*--------------------*/

void __fastcall THome::RestoreMemo() {

   Preview = false;

   Width = WindowWidth;

//   btnSave->Left = LeftbtnSave;
//   btnSave->Show();

   btnExit->Left = LeftbtnExit;
   btnExit->Show();

   if (PlateNumber > 0) {
      btnSwitch->Left = LeftSwitch;
      btnSwitch->Show();
      TabControl2->Left= LeftTabControl2;
      TabControl2->Show();
      }

   TabControl3->Left = LeftTabControl3;
   TabControl3->Show();

   btnSetupFile->Show();
//   btnDirectory->Show();
//   btnPrevBook->Show();
//   btnNextBook->Show();
//   Button1->Show();
//   btnSave->Show();
//   fldSetupFile->Show();
//   fldDirectory->Show();
   fldTitle->Show();
   fldAuthor->Show();
   fldChapterCount->Show();

   if (PlateNumber == 0) {
      Panel1->Left = LeftPanel;
      Panel1->Show();
      }
   else if (PlateNumber == 1) {
      Panel2->Left = LeftPanel;
      Panel2->Show();
      }
   else if (PlateNumber == 2) {
      Panel3->Left = LeftPanel;
      Panel3->Show();
      }
   else if (PlateNumber == 3) {
      Panel4->Left = LeftPanel;
      Panel4->Show();
      }

   Panel5->Show();

   Panel->Hide();
   Memo1->Show();
   Panel->SetSelStart();

   }


/*------------------------*/
/* THome::btnPreviewClick */
/*------------------------*/

void __fastcall THome::btnPreviewClick(TObject *Sender) {

   TButton* button = (TButton*) Sender;
   Preview = 1- Preview;
   if (!Preview) {
      RestoreMemo();
      return;
      }

   WindowWidth = Width;
   WindowHeight = Height;
   PreviewWindow();

   }


/*--------------------*/
/* THome::PrevChapter */
/*--------------------*/

void __fastcall THome::PrevChapter(int last) {

   CheckModified();
   if (ChapterCount == 0) return;
   if (ChapterNumber == 0) ChapterNumber = 1;
   else {
	  ChapterNumber--;
	  if (ChapterNumber < 1) ChapterNumber = ChapterCount;
	  }

   UnicodeString name = Directory + L"\\" + ChapterNumber + L".txt";
   if (!EditFile.Compare(name)) LoadEditFile();
   fldEditFile->Text = name;

   if (Preview) {
	  if (last) Panel->ShowChapter(last);
	  else {
		 Panel->ShowChapter(false);
		 Panel->ShowPage(0);
		 }
	  }

   }


/*----------------------------*/
/* THome::btnPrevChapterClick */
/*----------------------------*/

void __fastcall THome::btnPrevChapterClick(TObject *Sender) {

   PrevChapter(false);

   }


/*--------------------*/
/* THome::NextChapter */
/*--------------------*/

void __fastcall THome::NextChapter() {

   CheckModified();
   if (ChapterCount == 0) return;
   if (ChapterNumber == 0) ChapterNumber = 1;
   else {
	  ChapterNumber++;
	  if (ChapterNumber > ChapterCount) ChapterNumber = 1;
	  }

   UnicodeString name = Directory + L"\\" + ChapterNumber + L".txt";
   if (!EditFile.Compare(name)) LoadEditFile();
   fldEditFile->Text = name;

   if (Preview) {
	  Panel->ShowChapter(false);
	  Panel->ShowPage(0);
	  }

   }


/*----------------------------*/
/* THome::btnNextChapterClick */
/*----------------------------*/

void __fastcall THome::btnNextChapterClick(TObject *Sender) {

   NextChapter();

   }


/*--------------------------*/
/* THome::btnDirectoryClick */
/*--------------------------*/

void __fastcall THome::btnDirectoryClick(TObject *Sender) {

   UnicodeString dir;
   UnicodeString caption = L"��ܥؿ�";
   if (!SelectDirectory(caption, "", dir)) return;

   fldDirectory->Text = dir;

   }


/*--------------------------*/
/* THome::ValidateSetupData */
/*--------------------------*/

int __fastcall THome::ValidateSetupData() {

   if (fldAuthor->Text.IsEmpty()) {
	  ShowAlert(L"�п�J�@��");
	  fldAuthor->SetFocus();
	  return false;
	  }

   if (fldTitle->Text.IsEmpty()) {
	  ShowAlert(L"�п�J�ѦW");
	  fldTitle->SetFocus();
	  return false;
	  }

   if (!ChapterCount) {
	  ShowAlert(L"�п�J����");
	  fldChapterCount->SetFocus();
	  return false;
	  }
/*
   if (fldDirectory->Text.IsEmpty()) {
	  ShowAlert(L"�п�J�ؿ�");
	  fldDirectory->SetFocus();
	  return false;
	  }
*/
   return true;

   }


/*----------------------*/
/* THome::btnPasteClick */
/*----------------------*/

void __fastcall THome::btnPasteClick(TObject *Sender) {

   int length = Memo1->SelLength;
   if (!length) return;

   wchar_t buffer[101];
   Memo1->GetSelTextBuf(buffer, 101);
   fldSource->Text = buffer;
   fldTarget->Text = buffer;

   btnSavePositionClick(Sender);

   }


/*-----------------------------*/
/* THome::btnSavePositionClick */
/*-----------------------------*/

void __fastcall THome::btnSavePositionClick(TObject *Sender) {

//   SetupFileSaved = SetupFile;
   EditFileSaved = EditFile;
   SelStartSaved = Memo1->SelStart;
   SelLengthSaved = Memo1->SelLength;
   ChapterNumberSaved = ChapterNumber;

   }


/*--------------------------------*/
/* THome::btnRestorePositionClick */
/*--------------------------------*/

void __fastcall THome::btnRestorePositionClick(TObject *Sender) {

   CheckModified();
//   if (SetupFileSaved.IsEmpty()) return;
//   if (SetupFile.Compare(SetupFileSaved)) return;

   if (EditFileSaved.IsEmpty()) return;
   if (ChapterNumberSaved == 0) return;

   EditFile = EditFileSaved;
   fldEditFile->Text = EditFile;

   Memo1->SelStart = SelStartSaved;
   Memo1->SelLength = SelLengthSaved;
   ChapterNumber = ChapterNumberSaved;

   }


/*---------------------*/
/* THome::ConvertMarks */
/*---------------------*/

bool __fastcall THome::ConvertMarks() {
Char source[29] = L"�u�v�y�z�m�n�]�^�i�j�a�b�e�f�q�r�Тw���������U�ա֡X������";
Char target[29] = L"�w�x�{�|�o�p�_�`�k�l�c�d�g�h�s�t�W�W�w�x�{�|�W�s�t�W������";

   int changed = 0;
   int head;  // "xx" -> �wxx�x
   int head1; // 'xx' -> �{xx�|
   head = 1;
   for (int j = 1; j <= Buffer.Length(); j++) {
	   Char c = Buffer[j];
	   if (c > 255) { // Chinese character
         if (c == L'�K') {
            Buffer[j] = 0xFE19; //0xFFE4;  2014/8/15
            changed++;
            continue;
            }
         for (int k = 0; k < 29; k++) {
	         if (c == source[k]) {
		         Buffer[j] = target[k];
               changed++;
               break;
               }
			   }
			continue;
			}
		if (c == '"') {
			if (head) Buffer[j] = target[0]; else Buffer[j] = target[1];
			head = 1 - head;
			changed++;
			continue;
			}
		if (c == '\'') {
			if (head1) Buffer[j] = target[2]; else Buffer[j] = target[3];
			head1 = 1 - head1;
			changed++;
			continue;
			}
		if (c == '\n') {
		   head = 1;
			head1 = 1;
			continue;
			}
      if (c == '(') {
         Buffer[j] = target[6];
		   changed++;
			continue;
			}
      if (c == ')') {
		   Buffer[j] = target[7];
			changed++;
			continue;
			}
		if (c == '-') {
		   Buffer[j] = target[16];
			changed++;
			continue;
			}
		}

   return changed;

   }


/*-----------------------------*/
/* THome::btnConvertMarksClick */
/*-----------------------------*/

void __fastcall THome::btnConvertMarksClick(TObject *Sender) {

   CheckModified();
   if (!ValidateSetupData()) return;
   if (ConfirmAlert(L"�ন�������I�Ÿ��H") != IDOK) return;

   int number = ChapterNumber;
   for (int i = 0; i <= ChapterCount; i++) {
	   if (!LoadChapter(i)) continue;
	   if (ConvertMarks()) {
         SaveChapter(i);
         if (i == number) Memo1->Text = Buffer;
		   }
	   }

   ChapterNumber = number;
   ShowAlert(L"�������I�Ÿ���n�F�I");

   }


/*-----------------------------*/
/* THome::btnExamineMarksClick */
/*-----------------------------*/

void __fastcall THome::btnExamineMarksClick(TObject *Sender) {
Char pair[32] = L"�w�{�o�_�k�c�g�s�u�y�m�]�i�a�e�q�x�|�p�`�l�d�h�t�v�z�n�^�j�b�f�r";
int count[16];

   CheckModified();
   if (!ValidateSetupData()) return;
   if (ConfirmAlert(L"�ˬd���I�Ÿ��H") != IDOK) return;

   int MemoChapterNumber = ChapterNumber;
//   for (int n = 0; n < 8; n++) count[n] = 0;
   int start;
   int head;
   for (int i = MemoChapterNumber; i <= ChapterCount; i++) {
      start = 1;
      if (i == MemoChapterNumber) {
         Buffer = Memo1->Text;
         start = Memo1->SelStart + 1;
         if (start < 1) start = 1;
         }
    	else if (!LoadChapter(i)) continue;
      for (int k = 0; k < 16; k++) count[k] = 0;
      head = start;
      for (int j = start; j <= Buffer.Length(); j++) {
         Char c = Buffer[j];
	      if (c <= 255) {
            if (c == '\n') {
               for (int k = 0; k < 16; k++) {
                  if (count[k] != 0) {
                     if (MemoChapterNumber != ChapterNumber) {
                        UnicodeString name = Directory + L"\\" + ChapterNumber + L".txt";
                        fldEditFile->Text = name;
                        Memo1->Text = Buffer;
                        Memo1->Refresh();
                        Memo1->Modified = false;
                        }
                     Memo1->SelStart = head - 1;
                     Memo1->SelLength = j-head+1;
                     ShowAlert(pair[k]);
                     return;
                     }
                  }
               head = j+1;
               }
            continue;
            }
         for (int k = 0; k < 32; k++) {
            if (c != pair[k]) continue;
            if (k < 16) {
               count[k]++;
               break;
               }
            k -= 16;
            if (count[k] > 0) {
               count[k]--;
               break;
               }
//            ShowAlert(ChapterNumber);
            if (MemoChapterNumber != ChapterNumber) {
//               ShowAlert(L"Change Chapter");
               UnicodeString name = Directory + L"\\" + ChapterNumber + L".txt";
               fldEditFile->Text = name;
               Memo1->Text = Buffer;
               Memo1->Refresh();
               Memo1->Modified = false;
               }
            Memo1->SelStart = j - 1;
            Memo1->SelLength = 1;
            ShowAlert(c);
            return;
            }
 			}
		}

   ChapterNumber = MemoChapterNumber;
   ShowAlert(L"���I�Ÿ��ˬd�n�F�I");

   }


/*------------------------*/
/* THome::btnConvertClick */
/*------------------------*/

void __fastcall THome::btnConvertClick(TObject *Sender) {

   CheckModified();
   if (!ValidateSetupData()) return;

   UnicodeString msg;
   if (ConvertToBig5) msg = L"�ন�nŪ pdb �� (Big5)�H";
      else msg = L"�ন�nŪ updb �� (Unicode)�H";
   if (ConfirmAlert(msg) != IDOK) return;

   int number = ChapterNumber;
   SetPdbHeader();
   SetPdbBookInfo();
   WritePdb();
   ChapterNumber = number;

   if (ConvertToBig5) ShowAlert(L"�ন�nŪpdb�ɤF�I");
      else ShowAlert(L"�ন�nŪupdb�ɤF�I");

   }


/*---------------------*/
/* THome::btnFontClick */
/*---------------------*/

void __fastcall THome::btnFontClick(TObject *Sender) {

   TFontDialog* dialog = new TFontDialog(this);
   dialog->Font->Name = Memo1->Font->Name;
   dialog->Font->Size = Memo1->Font->Size;
   dialog->Font->Style = Memo1->Font->Style;
   if (!dialog->Execute()) return;
   Memo1->Font->Name = dialog->Font->Name;
   Memo1->Font->Size = dialog->Font->Size;
   Memo1->Font->Style = dialog->Font->Style;
   Panel->SetPageFont();
   SetupChanged = true;

   }


/*--------------------------------*/
/* THome::btnRemoveSoftBreakClick */
/*--------------------------------*/

void __fastcall THome::btnRemoveSoftBreakClick(TObject *Sender) {

   CheckModified();
   if (!ValidateSetupData()) return;
//   if (!GetChapterSizes()) return;
   if (ConfirmAlert(L"�R���q�������_��H") != IDOK) return;

   int number = ChapterNumber;
   UnicodeString space = L"�@";
   for (int i = 1; i <= ChapterCount; i++) {
	  if (!LoadChapter(i)) continue;
	  int changed = false;
	  int new_paragraph = false;
	  for (int j = 1; j <= Buffer.Length(); j++) {
		 Char c = Buffer[j];
		 if (c == '\r') continue;
		 if (c == '\n') {
			 new_paragraph = true;
			 continue;
			 }
		 if (!new_paragraph) continue;
		 if ((c != ' ') && (c != space[1]) && (c != '\t')) {
			Buffer.Delete(j - 2, 2);
			j = j - 1;
			changed++;
			}
		 new_paragraph = false;
		 }
	  if (changed) {
		 SaveChapter(i);
		 if (i == number) Memo1->Text = Buffer;
		 }
	  }

   ChapterNumber = number;
   ShowAlert(L"�q�������_��R���F�I");

   }


/*-------------------------*/
/* THome::btnAddSpaceClick */
/*-------------------------*/

void __fastcall THome::btnAddSpaceClick(TObject *Sender) {

   CheckModified();
   if (!ValidateSetupData()) return;
//   if (!GetChapterSizes()) return;
   if (ConfirmAlert(L"���j�q���H") != IDOK) return;

   int number = ChapterNumber;
   UnicodeString space = L"\r\n";
   for (int i = 1; i <= ChapterCount; i++) {
	  int changed = false;
	  if (!LoadChapter(i)) continue;
	  for (int j = 1; j <= Buffer.Length(); j++) {
		 if (j + 2 > Buffer.Length()) break;
		 if (Buffer[j] == '\r' && Buffer[j+1] == '\n') {
            if (Buffer[j+2] == '\r') { j = j + 3; continue; }
            else if (Buffer[j-1] == '\n') { j = j + 3; continue; }
            else {
               j = j + 2;
               Buffer.Insert(space, j);
               changed++;
               j++;
               }
            }
         }
	  if (changed) {
		 SaveChapter(i);
		 if (i == number) Memo1->Text = Buffer;
		 }
	  }

   ChapterNumber = number;
   ShowAlert(L"�q�����j�F�I");

   }


 /*---------------------------*/
/* THome::btnRemoveSpaceClick */
/*----------------------------*/

void __fastcall THome::btnRemoveSpaceClick(TObject *Sender) {

   CheckModified();
   if (!ValidateSetupData()) return;
   if (ConfirmAlert(L"�R������H") != IDOK) return;

   int number = ChapterNumber;
   int size;
   int count = 0;
   for (int i = 1; i <= ChapterCount; i++) {
	  int changed = false;
	  if (!LoadChapter(i)) continue;
	  for (int j = 1; j <= Buffer.Length(); j++) {
		 if (j + 3 > Buffer.Length()) break;
		 if (Buffer[j] == '\r' && Buffer[j+1] == '\n') {
			if (Buffer[j+2] == '\r' && Buffer[j+3] == '\n') {
			   Buffer.Delete(j, 2);
			   changed = true;
			   }
			}
		 }
	  if (changed) {
		 SaveChapter(i);
		 if (i == number) Memo1->Text = Buffer;
		 }
	  }

   ChapterNumber = number;
   ShowAlert(L"����R���F�I");

   }


/*-----------------------------*/
/* THome::btnRemoveWhitesClick */
/*-----------------------------*/

void __fastcall THome::btnRemoveWhitesClick(TObject *Sender) {

   CheckModified();
   if (!ValidateSetupData()) return;
//   if (!GetChapterSizes()) return;
   if (ConfirmAlert(L"�R���ťզr���H") != IDOK) return;

   UnicodeString space2 = L"�@�@";
   int number = ChapterNumber;
   for (int i = 0; i <= ChapterCount; i++) {
	  if (!LoadChapter(i)) continue;
	  int changed = false;
	  int size = 0;
	  changed = Buffer.Length();
	  int chinese = true;
	  for (int j = 1; j < Buffer.Length(); j++) {
		 size++;
		 Char c = Buffer[j];
		 if (c == '\t') { // Replace Tab with two spaces
			Buffer.Delete(j, 1);
			Buffer.Insert(space2, j);
			changed++;
			j++;
			continue;
			}
		 if (c == '\r') {
			while (size >= 2) { // Remove trailing spaces of the previous line
			   if (Buffer[j-1] == L'�@') {
				  j--;
				  Buffer.Delete(j, 1);
				  size--;
				  changed++;
				  continue;
				  }
			   break;
			   }
			chinese = true;
			size = 0;
			continue;
			}
		 if (c == '\n') { chinese = true; size = 0; continue; }
		 if (c != ' ') {
			if (('a'<= c && c <='z') || ('A'<= c && c <='Z')) chinese = false;
			else chinese = true;
			continue;
			}
		 if (!chinese) continue;
		 Buffer.Delete(j, 1);
		 j--;
		 changed++;
		 }
	  size = Buffer.Length();
	  int cut = 0;
	  for (int j = size; j > 1; j--) {
		 Char c = Buffer[j];
		 if (c == '\n' || c == '\r') {
			cut++;
			continue;
			}
		 break;
		 }
	  if (cut) {
		  Buffer.SetLength(size - cut);
		  changed++;
		  }
	  if (changed) {
		 SaveChapter(i);
		 if (i == number) Memo1->Text = Buffer;
		 }
	  }

   ChapterNumber = number;
   ShowAlert(L"�ťզr���R���F�I");

   }


/*-----------------------*/
/* THome::btnIndentClick */
/*-----------------------*/

void __fastcall THome::btnIndentClick(TObject *Sender) {

   CheckModified();
   if (!ValidateSetupData()) return;
//   if (!GetChapterSizes()) return;
   if (ConfirmAlert(L"�Y�Ƭq���H") != IDOK) return;

   int number = ChapterNumber;
   UnicodeString space2 = L"�@�@";
   UnicodeString space1 = L"�@";
   for (int i = 1; i <= ChapterCount; i++) {
	  int changed = false;
	  if (!LoadChapter(i)) continue;
	  int insert = 0;
	  for (int j = 1; j <= Buffer.Length(); j++) {
		 Char c = Buffer[j];
		 if (c == '\r') continue;
		 if (c == '\n') {
			 insert = 2;
			 continue;
			 }
		 if (insert == 0) continue;
		 if (c == space1[1]) {
			if (insert > 0) insert--;
			continue;
			}
		 if (insert == 2) Buffer.Insert(space2, j++);
		 else Buffer.Insert(space1, j);
		 j++;
		 changed++;
		 insert = 0;
		 }
	  if (changed) {
		 SaveChapter(i);
		 if (i == number) Memo1->Text = Buffer;
		 }
	  }

   ChapterNumber = number;
   ShowAlert(L"�q���Y�ƤF�I");

   }


/*--------------------------------*/
/* THome::btnProduceContentsClick */
/*--------------------------------*/

void __fastcall THome::btnProduceContentsClick(TObject *Sender) {

   CheckModified();
   if (!ValidateSetupData()) return;
   if (ConfirmAlert(L"���ͥؿ��H") != IDOK) return;

   UnicodeString name = Directory + L"\\0.txt";
   TFileStream* FileStream = new TFileStream(name, fmCreate);
   FileStream->Write(&BOM[0], BOM.Length);

   int number = ChapterNumber;
   for (int i = 1; i <= ChapterCount; i++) {
	   if (!LoadChapter(i)) {
         FileStream->Write(L"\r\n", 4);
         continue;
         }
	   int changed = false;
	   UnicodeString title;
	   title.SetLength(0);
	   for (int j = 1; j <= Buffer.Length(); j++) {
		   if (Buffer[j] != '\r') continue;
		   title = Buffer.SubString(1, j-1);
		   if ((j + 4 <= Buffer.Length()) && (Buffer[j+1] == '\n')) {
			   if (Buffer[j+2] != '\r') {
			      Buffer.Insert(L"\r\n\r\n", j+2);
			      changed++;
			      }
			   else if (Buffer[j+3] == '\n' && Buffer[j+4] != '\r') {
			      Buffer.Insert(L"\r\n", j+2);
			      changed++;
			      }
			   }
		   break;
		   }
	   TBytes out = WideBytesOf(title);
	   FileStream->Write(&out[0], out.Length);
	   if (i < ChapterCount) FileStream->Write(L"\r\n", 4);
         if (changed) {
		      SaveChapter(i);
		      if (i == number) Memo1->Text = Buffer;
		      }
       }

   delete FileStream;
   ChapterNumber = number;
   fldEditFile->Text = name;
   ShowAlert(L"�ؿ� 0.txt ���ͤF�I");

   }


/*-----------------*/
/* ToChineseNumber */
/*-----------------*/

void __fastcall THome::ToChineseNumber(UnicodeString source, UnicodeString& target) {

   Char n[10] = { L'�s', L'�@', L'�G', L'�T', L'�|',
	  L'��', L'��', L'�C', L'�K', L'�E'};
   Char u[11] = { L'�d', L'��', L'�Q', L'��', L'�d', L'��', L'�Q', L'�U',
	  L'�d', L'��', L'�Q' };

   int length = source.Length();
   if (!length) return;
   if (length > 14) return;

   for (int x = 1; x <= length; x++) { // Remove commas
	  Char c = source[x];
	  if (c == L'.') break;
	  if (c == L',') {
		 source.Delete(x, 1);
		 length--;
		 x--;
		 }
	  }

   int unit = 0;
   int zero = true;
   UnicodeString buffer = source;
   for (int x = length; x >= 1; x--) {
	  unit++;
	  if (buffer[x] != L'0') { zero = false; continue; }
	  if (unit == 5) { // �U
		 buffer[x] = L'W';
		 zero = true;
		 if ((length >= 9)
			&& (buffer[x - 1] == '0')
			&& (buffer[x - 2] == '0')
			&& (buffer[x - 3] == '0'))
			buffer[x] = (buffer[x + 1] == '0') ? L'B' : L'0';
		 else if (buffer[x + 1] != '0') buffer[x] = L'Z';
		 }
	  else if (unit == 9) buffer[x] = L'I'; // ��
	  else if (zero) buffer[x] = L'B';
	  else zero = true;
	  }

   for (int x = length - 1; x >= 1; x--) {
	  if (buffer[x] == L'B') continue;
	  if (buffer[x] == L'Z') { buffer[x] = L'W'; break; }
	  break;
	  }

   int k = 0;
   if (length > 12) length = 12;
   target.SetLength(24);
   for (int i = 1; i <= length; i++) {
	  Char c = buffer[i];
	  if (c == L'W') target[++k] = u[7];
	  else if (c == L'Z') target[++k] = u[7];
	  else if (c == L'I') target[++k] = u[3];
	  else if (L'0' <= c && c <= L'9') {
		 int d = c - L'0';
		 int e = 11 - length + i;
		 if (d) {
			if (d != 1) target[++k] = n[d];
			else if (i != 1) target[++k] = n[d];
			else if (length != 2 && length != 6 && length != 10)
			   target[++k] = n[d];
			if (e < 11) target[++k] = u[e];
			}
		 else if (!SkipChineseZero) target[++k] = n[d];
		 }
	  }

   target.SetLength(k);

   }


/*---------------*/
/* ToChineseYear */
/*---------------*/

void __fastcall THome::ToChineseYear(UnicodeString source, UnicodeString& target) {

   Char n[10] = { L'��', L'�@', L'�G', L'�T', L'�|',
	  L'��', L'��', L'�C',L'�K', L'�E'};

   int length = source.Length();
   if (!length) return;
   if (length > 9) return;

   target.SetLength(length);
   for (int i = 1; i <= length; i++) {
	  Char c = source[i];
	  int d = source[i] - L'0';
	  target[i] = n[d];
      }

   }


/*------------------------------*/
/* THome::btnConvertNumberClick */
/*------------------------------*/

void __fastcall THome::btnConvertNumberClick(TObject *Sender) {

   CheckModified();
   if (!ValidateSetupData()) return;
   if (ConfirmAlert(L"�ഫ�Ʀr�H") != IDOK) return;

   Char year = L'�~';
   Char to = L'�U';
   UnicodeString edit_saved = EditFile;
   int start_saved = Memo1->SelStart;
   int length_saved = Memo1->SelLength;
   int number_saved = ChapterNumber;

   int Prompt = true;
   if (cbAutoReplace->Checked) Prompt = false;

   int found = false;
   int count = 0;
   UnicodeString source;
   for (int i = ChapterNumber; i <= ChapterCount; i++) {
      int changed = false;
      int start = 1;
      if (ChapterNumber == i) {
         Buffer = Memo1->Text;
         start = Memo1->SelStart + 1;
         if (start < 1) start = 1;
         }
      else {
         if (!LoadChapter(i)) continue;
         Memo1->Text = Buffer;
         Memo1->Refresh();
         Memo1->Modified = false;
         fldEditFile->Text = EditChapter;
         fldEditFile->Refresh();
		   }
	   for (int j = start; j <= Buffer.Length(); j++) {
         Char c = Buffer[j];
         if (L'0' <= c && c <= L'9') {
            count = 0;
            source.SetLength(++count);
            source[count] = c;
            while (j + count < Buffer.Length()) {
               c = Buffer[j + count];
               if (L'0' <= c && c <= L'9') {
                  source.SetLength(++count);
                  source[count] = c;
                  continue;
                  }
               break;
               }
            int answer = IDYES;

			   Memo1->SelStart = j - 1;
	    		Memo1->SelLength = count;

            CNEditFilePrev = CNEditFileTemp;
            CNSelStartPrev = CNSelStartTemp;
            CNSelLengthPrev = CNSelLengthTemp;

            CNEditFileTemp = EditFile;
            CNSelStartTemp = Memo1->SelStart;
            CNSelLengthTemp = Memo1->SelLength;

			   UnicodeString target = source;
		   	if (count == 4 && ((year == Buffer[j + count]) ||
			      (to == Buffer[j + count])))
			      ToChineseYear(source, target);
		    	else ToChineseNumber(source, target);

			   UnicodeString msg;
            if (Prompt) {
               msg.sprintf(L"�N %s ���� %s?", source.w_str(), target.w_str());
		      	answer = ChoiceAlert(msg);
               }
		   	if (answer == IDYES) {
	   		   Memo1->SetSelTextBuf(target.w_str());
	   		   LogReplacement(j, count, target);
		   	   changed++;
		    	   Buffer.Delete(j, count);
               int length = target.Length();
			      if (length) {
                  CNSelLengthTemp = length;
		            Buffer.Insert(target, j);
	               j = j + length - 1;
                  }
			      }
			   else if (answer == IDNO) { j = j + count - 1; continue; }
			   else {
			   	if (changed) { // Cancel
			         SaveChapter(i);
	               changed = false;
		            Memo1->Modified = false;
				      }
			      return;
			      }
	         }
         }

	   if (changed) {
         SaveChapter(i);
         Memo1->Modified = false;
		   }
	   }

   Restore_Position:

	  if (ConfirmAlert(L"�ഫ�Ʀr���F�I��^��B�H") != IDOK) return;

      EditFile = edit_saved;
      fldEditFile->Text = EditFile;

      Memo1->SelStart = start_saved;
      Memo1->SelLength = length_saved;
	   ChapterNumber = number_saved;
      Memo1->SetFocus();

   }


/*--------------------------*/
/* THome::PrevConvertNumber */
/*--------------------------*/

void __fastcall THome::PrevConvertNumber(TObject *Sender) {

   CheckModified();
   if (SetupFile.IsEmpty()) return;
   if (CNEditFilePrev.IsEmpty()) return;

   EditFile = CNEditFilePrev;
   fldEditFile->Text = EditFile;

   Memo1->SelStart = CNSelStartPrev;
   Memo1->SelLength = CNSelLengthPrev;

   }




/*-----------*/
/* LoadPlate */
/*-----------*/

void __fastcall THome::LoadPlate() {

   if (PlateNumber == 0) return;
   if (PlatePageLoaded[PlateNumber] == FlipPageNumber) return;

   UnicodeString plate = "PDB";
   UnicodeString page = "Pages";
   UnicodeString name = ExePath + plate[PlateNumber] + page[FlipPageNumber] + L".txt";
   if (FileExists(name)) {
	  Buffer.SetLength(0);
	  LoadFile(name);
	  TStringList* list = new TStringList();
	  list->Text = Buffer;
      if (PlateNumber == 1) {
         for (int i = 0; i < 72; i++)
            Plate1Button[i]->Caption = list->Strings[i];
         }
      else if (PlateNumber == 2) {
         for (int i = 0; i < 72; i++)
            Plate2Button[i]->Caption = list->Strings[i];
         }
      else if (PlateNumber == 3) {
         for (int i = 0; i < 48; i++)
            Plate3Button[i]->Caption = list->Strings[i];
         }
      delete list;
      }
   else {
      if (PlateNumber == 1) {
         for (int i = 0; i < 72; i++)
            Plate1Button[i]->Caption = "";
         if (FlipPageNumber == 1) {
			Char str[] = L"�A�C�G�w�x�H�B�F�W�{�|�I���@�G�T�|���D�@���C�K�E�Q��";
			int j = 0;
			for (int k = 0; k < 26; k++) {
			   Plate1Button[j]->Caption = str[k];
			   if (k == 5 || k == 11) j += 13;
			   else if (k == 18) j += 12;
			   else j++;
			   }
			}
		 }
	  else if (PlateNumber == 2) {
		 for (int i = 0; i < 72; i++)
			Plate2Button[i]->Caption = "";
		 }
	  else if (PlateNumber == 3) {
		 for (int i = 0; i < 48; i++)
			Plate3Button[i]->Caption = "";
		 }
	  }

   PlatePageLoaded[PlateNumber] = FlipPageNumber;

   }


/*-----------*/
/* SavePlate */
/*-----------*/

void __fastcall THome::SavePlate() {

   if (PlateNumber == 0) return;

   TStringList* list = new TStringList();
   if (PlateNumber == 1) {
	  for (int i = 0; i < 72; i++)
		 list->Add(Plate1Button[i]->Caption);
	  }
   else if (PlateNumber == 2) {
	  for (int i = 0; i < 72; i++)
		 list->Add(Plate2Button[i]->Caption);
	  }
   else if (PlateNumber == 3) {
	  for (int i = 0; i < 48; i++)
		 list->Add(Plate3Button[i]->Caption);
	  }

   UnicodeString plate = "PDB";
   UnicodeString page = "Pages";
   UnicodeString name = ExePath + plate[PlateNumber] + page[FlipPageNumber] + L".txt";
   try { list->SaveToFile(name, TEncoding::Unicode); } catch (EFOpenError&) { }
   delete list;

   PlatePageLoaded[PlateNumber] = FlipPageNumber;

   }


/*---------------*/
/* LayoutButtons */
/*---------------*/

void __fastcall THome::LayoutButtons() {

   if (PlateNumber == 0) return;

   if (PlateNumber == 1) {
      if (Plate1ButtonsCreated) return;
      CreateButtons();
      int x = 1;
      int y = 155;
      int i = 0;
      for (int j = 0; j < 4; j++) {
         for (int k = 0; k < 18; k++) {
            TButton* button = Plate1Button[i++];
            button->Left = x;
            button->Top = y;
            button->Width = 30;
            if (k == 5 || k == 11) y += 4;
            y += 25;
            }
         x += 33;
         y = 155;
         }
      }
   else if (PlateNumber == 2) {
      if (Plate2ButtonsCreated) return;
      CreateButtons();
      int x = 1;
      int y = 1;
      int i = 0;
      for (int j = 0; j < 3; j++) {
         for (int k = 0; k < 24; k++) {
            TButton* button = Plate2Button[i++];
            button->Left = x;
            button->Top = y;
            button->Width = 41;
            if (k == 5 || k == 11 || k == 17) y += 4;
            y += 25;
            }
         x += 44;
         y = 1;
         }
      }
   else if (PlateNumber == 3) {
      if (Plate3ButtonsCreated) return;
      CreateButtons();
      int x = 1;
      int y = 1;
      int i = 0;
      for (int j = 0; j < 2; j++) {
         for (int k = 0; k < 24; k++) {
            TButton* button = Plate3Button[i++];
            button->Left = x;
            button->Top = y;
            button->Width = 63;
            if (k == 5 || k == 11 || k == 17) y += 4;
            y += 25;
            }
         x += 66;
         y = 1;
         }
      }

   }


/*-----------------------*/
/* MPanel::DrawPlateLine */
/*-----------------------*/

void __fastcall THome::DrawPlateLine() {

   int width = Screen->Width;
   int height = Screen->Height;
   if (!TabControl2->Visible) return;
   Canvas->Pen->Color = (PlateModeSwitched) ? clRed : clBtnFace;
   int left = Panel1->Left - 1;
   Canvas->MoveTo(left, Panel1->Top);
   Canvas->LineTo(left, Panel1->Top + Panel1->Height);

   }


/*-------------------*/
/* TabControl2Change */
/*-------------------*/

void __fastcall THome::TabControl2Change(TObject *Sender) {

   FlipPageNumber = TabControl2->TabIndex + 1;
   LoadPlate();

   }


/*-------------------*/
/* TabControl3Change */
/*-------------------*/

void __fastcall THome::TabControl3Change(TObject *Sender) {

   PlateNumber = TabControl3->TabIndex;
   if (PlateNumber == 0) {
      if (Panel2->Visible) Panel2->Hide();
      if (Panel3->Visible) Panel3->Hide();
      if (Panel4->Visible) Panel4->Hide();
      if (!Panel1->Visible) Panel1->Show();
	  if (btnSwitch->Visible) btnSwitch->Hide();
      if (TabControl2->Visible) TabControl2->Hide();
      }
   else {
      LayoutButtons();
      if (PlatePageLoaded[PlateNumber] == 0) {
         FlipPageNumber = 1;
         TabControl2->TabIndex = 0;
         }
      else {
         FlipPageNumber = PlatePageLoaded[PlateNumber];
         TabControl2->TabIndex = PlatePageLoaded[PlateNumber] - 1;
         }
      LoadPlate();
      if (PlateNumber == 1) {
         if (Panel1->Visible) Panel1->Hide();
         if (Panel3->Visible) Panel3->Hide();
         if (Panel4->Visible) Panel4->Hide();
         if (!Panel2->Visible) Panel2->Show();
         }
      else if (PlateNumber == 2) {
         if (Panel1->Visible) Panel1->Hide();
         if (Panel2->Visible) Panel2->Hide();
         if (Panel4->Visible) Panel4->Hide();
         if (!Panel3->Visible) Panel3->Show();
         }
      else if (PlateNumber == 3) {
         if (Panel1->Visible) Panel1->Hide();
         if (Panel2->Visible) Panel2->Hide();
         if (Panel3->Visible) Panel3->Hide();
         if (!Panel4->Visible) Panel4->Show();
         }
      if (!TabControl2->Visible) TabControl2->Show();
      if (!btnSwitch->Visible) btnSwitch->Show();
      }
   }


/*---------------*/
/* CheckModified */
/*---------------*/

void __fastcall THome::CheckModified() {

   if (!Memo1->Modified) return;

   UnicodeString msg;
   msg.sprintf(L"%s ���ܤF�A�n�x�s�ܡH", EditFile);
   if (YesNoAlert(msg) == IDYES) SaveEditFile();

   }


/*------------------*/
/* THome::FormPaint */
/*------------------*/

void __fastcall THome::FormPaint(TObject *Sender) {

   DrawPlateLine();

   }


/*-----------------------*/
/* THome::FormCloseQuery */
/*-----------------------*/

void __fastcall THome::FormCloseQuery(TObject *Sender, bool &CanClose) {

   if (ConfirmAlert(L"���� mPDB�H") != IDOK) {
      CanClose = false;
      return;
      }
   CanClose = true;
   CheckModified();
   if (!SetupFileChanged) { SaveIni(); return; }
   int ret = ChoiceAlert(L"�]�w���e���ܤF�A�O�_�n�x�s�H");
   if (ret == IDNO) return;
   if (ret == IDCANCEL) {
      CanClose = false;
      return;
      }
   btnSaveClick(Sender);
   SaveIni();

   }






/*--------------------------*/
/* THome::btnSetupFileClick */
/*--------------------------*/

void __fastcall THome::btnSetupFileClick(TObject *Sender) {

   PopupMenu1->Popup(Left + 30, Top + 58);

   }


/*-------------------------*/
/* THome::btnContentsClick */
/*-------------------------*/

void __fastcall THome::btnContentsClick(TObject *Sender) {

   CheckModified();
   UnicodeString name = Directory + L"\\0.txt";
   ChapterNumber = 0;
   fldEditFile->Text = name;

   }


/*---------------------*/
/* THome::NewSetupFile */
/*---------------------*/

void __fastcall THome::NewSetupFile(TObject *Sender) {

   if (ConfirmAlert(L"�s�W�s�ѳ]�w�ɡH") != IDOK) return;
   CheckModified();
   fldDirectory->Text = "";
   fldAuthor->Text= "�п�J�@��";
   fldTitle->Text= "�п�J�ѦW";
   fldChapterCount->Text= "1";
   fldSetupFile->Text = "updb.inf";
   fldEditFile->Text = "";
   btnSaveAsClick(Sender);
   LoadSetupFile();

   }


/*--------------------------*/
/* THome::cbSearchOnlyClick */
/*--------------------------*/

void __fastcall THome::cbSearchOnlyClick(TObject *Sender) {

   if (cbSearchOnly->Checked) {
	  if (cbAutoReplace->Checked) cbAutoReplace->Checked = false;
	  if (fldTarget->Visible) fldTarget->Visible = false;
	  if (labelReplace->Visible) labelReplace->Visible = false;
	  }
   else {
	  if (!fldTarget->Visible) fldTarget->Visible = true;
	  if (!labelReplace->Visible) labelReplace->Visible = true;
	  }

}


/*---------------------------*/
/* THome::cbAutoReplaceClick */
/*---------------------------*/

void __fastcall THome::cbAutoReplaceClick(TObject *Sender) {

   if (cbAutoReplace->Checked) {
	  if (cbSearchOnly->Checked) cbSearchOnly->Checked = false;
	  }
   }


/*---------------*/
/* THome::Backup */
/*---------------*/

void __fastcall THome::Backup(TObject *Sender) {

   CheckModified();
   if (ConfirmAlert(L"�ƥ��Ҧ��� txt �ɡH") != IDOK) return;
   UnicodeString target = Directory + L"\\�ƥ�\\";
   if (!DirectoryExists(target)) CreateDir(target);

   TSearchRec sr;
   UnicodeString name = Directory + "\\*.txt";
   int count = 0;
   if ((FindFirst(name, faAnyFile, sr)) == 0) {
      do {
         UnicodeString name1 = Directory + "\\" + sr.Name;
         UnicodeString name2 = target + sr.Name;
         CopyFileW(name1.w_str(), name2.w_str(), false);
         count++;
         }
      while (FindNext(sr) == 0);
      FindClose(sr);
      }

   UnicodeString msg;
   msg.sprintf(L"�ƥ��F %d �� txt �ɡC", count);
	ShowAlert(msg);

   }


/*----------------------*/
/* THome::RestoreBackup */
/*----------------------*/

void __fastcall THome::RestoreBackup(TObject *Sender) {

   if (ConfirmAlert(L"�٭�ƥ��H") != IDOK) return;
   UnicodeString source = Directory + L"\\�ƥ�\\";
   if (!DirectoryExists(source)) {
      ShowAlert(L"�L�ƥ��ɡI");
      return;
      }

   TSearchRec sr;
   UnicodeString name = source + "*.txt";
   int count = 0;
   if ((FindFirst(name, faAnyFile, sr)) == 0) {
      do {
         UnicodeString name1 = source + sr.Name;
         UnicodeString name2 = Directory + "\\" + sr.Name;
         CopyFileW(name1.w_str(), name2.w_str(), false);
         count++;
         }
      while (FindNext(sr) == 0);
      FindClose(sr);
      }

   UnicodeString msg;
   msg.sprintf(L"�٭�F %d �� txt �ɡC", count);
	ShowAlert(msg);

   }


/*------------------*/
/* THome::ProofRead */
/*------------------*/

void __fastcall THome::ProofRead() {

   UnicodeString edit_saved = EditFile;
   int start_saved = Memo1->SelStart;
   int length_saved = Memo1->SelLength;
   int number_saved = ChapterNumber;

   for (int i = ChapterNumber; i <= ChapterCount; i++) {
	   int start = 1;
      int first = 0;
	   if (PRResumePosition >= 0 && ChapterNumber == i) {
         Buffer = Memo1->Text;
         start = PRResumePosition;
         first = PRResumeIndex;
//      UnicodeString msg;
//      msg.sprintf(L"PR Index: %d Start: %d", first, start);
//	   ShowAlert(msg);
         if (Memo1->SelLength > 0) start++;
         if (start < 1) start = 1;
         }
      else {
         if (!LoadChapter(i)) continue;
         PRResumePosition = 0;
         Memo1->Text = Buffer;
         Memo1->Refresh();
         Memo1->Modified = false;
         fldEditFile->Text = EditChapter;
         fldEditFile->Refresh();
         }

	  int changed = false;
	  int skip = false;
     int prompt = true;
	  for (int d = first; d < PRSource->Count; d++) {
        UnicodeString source = PRSource->Strings[d];
        if (source[1] == L'~') {
           prompt = false;
           source.Delete(1, 1);
           }
        else prompt = true;
        int length1 = source.Length();
        UnicodeString target = PRTarget->Strings[d];
        int length2 = target.Length();
        if (d != first) start = 1;
        for (int j = start; j <= Buffer.Length(); j++) {
           if (j + length1 - 1 > Buffer.Length()) break;
           int k = 0;
           while (k < length1) {
              if (Buffer[j+k] != source[k+1]) break;
              k++;
              }
			  if (k < length1) continue;
			  int answer = IDYES;
           Memo1->SelStart = j - 1;
           Memo1->SelLength = length1;

           PREditFilePrev = PREditFileTemp;
           PRSelStartPrev = PRSelStartTemp;
           PRSelLengthPrev = PRSelLengthTemp;
           PRIndexPrev = PRIndexTemp;

           PREditFileTemp = EditFile;
           PRSelStartTemp = Memo1->SelStart;
           PRSelLengthTemp = Memo1->SelLength;
           PRIndexTemp = d;

           if (prompt) {
              UnicodeString temp = source.SubString(1, length1);
              UnicodeString msg;
              msg.sprintf(L"%s/%s", temp.w_str(), target.w_str());
              answer = ChoiceAlert(msg);
              }
           if (answer == IDYES) { // replace
              Memo1->SetSelTextBuf(target.w_str());
              LogReplacement(j, length1, target);
              changed++;
              Buffer.Delete(j, length1);
              Buffer.Insert(target, j);
              j = j + length2 - 1;
              }
           else if (answer == IDNO) continue; // Do not replace
           else {
              if (changed) { // Cancel
		           SaveChapter(i);
				     changed = false;
				     Memo1->Modified = false;
				     }
			     PRResumeIndex = d;
              return;
              }

           }
		 }

	  if (changed) {
		 SaveChapter(i);
		 Memo1->Modified = false;
		 }

	  }

   Restore_Position:

	  FreePRList();
	  if (YesNoAlert(L"���O�r�ե����F�I��^��B�H") != IDYES) return;

	  EditFile = edit_saved;
	  fldEditFile->Text = EditFile;

	  Memo1->SelStart = start_saved;
	  Memo1->SelLength = length_saved;
	  ChapterNumber = number_saved;
	  Memo1->SetFocus();

   }


/*-------------------*/
/* THome::LoadPRList */
/*-------------------*/

int __fastcall THome::LoadPRList() {

   FreePRList();
   Buffer.SetLength(0);
   LoadFile(PRFile);
   PRList = new TStringList();
   PRList->Text = Buffer;
   if (PRList->Count == 0) {
	   delete PRList;
      PRList = NULL;
	   return false;
	   }
   PRSource = new TStringList();
   PRTarget = new TStringList();
   int skip = false;
   for (int d = 0; d < PRList->Count; d++) {
      UnicodeString source = PRList->Strings[d];
      if (source.IsEmpty()) continue;
      Char c = source[1];
      if (skip) {
         if (c != L'}') continue;
         skip = false;
         continue;
         }
      if (c == L'{') {
         skip = true;
         continue;
         }
      if (c == L'#') continue;
      if (c == L'$') {
         d = PRList->Count;
         continue;
         }
/*
      int prompt = true;
      if (c == L'~') {
         prompt = false;
         source.Delete(1, 1);
         }
*/
      int length1 = source.Length();
      int k = 1;
      int found = false;
      while (k <= length1) {
         if (source[k++] == L'/') {
            found = true;
            break;
            }
         }
      if (!found) continue;
      int length2 = length1 - k + 1;
      if (length2 == 0) continue;
      length1 = k - 2;
      if (length1 == 0) continue;
      UnicodeString target = source.SubString(k, length2);
      k = 1;
      found = false;
      while (k <= length2) {
         if (target[k++] == L' ') {
            found = true;
            break;
            }
         }
      if (found) target.SetLength(k-2);
      source.SetLength(length1);
      PRSource->Add(source);
      PRTarget->Add(target);
      }

   UnicodeString msg = PRFile + L"\n\r\n\r";
   int k = PRList->Count;
   if (k > 5) k = 5;
   for (int d = 0; d < k; d++) {
      msg = msg + PRList->Strings[d] + L"\n\r";
      }

   msg = msg + L"\n\r" + PRSource->Count;
   ShowAlert(msg);
   if (PRSource->Count > 0) return true;
   return false;

   }


/*-------------------*/
/* THome::FreePRList */
/*-------------------*/

void __fastcall THome::FreePRList() {

   if (PRList) {
      delete PRList;
      PRList = NULL;
      }
   if (PRSource) {
      delete PRSource;
      PRSource = NULL;
      }
   if (PRTarget) {
      delete PRTarget;
      PRTarget = NULL;
      }

   }


/*--------------------------*/
/* THome::btnProoRreadClick */
/*--------------------------*/

void __fastcall THome::btnProofReadClick(TObject *Sender) {

   CheckModified();
   if (!ValidateSetupData()) return;
   if (ConfirmAlert(L"���O�r�ե��H") != IDOK) return;

   UnicodeString name;
   if (RadioButton1->Checked) name = ExePath + L"C1.txt";
   else if (RadioButton2->Checked) name = ExePath + L"C2.txt";
   else if (RadioButton3->Checked) name = ExePath + L"C3.txt";
   else if (RadioButton4->Checked) name = ExePath + L"C4.txt";
   else if (RadioButton5->Checked) name = ExePath + L"C5.txt";
   else if (RadioButtonA->Checked) name = Directory + L"\\A.txt";
   else {
      name = ExePath + L"���O�r.txt";
      if (!(RadioButtonX->Checked)) RadioButtonX->Checked = true;
      }
   if (!FileExists(name)) {
	  ShowAlert(L"�䤣�� " + name);
	  return;
	  }

   PRFile = name;
   PRResumeIndex = 0;
   PRResumePosition = -1;
   PREditFilePrev.SetLength(0);
   if (!LoadPRList()) return;
   ProofRead();

   }


/*------------------------*/
/* THome::ResumeProofread */
/*------------------------*/

void __fastcall THome::ResumeProofRead(TObject *Sender) {

   if (PRFile.IsEmpty()) {
      btnProofReadClick(Sender);
      return;
      }
   CheckModified();
   if (!SetupFile.Length()) return;
   if (!fldEditFile->Text.Length()) return;

   PRResumePosition = Memo1->SelStart + 1;
   if (Memo1->SelLength > 0) PRResumePosition++;
   ProofRead();

   }


/*----------------------*/
/* THome::PrevProofread */
/*----------------------*/

void __fastcall THome::PrevProofRead(TObject *Sender) {

   CheckModified();
   if (SetupFile.IsEmpty()) return;
   if (PREditFilePrev.IsEmpty()) return;

   EditFile = PREditFilePrev;
   fldEditFile->Text = EditFile;

   Memo1->SelStart = PRSelStartPrev;
   Memo1->SelLength = PRSelLengthPrev;
   PRResumeIndex = PRIndexPrev;

   }


/*---------------------*/
/* THome::ConvertToPDB */
/*---------------------*/

void __fastcall THome::ConvertToPDB(TObject *Sender) {

   ConvertToBig5 = true;
   btnConvertClick(Sender);
   ConvertToBig5 = false;

   }

/*--------------------------*/
/* THome::ConvertMarksVePub */
/*--------------------------*/

bool __fastcall THome::ConvertMarksVePub() {
Char source[29] = L"�u�v�y�z�m�n�]�^�i�j�a�b�e�f�q�r�Тw���������U�ա֡X������";
Char target[29] = L"�w�x�{�|�o�p�_�`�k�l�c�d�g�h�s�t�W�W�w�x�{�|�W�s�t�W������";

   int changed = 0;
   int head;  // "xx" -> �wxx�x
   int head1; // 'xx' -> �{xx�|
   head = 1;
   for (int j = 1; j <= Buffer.Length(); j++) {
	   Char c = Buffer[j];
	   if (c > 255) { // Chinese character
//         if (c == L'�K') {
//            Buffer[j] = 0xFFE4;
         if (c == 0xFFE4 || c == 0xFE19) {
            Buffer[j] = L'�K';
            changed++;
            continue;
            }
         for (int k = 0; k < 29; k++) {
	         if (c == source[k]) {
		         Buffer[j] = target[k];
               changed++;
               break;
               }
			   }
			continue;
			}
/*   7/8/2015
		if (c == '"') {
			if (head) Buffer[j] = target[0]; else Buffer[j] = target[1];
			head = 1 - head;
			changed++;
			continue;
			}
		if (c == '\'') {
			if (head1) Buffer[j] = target[2]; else Buffer[j] = target[3];
			head1 = 1 - head1;
			changed++;
			continue;
			}
*/
		if (c == '\n') {
		   head = 1;
			head1 = 1;
			continue;
			}
      if (c == '(') {
         Buffer[j] = target[6];
		   changed++;
			continue;
			}
      if (c == ')') {
		   Buffer[j] = target[7];
			changed++;
			continue;
			}
/*   7/8/2015
		if (c == '-') {
		   Buffer[j] = target[16];
			changed++;
			continue;
			}
*/
		}

   return changed;

   }


/*-------------------------------*/
/* THome::ConvertMarksHorizontal */
/*-------------------------------*/

bool __fastcall THome::ConvertMarksHorizontal() {
Char source[29] = L"�w�x�{�|�o�p�_�`�k�l�c�d�g�h�s�t�W�U�w�x�{�|�s�t�U�x������";
Char target[29] = L"�u�v�y�z�m�n�]�^�i�j�a�b�e�f�q�r�w�w���������ա֡X�K������";

   int changed = 0;
   for (int j = 1; j <= Buffer.Length(); j++) {
      Char c = Buffer[j];
		if (c > 255) { // Chinese character
         if (c == 0xFFE4 || c == 0xFE19) {
            Buffer[j] = L'�K';
            changed++;
            continue;
            }
			for (int k = 0; k < 30; k++) {
			   if (c == source[k]) {
				  Buffer[j] = target[k];
              changed++;
				  break;
				  }
			   }
			}
	   }

   return changed;

   }


/*----------------------------------*/
/* THome::btnConvertMarksHorizontal */
/*---------------------------------*/

void __fastcall THome::btnConvertMarksHorizontal(TObject *Sender) {

   CheckModified();
   if (!ValidateSetupData()) return;
   if (ConfirmAlert(L"�ন����I�Ÿ��H") != IDOK) return;

   int number = ChapterNumber;
   for (int i = 0; i <= ChapterCount; i++) {
	  if (!LoadChapter(i)) continue;
	  if (ConvertMarksHorizontal()) {
        SaveChapter(i);
		  if (i == number) Memo1->Text = Buffer;
		  }
	  }

   ChapterNumber = number;
   ShowAlert(L"����I�Ÿ���n�F�I");

   }


/*--------------------*/
/* THome::ReplaceCRNL */
/*--------------------*/

void __fastcall THome::ReplaceCRNL() {

   ChapterTitle.SetLength(0);
   UnicodeString br = L"<br>";

   bool firstln = true;
   if (ChapterNumber == 0) firstln = false;
	for (int j = 1; j <= Buffer.Length(); j++) {
	   if (j + 1 > Buffer.Length()) break;
	   if (Buffer[j] == '\r' && Buffer[j+1] == '\n') {
         if (firstln) {
            ChapterTitle = Buffer.SubString(1, j - 1);
            Buffer.Delete(j, 2);
            Buffer.Insert(L"</h3>", j);
            Buffer.Insert(L"<h3>", 1);
            j = j + 7;
            firstln = false;
            continue;
            }
		   Buffer.Delete(j, 2);
         Buffer.Insert(br, j);
         j = j + 2;
			}
		}

   }

/*------------------------*/
/* THome::ReplaceCRNLePub */
/*------------------------*/

void __fastcall THome::ReplaceCRNLePub() {

   ChapterTitle.SetLength(0);
   UnicodeString br = L"<br />";

   bool firstln = true;
   if (ChapterNumber == 0) firstln = false;
	for (int j = 1; j <= Buffer.Length(); j++) {
	   if (j + 1 > Buffer.Length()) break;
	   if (Buffer[j] == '\r' && Buffer[j+1] == '\n') {
         if (firstln) {
            ChapterTitle = Buffer.SubString(1, j - 1);
            Buffer.Delete(j, 2);
            Buffer.Insert(L"</h3>", j);
            Buffer.Insert(L"<h3>", 1);
            j = j + 7;
            firstln = false;
            continue;
            }
		   Buffer.Delete(j, 2);
         Buffer.Insert(br, j);
         j = j + 4;
			}
		}

   }


/*-------------------*/
/* THome::WriteXHTML */
/*-------------------*/

void __fastcall THome::WriteXHTML() {

   UnicodeString h1 = L"<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n";
   UnicodeString h2 = L"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\r\n";
   UnicodeString h3 = L"<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"zh-TW\">\r\n<head>\r\n<title>";
   UnicodeString h4 = L"</title>\r\n<link href=\"stylesheet.css\" type=\"text/css\" rel=\"stylesheet\" />\r\n";
   UnicodeString h5 = L"<link rel=\"stylesheet\" type=\"application/vnd.adobe-page-template+xml\" href=\"page-template.xpgt\"/>\r\n";
   UnicodeString h6 = L"</head>\r\n<body>\r\n<div>\r\n";
   UnicodeString h7 = L"</div></body></html>";

   if (ChapterNumber == 0) ChapterTitle = L"�ؿ�";
   if (ChapterTitle.IsEmpty()) ChapterTitle = L"�@�@";
//   AnsiString name = "OEBPS\\";
//   name = name + ChapterNumber + ".xhtml";
//   std::auto_ptr<TFileStream> FileStream(new TFileStream(name, fmCreate));
   TBytes bom = TEncoding::UTF8->GetPreamble();
//   FileStream->Write(&bom[0], bom.Length);

   Buffer = h1 + h2 + h3 + ChapterTitle + h4 + h5 + h6 + Buffer + h7;
   TBytes buf = WideBytesOf(Buffer);
   buf = TEncoding::Convert(TEncoding::Unicode, TEncoding::UTF8, buf);
//  FileStream->Write(&buf[0], buf.Length);

   AnsiString data;
   data.SetLength(buf.Length + bom.Length);
   memcpy(&data[1], &bom[0], bom.Length);
   memcpy(&data[1+ bom.Length], &buf[0], buf.Length);
   AnsiString name = "OEBPS/";
   name = name + ChapterNumber + ".xhtml";
   Zip->AddFile(name);
   Zip->Compress = true;
   Zip->Data[ZipCount++] = data;

   }

/*------------------------*/
/* THome::WriteContentOpf */
/*------------------------*/

void __fastcall THome::WriteContentOpf() {

   UnicodeString h1 = L"<?xml version=\"1.0\"?>\r\n";
   UnicodeString h2 = L"<package version=\"2.0\" xmlns=\"http://www.idpf.org/2007/opf\" unique-identifier=\"BookId\">\r\n";
   UnicodeString h3 = L" <metadata xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:opf=\"http://www.idpf.org/2007/opf\">\r\n";
   UnicodeString h4 = L"   <dc:creator opf:role=\"aut\">";
   UnicodeString h5 = L"</dc:creator>\r\n";
   UnicodeString h6 = L"   <dc:language>zh-TW</dc:language>\r\n";
   UnicodeString h7 = L"   <dc:identifier id=\"BookId\">www.haodoo.net</dc:identifier>\r\n";
   UnicodeString h8 = L" </metadata>\r\n";
   UnicodeString h9 = L" <manifest>\r\n";
   UnicodeString h10 = L"  <item id=\"ncx\" href=\"toc.ncx\" media-type=\"application/x-dtbncx+xml\" />\r\n";
   UnicodeString h11 = L"  <item id=\"style\" href=\"stylesheet.css\" media-type=\"text/css\" />\r\n";
   UnicodeString h12 = L"  <item id=\"pagetemplate\" href=\"page-template.xpgt\" media-type=\"application/vnd.adobe-page-template+xml\" />\r\n";

   UnicodeString id;
   UnicodeString item;
   for (int i = 0; i <= ChapterCount; i++) {
      item = L"  <item id=\"P" + IntToStr(i) + L"\" href=\"" + IntToStr(i) + L".xhtml\" media-type=\"application/xhtml+xml\" />\r\n";
      id = id + item;
      }
   id = id + L" </manifest>\r\n";

   UnicodeString idref =  L"<spine page-progression-direction=\"rtl\" toc=\"ncx\">\r\n";
   if (!VerticalEpub) idref =  L"<spine toc=\"ncx\">\r\n";

   for (int i = 0; i <= ChapterCount; i++) {
      item = L"  <itemref idref=\"P" + IntToStr(i) + L"\" />\r\n";
      idref = idref + item;
      }
   idref = idref + L" </spine>\r\n</package>";

   Buffer = h1 + h2 + h3 + h4 + Author + h5
      + L"   <dc:title>" + Title + L"</dc:title>\r\n"
      + h6 + h7 + h8 + h9 + h10 + h11 + h12 + id + idref;

//   AnsiString name = "OEBPS\\content.opf";
//   std::auto_ptr<TFileStream> FileStream(new TFileStream(name, fmCreate));
   TBytes bom = TEncoding::UTF8->GetPreamble();
//   FileStream->Write(&bom[0], bom.Length);

   TBytes buf = WideBytesOf(Buffer);
   buf = TEncoding::Convert(TEncoding::Unicode, TEncoding::UTF8, buf);
 //  FileStream->Write(&buf[0], buf.Length);

   AnsiString data;
   data.SetLength(buf.Length + bom.Length);
   memcpy(&data[1], &bom[0], bom.Length);
   memcpy(&data[1+ bom.Length], &buf[0], buf.Length);
   AnsiString name = "OEBPS/content.opf";
   Zip->AddFile(name);
   Zip->Compress = true;
   Zip->Data[ZipCount++] = data;

   }


/*--------------------*/
/* THome::WriteTocNcx */
/*--------------------*/

void __fastcall THome::WriteTocNcx() {

   UnicodeString h1 = L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n";
   UnicodeString h2 = L"<ncx xmlns=\"http://www.daisy.org/z3986/2005/ncx/\" version=\"2005-1\" xml:lang=\"zh-TW\">\r\n";
   UnicodeString h3 = L"  <head>\r\n";
   UnicodeString h4 = L"    <meta name=\"dtb:uid\" content=\"www.haodoo.net\"/>\r\n";
   UnicodeString h5 = L"    <meta name=\"dtb:depth\" content=\"1\"/>\r\n";
   UnicodeString h6 = L"    <meta name=\"dtb:totalPageCount\" content=\"0\"/>\r\n";
   UnicodeString h7 = L"    <meta name=\"dtb:maxPageNumber\" content=\"0\"/>\r\n";
   UnicodeString h8 = L"  </head>\r\n";

   TStringList* list = new TStringList();
   UnicodeString name = Directory + "\\0.txt";
   if (FileExists(name)) {
      Buffer.SetLength(0);
      LoadFile(name);
      list->Text = Buffer;
      }

   UnicodeString navMap = L"  <navMap>\r\n";
   UnicodeString navPoint;
   for (int i = 0; i <= ChapterCount; i++) {
      navPoint = L"    <navPoint id=\"" + IntToStr(i) + L"\" playOrder=\"" + IntToStr(i+1) + L"\">\r\n";
      navPoint = navPoint + L"      <navLabel>\r\n        <text>";
      if (i == 0) navPoint = navPoint + L"�ؿ�";
         else if (i <= list->Count) navPoint = navPoint + list->Strings[i - 1];
      navPoint = navPoint + L"</text>\r\n      </navLabel>\r\n";
      navPoint = navPoint + L"      <content src=\""+ IntToStr(i) + L".xhtml\"/>\r\n";
      navPoint = navPoint + L"    </navPoint>\r\n";
      navMap = navMap + navPoint;
      }
   navMap = navMap + L"  </navMap>\r\n</ncx>";

   UnicodeString ncx = h1 + h2 + h3 + h4 + h5 + h6 + h7 + h8
      + L"  <docTitle>\r\n    <text>�m" + Title + L"�n" + Author + L"</text>\r\n  </docTitle>\r\n"
      + navMap;

   list->Free();
//   AnsiString zip = "OEBPS\\toc.ncx";
//   std::auto_ptr<TFileStream> FileStream(new TFileStream(zip, fmCreate));
   TBytes bom = TEncoding::UTF8->GetPreamble();
//   FileStream->Write(&bom[0], bom.Length);

   TBytes buf = WideBytesOf(ncx);
   buf = TEncoding::Convert(TEncoding::Unicode, TEncoding::UTF8, buf);
//   FileStream->Write(&buf[0], buf.Length);

   AnsiString data;
   data.SetLength(buf.Length + bom.Length);
   memcpy(&data[1], &bom[0], bom.Length);
   memcpy(&data[1+ bom.Length], &buf[0], buf.Length);
   Zip->AddFile("OEBPS/toc.ncx");
   Zip->Compress = true;
   Zip->Data[ZipCount++] = data;

   }


/*--------------------------*/
/* THome::WriteContainerXml */
/*--------------------------*/

void __fastcall THome::WriteContainerXml() {

   AnsiString h1 = "<?xml version=\"1.0\"?>\r\n";
   AnsiString h2 = "<container version=\"1.0\" xmlns=\"urn:oasis:names:tc:opendocument:xmlns:container\">\r\n";
   AnsiString h3 = "  <rootfiles>\r\n";
   AnsiString h4 = "    <rootfile full-path=\"OEBPS/content.opf\" media-type=\"application/oebps-package+xml\"/>\r\n";
   AnsiString h5 = "  </rootfiles>\r\n</container>";

//   AnsiString name = "META-INF\\container.xml";
//   std::auto_ptr<TFileStream> FileStream(new TFileStream(name, fmCreate));

//   Buffer = h1 + h2 + h3 + h4 + h5;
//   TBytes buf = BytesOf(Buffer);
//   FileStream->Write(&buf[0], buf.Length);
   AnsiString h = h1 + h2 + h3 + h4 + h5;
//   FileStream->Write(&h[1], h.Length());

   AnsiString name = "META-INF/container.xml"; // Note. "META-INF\\container.xml"; does not work.
   Zip->AddFile(name);
   Zip->Compress = true;
   Zip->Data[ZipCount++] = h;

   }


/*----------------------*/
/* THome::WriteMimeType */
/*----------------------*/

void __fastcall THome::WriteMimeType() {

   AnsiString h1 = "application/epub+zip";
   AnsiString name = "mimetype";
//   std::auto_ptr<TFileStream> FileStream(new TFileStream(name, fmCreate));
//   FileStream->Write(&h1[1], h1.Length());

   Zip->AddFile(name);
   Zip->Compress = false;
   Zip->Data[ZipCount++] = h1;

   }


/*---------------------*/
/* THome::AddToZipFile */
/*---------------------*/

void __fastcall THome::AddToZipFile(AnsiString name1, AnsiString name2) {

   std::auto_ptr<TFileStream> FileStream(new TFileStream(name1, fmOpenRead));
   TBytes Nothing;
   Nothing.set_length(FileStream->Size);
   std::auto_ptr<TBytesStream> BytesStream(new TBytesStream(Nothing));
   BytesStream->CopyFrom(FileStream.get(), FileStream->Size);

   AnsiString data;
   data.SetLength(FileStream->Size);
   memcpy(&data[1], &BytesStream->Bytes[0], FileStream->Size);
   Zip->AddFile(name2);
   Zip->Compress = true;
   Zip->Data[ZipCount++] = data;

   }


/*--------------------*/
/* THome::ConvertePub */
/*--------------------*/

void __fastcall THome::ConvertePub() {

   CheckModified();
   if (!ValidateSetupData()) return;

   UnicodeString msg = L"�ন epub �ɡH";
   if (VerticalEpub) msg = L"�ন���� epub �ɡH";
   if (ConfirmAlert(msg) != IDOK) return;

//   ePub = Directory + L"\\ePub\\";
//   if (!DirectoryExists(ePub)) CreateDir(ePub);
//   SetCurrentDir(ePub);
   Zip = new TZipFile;
   ZipCount = 0;

   WriteMimeType();
//   MetaInf = "META-INF\\";
//   if (!DirectoryExists(MetaInf)) CreateDir(MetaInf);
   WriteContainerXml();

//   OEBPS = "OEBPS\\";
//   if (!DirectoryExists(OEBPS)) CreateDir(OEBPS);

   WriteContentOpf();
   int number = ChapterNumber;
   for (int i = 0; i <= ChapterCount; i++) {
	  if (!LoadChapter(i)) continue;
	  if (!VerticalEpub) ConvertMarksHorizontal();
        else ConvertMarksVePub();
     ReplaceCRNLePub();
     WriteXHTML();
	  }

   UnicodeString name = "page-template.xpgt";
   UnicodeString name1 = ExePath + name;
   UnicodeString name2 = "OEBPS/page-template.xpgt";
//   CopyFileW(name1.w_str(), name2.w_str(), false);
   AddToZipFile(name1, name2);

   if (VerticalEpub) name = "stylesheetV.css";
      else name = "stylesheet.css";
   name1 = ExePath + name;
   name2 = "OEBPS/stylesheet.css";
//   CopyFileW(name1.w_str(), name2.w_str(), false);
   AddToZipFile(name1, name2);

   WriteTocNcx();

   if (ZipCount > 0) {
      name = Directory + '\\' + PdbName + ".epub";
      if (VerticalEpub) name = Directory + "\\V" + PdbName + ".epub";
      Zip->SaveToFile(name);
      }
   Zip->Free();


   ChapterNumber = number;
   ShowAlert(L"�ন epub �ɤF�I");

   }


/*----------------------------*/
/* THome::btnConvertePubHClick */
/*----------------------------*/

void __fastcall THome::btnConvertePubHClick(TObject *Sender) {

   VerticalEpub = false;
   ConvertePub();

   }

/*----------------------------*/
/* THome::btnConvertePubVClick */
/*----------------------------*/

void __fastcall THome::btnConvertePubVClick(TObject *Sender) {

   VerticalEpub = true;
   ConvertePub();

   }


/*---------------------*/
/* THome::ConvertToTXT */
/*---------------------*/

 void __fastcall THome::ConvertToTXT(TObject *Sender) {

   CheckModified();
   if (!ValidateSetupData()) return;

   UnicodeString msg = L"�ন txt �ɡH";
   if (ConfirmAlert(msg) != IDOK) return;

	UnicodeString name = Directory + '\\' + PdbName + ".txt";
   std::auto_ptr<TFileStream> FileStream(new TFileStream(name, fmCreate));
   FileStream->Write(&BOM[0], BOM.Length);
   int number = ChapterNumber;
   for (int i = 0; i <= ChapterCount; i++) {
	  if (!LoadChapter(i)) continue;
     if (i < ChapterCount) Buffer = Buffer + "\r\n\r\n\r\n\r\n\r\n\r\n";
	  ConvertMarksHorizontal();
     TBytes buf = WideBytesOf(Buffer);
     FileStream->Write(&buf[0], buf.Length);
	  }

   ChapterNumber = number;

   ShowAlert(L"�ন txt �ɤF�I");

   }


/*----------------------*/
/* THome::WriteXHTMLPRC */
/*----------------------*/

void __fastcall THome::WriteXHTMLPRC() {

   UnicodeString h1 = L"<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n";
   UnicodeString h2 = L"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\r\n";
   UnicodeString h3 = L"<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"zh-CN\">\r\n<head>\r\n";
   UnicodeString h4 = L"<link href=\"stylesheet.css\" type=\"text/css\" rel=\"stylesheet\" />\r\n";
   UnicodeString h5 = L"<link rel=\"stylesheet\" type=\"application/vnd.adobe-page-template+xml\" href=\"page-template.xpgt\"/>\r\n";
   UnicodeString h6 = L"</head>\r\n<body>\r\n<div>\r\n";
//  UnicodeString h7 = L"<br /></div></body></html>";
   UnicodeString h7 = L"</div></body></html>";

   UnicodeString name = PRC + '\\' + ChapterNumber + ".html";
   std::auto_ptr<TFileStream> FileStream(new TFileStream(name, fmCreate));
   TBytes bom = TEncoding::UTF8->GetPreamble();
   FileStream->Write(&bom[0], bom.Length);

   Buffer = h1 + h2 + h3 + h4 + h5 + h6 + Buffer + h7;
   TBytes buf = WideBytesOf(Buffer);
   buf = TEncoding::Convert(TEncoding::Unicode, TEncoding::UTF8, buf);
   FileStream->Write(&buf[0], buf.Length);

   }


/*--------------------*/
/* THome::WriteTOCPRC */
/*--------------------*/

void __fastcall THome::WriteTOCPRC() {

   ChapterTitle.SetLength(0);
//   UnicodeString br = L"<br />";
   UnicodeString br = L"<br>";
   int chap = 1;
   UnicodeString TOC = "";
   int head = 1;
	for (int j = 1; j <= Buffer.Length(); j++) {
	   if (j + 1 > Buffer.Length()) break;
	   if (Buffer[j] == '\r' && Buffer[j+1] == '\n') {
         UnicodeString title = Buffer.SubString(head, j - head);
         TOC = TOC + L"<a href=\"" + IntToStr(chap) + ".html\">"
            + title + L"</a><br>";
         j = j + 2;
         chap++;
         head = j;
         }
		}

   if (head < Buffer.Length()) {
	   if (Buffer[head] != '\r') {
         UnicodeString title = Buffer.SubString(head, Buffer.Length() - head + 1);
         TOC = TOC + L"<a href=\"" + IntToStr(chap) + ".html\">"
            + title + L"</a><br>";
         }
      }
   Buffer = TOC;

   }

/*---------------------------*/
/* THome::WriteContentOpfPRC */
/*---------------------------*/

void __fastcall THome::WriteContentOpfPRC() {

   UnicodeString h1 = L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n";
   UnicodeString h2 = L"<package version=\"2.0\" xmlns=\"http://www.idpf.org/2007/opf\" unique-identifier=\"BookId\">\r\n";
   UnicodeString h3 = L" <metadata xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:opf=\"http://www.idpf.org/2007/opf\">\r\n";
   UnicodeString h4 = L"   <dc:creator opf:role=\"aut\">";
   UnicodeString h5 = L"</dc:creator>\r\n";
   UnicodeString h6 = L"   <dc:language>zh-CN</dc:language>\r\n";
   UnicodeString h7 = L"   <dc:identifier id=\"BookId\">www.haodoo.net</dc:identifier>\r\n";
   UnicodeString h8 = L" </metadata>\r\n";
   UnicodeString h81 = L" <x-metadata><output encoding=\"utf-8\"></output></x-metadata>\r\n";
   UnicodeString h9 = L" <manifest>\r\n";
   UnicodeString h10 = L"  <item id=\"toc\" href=\"toc.ncx\" media-type=\"application/x-dtbncx+xml\" />\r\n";
   UnicodeString h11 = L"  <item id=\"style\" href=\"stylesheet.css\" media-type=\"text/css\" />\r\n";
   UnicodeString h12 = L"  <item id=\"pagetemplate\" href=\"page-template.xpgt\" media-type=\"application/vnd.adobe-page-template+xml\" />\r\n";

   UnicodeString id;
   UnicodeString item;
   for (int i = 0; i <= ChapterCount; i++) {
      item = L"  <item id=\"P" + IntToStr(i) + L"\" href=\"" + IntToStr(i) + L".html\" media-type=\"application/xhtml+xml\" />\r\n";
      id = id + item;
      }
   id = id + L" </manifest>\r\n";

   UnicodeString idref =  L"<spine toc=\"ncx\">\r\n";
   for (int i = 0; i <= ChapterCount; i++) {
      item = L"  <itemref idref=\"P" + IntToStr(i) + L"\" />\r\n";
      idref = idref + item;
      }
   idref = idref + L" </spine>\r\n";

   UnicodeString g1 = L"<guide>\r\n";
   UnicodeString g2 = L"<reference type=\"toc\" title=\"Table of Contents\" href=\"0.html\"></reference>\r\n";
   UnicodeString g3 = L"<reference type=\"start\" title=\"Startup Page\" href=\"0.html\"></reference>\r\n";
   UnicodeString g4 = L"</guide>\r\n";
   UnicodeString g = g1 + g2 + g3 + g4;

   Buffer = h1 + h2 + h3 + h4 + Author + h5
      + L"   <dc:title>" + Title + L"</dc:title>\r\n"
      + h6 + h7 + h8 + h81 +  h9 + h10 + h11 + h12 + id + idref + g + "</package>";

   UnicodeString name = PRC + '\\' + PdbName + ".opf"; // "\\content.opf";
   std::auto_ptr<TFileStream> FileStream(new TFileStream(name, fmCreate));
   TBytes bom = TEncoding::UTF8->GetPreamble();
   FileStream->Write(&bom[0], bom.Length);

   TBytes buf = WideBytesOf(Buffer);
   buf = TEncoding::Convert(TEncoding::Unicode, TEncoding::UTF8, buf);
   FileStream->Write(&buf[0], buf.Length);

   }


/*------------------------*/
/* THome::WriteTocNcxPRC */
/*------------------------*/

void __fastcall THome::WriteTocNcxPRC() {

   UnicodeString h1 = L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n";
   UnicodeString h2 = L"<ncx xmlns=\"http://www.daisy.org/z3986/2005/ncx/\" version=\"2005-1\" xml:lang=\"zh-CN\">\r\n";
   UnicodeString h3 = L"  <head>\r\n";
   UnicodeString h4 = L"    <meta name=\"dtb:uid\" content=\"www.haodoo.net\"/>\r\n";
   UnicodeString h5 = L"    <meta name=\"dtb:depth\" content=\"1\"/>\r\n";
   UnicodeString h6 = L"    <meta name=\"dtb:totalPageCount\" content=\"0\"/>\r\n";
   UnicodeString h7 = L"    <meta name=\"dtb:maxPageNumber\" content=\"0\"/>\r\n";
   UnicodeString h8 = L"  </head>\r\n";

   TStringList* list = new TStringList();
   UnicodeString name = Directory + L"\\0.txt";
   if (FileExists(name)) {
      Buffer.SetLength(0);
      LoadFile(name);
      list->Text = Buffer;
      }

   UnicodeString navMap = L"  <navMap>\r\n";
   UnicodeString navPoint;
   for (int i = 0; i <= ChapterCount; i++) {
      navPoint = L"    <navPoint id=\"navPoint-" + IntToStr(i) + L"\" playOrder=\"" + IntToStr(i+1) + L"\">\r\n";
      navPoint = navPoint + L"      <navLabel>\r\n        <text>";
      if (i == 0) navPoint = navPoint + L"�ؿ�";
         else if (i <= list->Count) navPoint = navPoint + list->Strings[i - 1];
      navPoint = navPoint + L"</text>\r\n      </navLabel>\r\n";
      navPoint = navPoint + L"      <content src=\"" + IntToStr(i) + L".html\"/>\r\n";
      navPoint = navPoint + L"    </navPoint>\r\n";
      navMap = navMap + navPoint;
      }
   navMap = navMap + L"  </navMap>\r\n</ncx>";

   UnicodeString ncx = h1 + h2 + h3 + h4 + h5 + h6 + h7 + h8
      + L"  <docTitle>\r\n    <text>�m" + Title + L"�n" + Author + L"</text>\r\n  </docTitle>\r\n"
      + navMap;

   list->Free();
   name = PRC + "\\toc.ncx";
   std::auto_ptr<TFileStream> FileStream(new TFileStream(name, fmCreate));
   TBytes bom = TEncoding::UTF8->GetPreamble();
   FileStream->Write(&bom[0], bom.Length);

   TBytes buf = WideBytesOf(ncx);
   buf = TEncoding::Convert(TEncoding::Unicode, TEncoding::UTF8, buf);
   FileStream->Write(&buf[0], buf.Length);

   }


/*---------------------------*/
/* THome::btnConvertPRCClick */
/*---------------------------*/

void __fastcall THome::btnConvertPRCClick(TObject *Sender) {

   CheckModified();
   if (!ValidateSetupData()) return;

   UnicodeString msg = L"�ন prc �ɡH";
   if (ConfirmAlert(msg) != IDOK) return;

   PRC = Directory + L"\\prc\\";
   if (!DirectoryExists(PRC)) CreateDir(PRC);

   int number = ChapterNumber;
   for (int i = 0; i <= ChapterCount; i++) {
	  if (!LoadChapter(i)) continue;
	  ConvertMarksHorizontal();
     if (i == 0) WriteTOCPRC(); else ReplaceCRNL();
     WriteXHTMLPRC();
	  }

   WriteContentOpfPRC();
   WriteTocNcxPRC();

   UnicodeString name = "\\page-template.xpgt";
   UnicodeString name1 = ExePath + name;
   UnicodeString name2 = PRC + name;
   CopyFileW(name1.w_str(), name2.w_str(), false);

   name = "\\stylesheet.css";
   name1 = ExePath + name;
   name2 = PRC + name;
   CopyFileW(name1.w_str(), name2.w_str(), false);

   ChapterNumber = number;
   ShowAlert(L"�ন prc �ɤF�I");

   }

boolean __fastcall THome::RenameChapters(int head, int tail, int inc) {

   if (inc > 0) {
      for (int i = tail; i >= head; i--) {
         UnicodeString name = Directory + L"\\" + i + L".txt";
         if (FileExists(name)) {
            int j = i + inc;
            UnicodeString newname = Directory + L"\\" + j + L".txt";
            if (!FileExists(newname)) {
                if (RenameFile(name, newname)) continue;
                }
            UnicodeString msg = L"�L�k�N " + name + L" ��W�� " + newname;
            ShowAlert(msg);
            return false;
            }
         }
      return true;
      }

   if (inc < 0) {
      for (int i = head; i <= tail; i++) {
         UnicodeString name = Directory + L"\\" + i + L".txt";
         if (FileExists(name)) {
            int j = i + inc;
            UnicodeString newname = Directory + L"\\" + j + L".txt";
            if (!FileExists(newname)) {
                if (RenameFile(name, newname)) continue;
                }
            UnicodeString msg = L"�L�k�N " + name + L" ��W�� " + newname;
            ShowAlert(msg);
            return false;
            }
         }
      return true;
      }

   return false;
   }

void __fastcall THome::SplitChapter(TObject *Sender) {

   CheckModified();
   if (!ValidateSetupData()) return;

   UnicodeString msg = L"�����⳹�H";
   if (ConfirmAlert(msg) != IDOK) return;

   if (!RenameChapters(ChapterNumber + 1, ChapterCount, 1)) {
      ShowAlert(L"�L�k�����⳹�I");
      return;
      }

   int start = Memo1->SelStart + 1;
   int size = Memo1->Text.Length() - Memo1->SelStart;
   Buffer = Memo1->Text.SubString(start, size);
   if (Buffer.Length() > 0) SaveChapter(ChapterNumber + 1);

   Buffer = Memo1->Text.Delete(start, size);
   if (Buffer.Length() > 0) SaveChapter(ChapterNumber);
   else {
      UnicodeString name = Directory + L"\\" + ChapterNumber + L".txt";
      DeleteFile(name);
      }
   ChapterCount++;
   fldChapterCount->Text = ChapterCount;
   SetupFileChanged = true;
   SaveSetupFile();
   NextChapter();
   ShowAlert(L"�����⳹�F�I");

   }


void __fastcall THome::MergeChapters(TObject *Sender) {

   CheckModified();
   if (!ValidateSetupData()) return;

   UnicodeString msg = L"�X�֤U���H";
   if (ConfirmAlert(msg) != IDOK) return;

   if (ChapterNumber == ChapterCount) {
      ShowAlert(L"no next chapter");
      return;
      }

   int size = Buffer.Length();
   if (LoadChapter(ChapterNumber + 1)) {
      UnicodeString name = Directory + L"\\" + ChapterNumber + L".txt";
      DeleteFile(name);
      }
   ChapterNumber--;
   Memo1->Text = Memo1->Text + Buffer;
   Buffer = Memo1->Text;
   SaveChapter(ChapterNumber);
   if (ChapterNumber + 2 <= ChapterCount) {
      if (!RenameChapters(ChapterNumber + 2, ChapterCount, -1)) {
         ShowAlert(L"�L�k�X�֤U���I");
         return;
         }
      }

   Memo1->SelStart = size;
   Memo1->SelLength = 1;
   ChapterCount--;
   fldChapterCount->Text = ChapterCount;
   SetupFileChanged = true;
   SaveSetupFile();
   ShowAlert(L"�U���X�֤F�I");

   }
//---------------------------------------------------------------------------

void __fastcall THome::InsertChapterBefore(TObject *Sender) {

   CheckModified();
   if (!ValidateSetupData()) return;

   UnicodeString msg = L"�s�W�W���H";
   if (ConfirmAlert(msg) != IDOK) return;

   if (!RenameChapters(ChapterNumber, ChapterCount, 1)) {
      ShowAlert(L"�L�k�s�W�W���I");
      return;
      }

   ChapterCount++;
   fldChapterCount->Text = ChapterCount;
   SetupFileChanged = true;
   SaveSetupFile();
   LoadChapter(ChapterNumber);
   Memo1->Text = Buffer;
   ShowAlert(L"�W���s�W�F�I");


   }

void __fastcall THome::InsertChapterAfter(TObject *Sender) {

   CheckModified();
   if (!ValidateSetupData()) return;

   UnicodeString msg = L"�s�W�U���H";
   if (ConfirmAlert(msg) != IDOK) return;

   if (!RenameChapters(ChapterNumber + 1, ChapterCount, 1)) {
      ShowAlert(L"�L�k�s�W�U���I");
      return;
      }

   ChapterCount++;
   fldChapterCount->Text = ChapterCount;
   SetupFileChanged = true;
   SaveSetupFile();
   NextChapter();
   ShowAlert(L"�U���s�W�F�I");


   }
//---------------------------------------------------------------------------

