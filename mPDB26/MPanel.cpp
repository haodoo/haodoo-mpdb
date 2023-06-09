/*------------*/
/*            */
/* MPanel.cpp */
/*            */
/*------------*/

#include "Main.h"
#include "MPanel.h"
#include <stdio.h>
#include <FileCtrl.hpp>
#include <IdGlobal.hpp>

#define FONT_COUNT 9

TColor PageColor = clBtnFace;

char* Extension = "*.PDB";
AnsiString BookShelf;
char  NewBookShelf[MAX_PATH];
AnsiString MainBookShelf;
short TargetShelf, TargetNumber;

short  StylusStatus = 0;
short  StylusDown = 0;

HINSTANCE Instance;
HANDLE HandleFile;

char*   FontSizeStr = "字體";
char*   FontStr = "字型";
char*   PenColorStr = "字色";
char*   BkColorStr = "背景";
char*   BlackStr = "黑";
char*   WhiteStr = "白";
char*   ScreenSizeStr = "螢幕";

char*   SearchStr = "尋找";
char*   SearchPromptStr = "尋找：";
char*   SearchNextStr = "找下一個";
char*   SearchPrevStr = "找上一個";
char*   NotFoundStr = "找不到！";

char*   OperationStr = "操作說明";
char*   StartStr = "開始";
char*   BackStr = "返回";
char*   MoveStr = "移至";
char*   DeleteStr = "刪除";
char*   ExitStr = "關閉";

char*   HowToStr = "如何翻頁和換節？";
char*   HowToStr1 = "全螢幕觸控：";
char*   HowToStr2 = "下一節：PgDn　上一節：PgUp";
char*   HowToStr3 = "下一頁：←↓　上一頁：→↑";
char*   HowToStr4 = "亦可使用滑鼠滾輪，上下頁";
char*   ShowStr = "顯示";
char*   PageNumberStr = "頁數";
char*   ChapterNumberStr = "節數";
char*   NextPageStr = "下一頁";
char*   PrevPageStr = "上一頁";
char*   NextChapterStr = "下一節";
char*   PrevChapterStr = "上一節";
char*   MenuStr = "選項";
char*   TurnPageStr = "翻頁";
char*   CancelStr = "取消";
char*   OKStr = "確定";

short*  BookChapterCount = NULL;
short*  ContentsPtr = NULL;
short*  InfoPtr = NULL;
short*  BookChapter = NULL;
short*  BookPage = NULL;

short WBook1, WBook2;
short YLine1, YLine2, YLine3, YLine4, YLine5, YLine6, YLine7;
short XText1, XText2;
short XTitle, WTitle;
short XYes, XNo;

Char* Chapter = NULL;
int   ChapterLength;

char* Contents = NULL;
int ContentsLength = 0;

short PageNumber = 1;
short PageCount;

short ChapterCountSaved;
short PageNumberSaved;

int PageShown;
int PageNumberShown;

int ChapterNumberShown;

int ContentsShown;
int BooksShown;

int* Stack = NULL;
short StackSize = 256;
short StackSizeInc = 100;
short StackTop = 0;

short ScreenWidth;
short ScreenHeight;
short ScreenTop = 26;
short PageTopMargin = 16;
short PageRightMargin = 20;
short PageTop, PageBottom, PageLeft, PageRight;

short ScreenCount;
short ScreenSize = 0;
short ScreenSizeChanged = false;

short PenColor = 1;
short BkColor = 2;

short BkColors[54] = {
   255, 255, 255,
   128, 96, 198,
   128, 128, 64,
   0, 128, 128,
   0, 128, 192,
   192, 192, 255,
   255, 192, 255,
   192, 255, 255,
   255, 255, 192,

   58, 58, 58,
   84, 54, 128,
   96, 84, 0,
   0, 64, 64,
   0, 64, 128,
   58, 110, 165,
   127, 127, 191,
   0, 96, 0,
   0, 0, 96
   };

short FontSize = 1;
short CharWidth;
short CharHeight;
short LineOffset = 8;
short SpaceWidth = 8;

char  Temp[MAX_PATH];
short PageMode = 1;
short PrevPageMode;

