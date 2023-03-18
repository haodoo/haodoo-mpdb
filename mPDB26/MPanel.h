/*----------*/
/*          */
/* MPanel.h */
/*          */
/*----------*/

#ifndef MPanelH
#define MPanelH

#include <Controls.hpp>
#include <Classes.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>


/*--------------*/
/* class MPanel */
/*--------------*/

class MPanel: public TCustomPanel {

public:

   __fastcall MPanel(TComponent* Owner);
   __fastcall virtual ~MPanel();

protected:

   virtual void __fastcall Paint(void);

public:

   AnsiString MainBookShelf;
   AnsiString PageFont;

// PDB functions

   int  __fastcall GetString(char* info);

// Page functions

   void __fastcall SetDrawCharMode();
   void __fastcall DrawLine(int x1, int y1, int x2, int y2);
   void __fastcall DrawChar(Char c, int x, int y);
   void __fastcall DrawChars(char* str, int x, int y);
   void __fastcall DrawCharsCentered(char* str, int y);
   void __fastcall DrawRoundRect(int x1, int y1, int x2, int y2);
   void __fastcall ClearRect(int x1, int y1, int x2, int y2);
   void __fastcall InvertRect(int x, int y, int w, int h);

   int  __fastcall TwoDigits(Char* str, int length, int index);
   void __fastcall ComputePageTop();
   void __fastcall SetPageCharSize();
   void __fastcall SetPageFont();
   void __fastcall RotatePageFont();
   void __fastcall ResetPageFont();
   void __fastcall UsePageFont();
   void __fastcall DisplayPage(int index);
   void __fastcall ShowPageMode(int clear);
   void __fastcall ClearWindow(int mode);
   void __fastcall ShowPage(int index);
   void __fastcall ComputePages();

   void __fastcall ShowFirstPage();
   void __fastcall ShowLastPage();

   void __fastcall SetSelStart();
   void __fastcall SetPageNumber();
   void __fastcall ShowChapter(int last);

   void __fastcall NextChapter();
   void __fastcall PrevChapter();

   void __fastcall NextPage();
   void __fastcall PrevPage();

// ShowPageMode functions

   void __fastcall ClearShown();

// PageMode functions

   void __fastcall PageMode0(int x, int y);
   void __fastcall ToNextPage();
   void __fastcall ToPrevPage();
   void __fastcall ToNextChapter();
   void __fastcall ToPrevChapter(int last);
   void __fastcall ShowNumber(int page);
   void __fastcall PageMode1(int x, int y);
   void __fastcall ScreenTapped(int x, int y);
   
// Ini functions

   void __fastcall ComputeYSettings();

// Mouse functions

   DYNAMIC void __fastcall MouseDown(TMouseButton button,
      TShiftState shift, int x, int y);
   DYNAMIC void __fastcall MouseMove(TShiftState shift, int x, int y);
   DYNAMIC void __fastcall MouseUp(TMouseButton button,
      TShiftState shift, int x, int y);

   DYNAMIC bool __fastcall DoMouseWheelDown(Classes::TShiftState Shift,
      const Types::TPoint &MousePos);
   DYNAMIC bool __fastcall DoMouseWheelUp(Classes::TShiftState Shift,
      const Types::TPoint &MousePos);

// Keyboard functions

   DYNAMIC void __fastcall KeyUp(WORD &key, TShiftState shift);

BEGIN_MESSAGE_MAP


END_MESSAGE_MAP(TCustomPanel)

   };

#endif