short PageX, PageY, ChapterX, ChapterY;
short XLeft, XRight, XHalf, YHalf;
short YPage, YChapter;
short YLineGap, YTextGap;
short ChapterTitleLength;
short YNumber, YPageNumber;
short XLeft0, XRight0, W2;
short XMain;
short XFont[FONT_COUNT];

HWND    fldSearch = 0;
char    SearchString[21];  // At most 10 Chinese characters
short   SearchOn = false;
short   SearchFound = false;
short   SearchChapter = 0;
short   SearchPosition = 0;
short   SearchResultShown = false;

short Newln, Indent, YIndent;
int   FullScreen = false;
char* mBookFont = "新細明體";

LOGFONT PageLogFont;
HFONT hPageFont = 0;

#define MAX_INDENT_PAGE 100
char  IndentPage[MAX_INDENT_PAGE];

#ifdef DEMO
int DemoCount = 30;
int Expired = false;
#endif

int CharIndex;
int PrevSelStart;

extern TMemo *Memo;
extern UnicodeString Buffer;
extern int ChapterCount;
extern int ChapterNumber;


/*----------------*/
/* MPanel::MPanel */
/*----------------*/

__fastcall MPanel::MPanel(TComponent* Owner) : TCustomPanel(Owner) {

   Left = Memo->Left;
   Top = 2;
   Width = Memo->Width;
   Height = Memo->Height + Memo->Top - Top + 28;

   ScreenWidth = Width;
   ScreenHeight = Height;
   int section = ScreenWidth / 3 + 1;
   XLeft = section;
   XRight = ScreenWidth - XLeft;
   XHalf = ScreenWidth / 2;

   section = ScreenWidth / 3 + 1;
   XLeft = section;
   XRight = ScreenWidth - XLeft;

   section = ScreenHeight / 3;
   YChapter = section;
   YPage = ScreenHeight - YChapter - YLineGap;

   YHalf = ScreenHeight / 2;
   XHalf = ScreenWidth / 2;

   Brush->Color = PageColor; //Memo->Brush->Color;

   }


/*-----------------*/
/* MPanel::~MPanel */
/*-----------------*/

__fastcall MPanel::~MPanel() {

   if (Stack) delete[] Stack;

   }


/*---------------*/
/* MPanel::Paint */
/*---------------*/

void __fastcall MPanel::Paint() {

   ShowPageMode(false);

   }


/*-----------------*/
/* SetDrawCharMode */
/*-----------------*/

void __fastcall MPanel::SetDrawCharMode() {

   SetBkMode(Canvas->Handle, TRANSPARENT);

   }


/*------------------*/
/* MPanel::DrawLine */
/*------------------*/

void __fastcall MPanel::DrawLine(int x1, int y1, int x2, int y2) {

   Canvas->Pen->Color = clBlack;
   Canvas->MoveTo(x1, y1);
   Canvas->LineTo(x2, y2);

   }


/*------------------*/
/* MPanel::DrawChar */
/*------------------*/

void __fastcall MPanel::DrawChar(Char c, int x, int y) {

   if ((c != L'│') && (c != 0xFFE4)) {
	  UnicodeString out = c;
	  Canvas->TextOut(x, y, out);
	  }
   else {
	  RotatePageFont();
	  Canvas->TextOut(x, y + CharHeight, L'…');
	  ResetPageFont();
	  }

   }


/*-------------------*/
/* MPanel::DrawChars */
/*-------------------*/

void __fastcall MPanel::DrawChars(char* str, int x, int y) {

   RECT rect;
   SetRect(&rect, x, y, ScreenWidth, y + CharHeight + 5);
   DrawText(Canvas->Handle, str, -1, &rect, DT_LEFT);

   }


/*---------------*/
/* DrawRoundRect */
/*---------------*/

void __fastcall MPanel::DrawRoundRect(int x1, int y1, int x2, int y2) {

   Canvas->RoundRect(x1, y1, x2, y2, 10, 10);

   }


/*-------------------*/
/* MPanel::ClearRect */
/*-------------------*/

void __fastcall MPanel::ClearRect(int x1, int y1, int x2, int y2) {

   TRect rect;
   rect.left = x1; rect.top = y1;
   rect.right = x2; rect.bottom = y2;
   Canvas->Brush->Color = PageColor; // Memo->Brush->Color;
   Canvas->FillRect(rect);

   }


/*--------------------*/
/* MPanel::InvertRect */
/*--------------------*/

void __fastcall MPanel::InvertRect(int x, int y, int w, int h) {

   BitBlt(Canvas->Handle, x, y, w, h, Canvas->Handle, x, y, DSTINVERT);

   }


/*-------------------*/
/* MPanel::TwoDigits */
/*-------------------*/

int __fastcall MPanel::TwoDigits(Char* str, int length, int index) {
Char d;

   if (index > 0) {
      d = str[index - 1];
      if ('0' <= d && d <= '9') return false;
      }

   if (++index == length) return false;
   d = str[index];
   if (d < '0' || d > '9') return false;

   if (++index == length) return true;
   d = str[index];
   if ('0' <= d && d <= '9') return false;
   return true;

   }


/*------------------------*/
/* MPanel::ComputePageTop */
/*------------------------*/

void __fastcall MPanel::ComputePageTop() {

   int count = (ScreenHeight - PageTopMargin - PageTopMargin) / CharHeight;
   PageTop = (ScreenHeight - count * CharHeight) / 2;
   PageBottom = ScreenHeight - PageTop;
   PageRight = ScreenWidth - PageRightMargin;

   }


/*-------------------------*/
/* MPanel::SetPageCharSize */
/*-------------------------*/

void __fastcall MPanel::SetPageCharSize() {

//   CharWidth = CharHeight = 14 + FontSize * 2;
//   Canvas->Font = Memo->Font;
   CharWidth = CharHeight = - Memo->Font->Height + 1;
   if (Indent) YIndent = Indent * CharHeight;

   }


/*---------------------*/
/* MPanel::SetPageFont */
/*---------------------*/

void __fastcall MPanel::SetPageFont() {

   SetPageCharSize();

   SIZE dimension;
   TEXTMETRIC tm;
   PageLogFont.lfHeight = - (CharHeight);
   PageLogFont.lfWidth = 0;

   PageLogFont.lfEscapement = 0;
   PageLogFont.lfOrientation = 0;
   PageLogFont.lfWeight = FW_NORMAL;

   PageLogFont.lfItalic = 0;
   PageLogFont.lfUnderline = 0;
   PageLogFont.lfStrikeOut = 0;

   PageLogFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
   PageLogFont.lfOutPrecision = CLIP_DEFAULT_PRECIS;

   PageLogFont.lfCharSet = CHINESEBIG5_CHARSET;

   PageLogFont.lfQuality = DEFAULT_QUALITY;
   PageLogFont.lfPitchAndFamily = DEFAULT_PITCH;

   strcpy(PageLogFont.lfFaceName, Memo->Font->Name.c_str());

   if (hPageFont) DeleteObject(hPageFont);
   hPageFont = CreateFontIndirect(&PageLogFont);

   }


/*------------------------*/
/* MPanel::RotatePageFont */
/*------------------------*/

void __fastcall MPanel::RotatePageFont() {

   PageLogFont.lfEscapement = 900;
   PageLogFont.lfOrientation = 900;
   if (hPageFont) DeleteObject(hPageFont);
   hPageFont = CreateFontIndirect(&PageLogFont);
   SelectObject(Canvas->Handle, hPageFont);

   }
  
 
 /*----------------------*/
/* MPanel::ResetPageFont */
/*-----------------------*/

void __fastcall MPanel::ResetPageFont() {

   PageLogFont.lfEscapement = 0;
   PageLogFont.lfOrientation = 0;
   if (hPageFont) DeleteObject(hPageFont);
   hPageFont = CreateFontIndirect(&PageLogFont);
   SelectObject(Canvas->Handle, hPageFont);

   }


/*---------------------*/
/* MPanel::UsePageFont */
/*---------------------*/

void __fastcall MPanel::UsePageFont() {

   SetPageCharSize();
   SelectObject(Canvas->Handle, hPageFont);

   }


/*---------------------*/
/* MPanel::DisplayPage */
/*---------------------*/

Char Chinese[] = L"ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ︵︶｜，：；？！";

void __fastcall MPanel::DisplayPage(int index) {
int x, y, space, height, width, bytes, number, indent;
Char c;

   Show_Page_Slip:

      Canvas->Pen->Color = clRed;
      int w = ScreenWidth / PageCount + 1;
      x = (PageCount - PageNumber) * ScreenWidth / PageCount;
      Canvas->MoveTo(x, ScreenHeight - 1);
      Canvas->LineTo(x + w, ScreenHeight - 1);

   Show_Chapter_Slip:

      int count = ChapterCount;
      if (!count) count = 1;
      w = ScreenWidth / count + 1;
      x = (ChapterCount - ChapterNumber) * ScreenWidth / count + 1;
      Canvas->MoveTo(x, 1);
      Canvas->LineTo(x + w, 1);

   Init:

      if (PenColor == 1) Canvas->Font->Color = clBlack;
         else Canvas->Font->Color = clWhite;
      UsePageFont();
      ComputePageTop();
      x = PageRight - CharWidth - 1;
      y = PageTop;
      indent = false;
      if (Indent) {
         Newln = false;
         if (PageNumber < MAX_INDENT_PAGE)
            if (IndentPage[PageNumber]) {
               y += YIndent;
               indent = true;
               }
         }
      space = false;
      height = PageBottom - CharHeight;
      width = CharWidth + LineOffset;
      SetBkMode(Canvas->Handle, TRANSPARENT);

   Next_Char:

        c = Chapter[index];
        if (c > 255) {
           DrawChar(Chapter[index], x, y);
           if (++index >= ChapterLength) goto Done;
           goto Next_Position;
           }
        if (c == '\r') {
           if (++index >= ChapterLength) goto Done;
           if (y == PageTop)
              if (Chapter[index - 2] == '\n') space = true;
              else goto Next_Char;
           else if (indent) if (y == PageTop + YIndent) {
                y = PageTop;
                if (Chapter[index - 2] == '\n') space = true;
                   else goto Next_Char;
                   }
                indent = false;
                goto New_Line;
                }
	  if (c == '\n') {
         if (++index >= ChapterLength) goto Done;
         goto Next_Char;
         }
      bytes = 2;
      switch (c) {
         case '(': number = 52; break;
         case ')': number = 53; break;
         case '-': number = 54; break;
         case ',': number = 55; break;
         case ':': number = 56; break;
         case ';': number = 57; break;
         case '?': number = 58; break;
         case '!': number = 59; break;
         default:
            if ('A' <= c && c <= 'Z') number = c - 'A';
			else if ('a' <= c && c <= 'z') number = c - 'a' + 26;
			else if ('0' <= c && c <= '9') {
			   if (y == PageTop) {
				  if (index) if (Chapter[index - 1] == 10)
					 indent = true;
				  }
			   if (!TwoDigits(Chapter, ChapterLength, index)) goto Single_Byte;
			   bytes = 1;
			   DrawChar(Chapter[index], x, y);
			   DrawChar(Chapter[++index], x + CharWidth/2, y);
			   }
            else { Single_Byte:
			   bytes = 1;
			   DrawChar(Chapter[index], x + CharWidth/4, y);
			   }
         }
	  if (bytes == 2) DrawChar(Chinese[number], x, y);
      if (++index >= ChapterLength) goto Done;

   Next_Position:

      y = y + CharHeight;
      if (y <= height) goto Next_Char;
      Newln = true;
 
   New_Line:
   
      y = PageTop;
      if (Indent && indent && Newln) { y += YIndent; Newln = false; }
      if (space) { x = x - SpaceWidth; space = false; }
      else x = x - width;
      if (x >= PageRightMargin) goto Next_Char;

   Done:

      SetFocus();

   }


/*----------------------*/
/* MPanel::ShowPageMode */
/*----------------------*/

void __fastcall MPanel::ShowPageMode(int clear) {
/*
   if (PageMode == 1) Brush->Color = PageColor;
   else {
      Brush->Color = mBookColor;
      Canvas->Font->Color = clBlack;
      }
*/
   Canvas->Brush->Color = PageColor;

   if (clear) {
      RECT rect;
      rect.left = 0; rect.right = ScreenWidth;
      rect.top = 0; rect.bottom = ScreenHeight;
      Canvas->FillRect(rect);
      }

   switch (PageMode) {
      case 1:
         if (PageShown >= 0) DisplayPage(PageShown);
//         if (ChapterNumberShown) ShowNumber(2);
//         if (PageNumberShown) ShowNumber(1);
         return;
      }

   }


/*---------------------*/
/* MPanel::ClearWindow */
/*---------------------*/

void __fastcall MPanel::ClearWindow(int mode) {

   PageMode = mode;
   StylusDown = false;

   ShowPageMode(true);

   }


/*------------------*/
/* MPanel::ShowPage */
/*------------------*/

void __fastcall MPanel::ShowPage(int index) {

   PageShown = index;
   ClearWindow(1);

   }


/*----------------------*/
/* MPanel::ComputePages */
/*----------------------*/

void __fastcall MPanel::ComputePages() {
int x, y, space, height, width, size, indent;
int index;
Char c;
int* stack;

   Init:

      ComputePageTop();

      y = PageTop;
      index = 0;
      space = false;
      indent = false;
      height = PageBottom - CharHeight;
      width = CharWidth + LineOffset;

      PageNumber = 1;
      StackTop = PageNumber;

   Next_Page:

      x = PageRight - CharWidth - 1;
      Stack[PageNumber] = index;
      if (Indent) {
         Newln = false;
         if (PageNumber < MAX_INDENT_PAGE)
            IndentPage[PageNumber] = (y > PageTop);
         }

   Next_Char:

      c = Chapter[index];
	  if (c > 255) {
		 if (++index >= ChapterLength) goto Done;
		 goto Next_Position;
		 }
	  if (c == '\r') {
		 if (++index >= ChapterLength) goto Done;
		 if (y == PageTop)
			if (Chapter[index - 2] == '\n') space = true;
			else goto Next_Char;
		 else if (indent) if (y == PageTop + YIndent) {
			y = PageTop;
			if (Chapter[index - 2] == '\n') space = true;
			else goto Next_Char;
			}
		 indent = false;
		 goto New_Line;
		 }
	  if (c == '\n') {
		 if (++index >= ChapterLength) goto Done;
		 goto Next_Char;
		 }

	  if ('0' <= c && c <= '9') {
		 if (y == PageTop) {
			if (index) if (Chapter[index - 1] == 10)
			   indent = true;
			}
	     if (TwoDigits(Chapter, ChapterLength, index)) {
			index++;
			}
		 }

	  if (++index >= ChapterLength) goto Done;

   Next_Position:

	  y = y + CharHeight;
	  if (y <= height) goto Next_Char;
	  Newln = true;

   New_Line:

	  y = PageTop;
	  if (Indent && indent && Newln) { y += YIndent; Newln = false; }
	  if (space) { x = x - SpaceWidth; space = false; }
	  else x = x - width;
	  if (x >= PageRightMargin) goto Next_Char;

   New_Page:

	  PageNumber++;
	  if (PageNumber >= StackSize - 2) {
		 stack = Stack;
		 StackSize = StackSize + StackSizeInc;
		 Stack = new int[StackSize];
		 size = (StackSize - StackSizeInc) * sizeof(int);
		 memmove(Stack, stack, size);
		 delete[] stack;
		 }
	  goto Next_Page;

   Done:

	  PageCount = PageNumber;
	  StackTop = PageNumber + 1;
	  Stack[StackTop] = index;

   }


/*-----------------------*/
/* MPanel::ShowFirstPage */
/*-----------------------*/

void __fastcall MPanel::ShowFirstPage() {

   PageNumber = 1;
   ShowPage(0);

   }


/*----------------------*/
/* MPanel::ShowLastPage */
/*----------------------*/

void __fastcall MPanel::ShowLastPage() {

   PageNumber = PageCount;
   ShowPage(Stack[PageNumber]);

   }


/*---------------------*/
/* MPanel::SetSelStart */
/*---------------------*/

void __fastcall MPanel::SetSelStart() {

   Memo->SelStart = Stack[PageNumber + 1];
   Memo->SelLength = 0;

   }


/*-----------------------*/
/* MPanel::SetPageNumber */
/*-----------------------*/

void __fastcall MPanel::SetPageNumber() {

   PrevSelStart = Memo->SelStart;
   ComputePages();
   PageNumber = 1;
   while (true) {
      if (Stack[PageNumber] >= PrevSelStart) break;
      if (++PageNumber >= PageCount) break;
      }

   PageNumber--;
   if (PageNumber < 1) PageNumber = 1;

   }


/*---------------------*/
/* MPanel::ShowChapter */
/*---------------------*/

void __fastcall MPanel::ShowChapter(int last) {

   Buffer = Memo->Text;
   Chapter = Buffer.w_str();
   ChapterLength = Buffer.Length();
/*
   Canvas->Font = Memo->Font;
   CharWidth = CharHeight = - Canvas->Font->Height;
*/
   UsePageFont();
   if (!Stack) Stack = new int[StackSize];

   if (last) {
      ComputePages();
      ShowLastPage();
      }
   else {
      SetPageNumber();
      PageShown = Stack[PageNumber];
      }

   }


/*------------------*/
/* MPanel::NextPage */
/*------------------*/

void __fastcall MPanel::NextPage() {

   if (PageNumber < PageCount) {
      ShowPage(Stack[++PageNumber]);
      }
   else if (ChapterNumber < ChapterCount) {
      ((THome*) Parent)->NextChapter();
      }

   }


/*------------------*/
/* MPanel::PrevPage */
/*------------------*/

void __fastcall MPanel::PrevPage() {

   if (PageNumber > 1) {
      PageNumber--;
      ShowPage(Stack[PageNumber]);
      }
   else if (ChapterNumber > 1) {
      ((THome*) Parent)->PrevChapter(true);
      }

   }


//
// ShowPageMode functions
//

/*--------------------*/
/* MPanel::ClearShown */
/*--------------------*/

void __fastcall MPanel::ClearShown() {

   PageNumberShown = false;

   }



//
// PageMode functions
//

/*--------------------*/
/* MPanel::ToNextPage */
/*--------------------*/

void __fastcall MPanel::ToNextPage() {

   ClearShown();
   NextPage();

   }


/*--------------------*/
/* MPanel::ToPrevPage */
/*--------------------*/

void __fastcall MPanel::ToPrevPage() {

   ClearShown();
   PrevPage();

   }


/*-----------------------*/
/* MPanel::ToNextChapter */
/*-----------------------*/

void __fastcall MPanel::ToNextChapter() {

   ClearShown();
   if (PageNumber == 1) {
      if (ChapterNumber == ChapterCount) ShowLastPage();
      else ((THome*) Parent)->NextChapter();
      }
   else if (PageNumber != PageCount) ShowLastPage();
   else if (ChapterNumber < ChapterCount) ((THome*) Parent)->NextChapter();

   }


/*-----------------------*/
/* MPanel::ToPrevChapter */
/*-----------------------*/

void __fastcall MPanel::ToPrevChapter(int last) {

   ClearShown();
   if (PageNumber > 1) ShowFirstPage();
   else if (ChapterNumber > 1) ((THome*) Parent)->PrevChapter(false);
   }


/*--------------------*/
/* MPanel::ShowNumber */
/*--------------------*/

void __fastcall MPanel::ShowNumber(int page) {

   int number, count, shown;
   if (page == 1) { // Show PageNumber
      number = PageNumber;
      count = PageCount;
      shown = PageNumberShown;
      }
   else { // show ChapterNumber
      number = ChapterNumber;
      count = ChapterCount;
      shown = ChapterNumberShown;
      }

   sprintf(Temp, " %u/%u ", number, count);
   SIZE size;
   UsePageFont();
   GetTextExtentPoint32(Canvas->Handle, Temp, strlen(Temp), &size);
   int x = (ScreenWidth - size.cx) / 2;
   int h = CharHeight;
   if (page == 2) h++;
   int y = (page == 1) ? ScreenHeight - h : 0;
   if (shown) {
      ClearRect(x, y, x + size.cx, y + h);
      SetBkMode(Canvas->Handle, TRANSPARENT);
      int c = ((PenColor-1) * 9 + BkColor - 1) * 3;
      SetBkColor(Canvas->Handle, RGB(BkColors[c], BkColors[c+1], BkColors[c+2]));
      if (PenColor == 1) Canvas->Font->Color = clBlack;
         else Canvas->Font->Color = clWhite;
       DrawChars(Temp, x, y);
      InvertRect(x, y, size.cx, h);
      }
   else {
      RECT rect;
      SetRect(&rect, x, y - 2, x + size.cx, y + h + 2);
      InvalidateRect(Handle, &rect, true);
      }

   }


/*-------------------*/
/* MPanel::PageMode1 */
/*-------------------*/
// Page Display
void __fastcall MPanel::PageMode1(int x, int y) {

   if (StylusStatus != 1) return;
   if (x < XLeft) {
      if (y >= YHalf) ToNextPage();
      else ToNextChapter();
      return;
      }
   if (x >= XRight) {
      if (y >= YHalf) ToPrevPage();
      else ToPrevChapter(false);
      return;
      }

   if (y >= YPage) { 
      PageNumberShown = 1 - PageNumberShown;
      ShowNumber(1);
      return;
      }
   if (y < YChapter) { 
      ChapterNumberShown = 1 - ChapterNumberShown;
      ShowNumber(2);
      return;
      }

   ((THome*) Parent)->RestoreMemo();

   }


/*----------------------*/
/* MPanel::ScreenTapped */
/*----------------------*/

void __fastcall MPanel::ScreenTapped(int x, int y) {

   if (!StylusDown) return;
   switch (PageMode) {
      case 1: PageMode1(x, y); return; // Page Display

      }

   }


//
// Mouse functions
//

/*-------------------*/
/* MPanel::MouseDown */
/*-------------------*/

void __fastcall MPanel::MouseDown(TMouseButton button,
   TShiftState shift, int x, int y) {

   if (button != mbLeft) return;
   StylusDown = true;
   StylusStatus = 1;
   ScreenTapped(x, y);

   }


/*-------------------*/
/* MPanel::MouseMove */
/*-------------------*/

void __fastcall MPanel::MouseMove(TShiftState shift, int x, int y) {

   if (!StylusDown) return;
   if (PageMode == 0 || PageMode == 2 || PageMode == 4) {
      StylusStatus = 2;
      ScreenTapped(x, y);
      }

   }


/*-----------------*/
/* MPanel::MouseUp */
/*-----------------*/

void __fastcall MPanel::MouseUp(TMouseButton button,
   TShiftState shift, int x, int y) {

   if (button != mbLeft) return;
   StylusStatus = 3;
   ScreenTapped(x, y);
   StylusDown = false;

   }


/*--------------------------*/
/* MPanel::DoMouseWheelDown */
/*--------------------------*/

bool __fastcall MPanel::DoMouseWheelDown(Classes::TShiftState Shift,
   const Types::TPoint &MousePos) {

   ToNextPage();
   return true;

   }


/*------------------------*/
/* MPanel::DoMouseWheelUp */
/*------------------------*/

bool __fastcall MPanel::DoMouseWheelUp(Classes::TShiftState Shift,
   const Types::TPoint &MousePos) {

   ToPrevPage();
   return true;

   }

//
// Keyboard functions
//

/*---------------*/
/* MPanel::KeyUp */
/*---------------*/

void __fastcall MPanel::KeyUp(WORD& key, TShiftState shift) {

   switch (key) {
      case VK_UP:
      case VK_RIGHT:
      case VK_PRIOR:
         if (key != VK_PRIOR) ToPrevPage();
         else ToPrevChapter(false);
         break;
      case VK_DOWN:
      case VK_LEFT:
      case VK_NEXT:
         if (key != VK_NEXT) ToNextPage();
         else ToNextChapter();
         break;
      }

   }
