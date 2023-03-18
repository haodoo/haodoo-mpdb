//---------------------------------------------------------------------------

#ifndef MainH
#define MainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
class THome : public TForm
{
__published:	// IDE-managed Components
        TButton *btnExit;
        TEdit *fldSetupFile;
        TButton *btnSetupFile;
        TMemo *Memo1;
        TEdit *fldDirectory;
        TLabel *Label4;
        TEdit *fldTitle;
        TLabel *Label3;
        TEdit *fldChapterCount;
        TBevel *Bevel5;
        TBevel *Bevel6;
        TTabControl *TabControl2;
        TPanel *Panel1;
        TTabControl *TabControl3;
        TButton *Button107;
        TButton *Button108;
        TButton *Button161;
        TButton *Button162;
        TButton *Button163;
        TButton *Button164;
        TButton *Button212;
        TButton *Button213;
        TButton *Button214;
        TPanel *Panel2;
	TCheckBox *cbAutoReplace;
        TLabel *Label5;
	TLabel *labelReplace;
        TEdit *fldSource;
        TEdit *fldTarget;
        TButton *Button216;
        TButton *Button217;
        TButton *Button218;
        TButton *Button219;
        TButton *Button220;
        TButton *Button221;
        TPanel *Panel3;
        TPanel *Panel4;
        TButton *btnSwitch;
        TPanel *Panel5;
        TEdit *fldEditFile;
        TButton *btnPreview;
        TButton *btnPrevChapter;
        TButton *btnNextChapter;
        TButton *btnSaveFile;
        TButton *Button2;
	TPopupMenu *PopupMenu1;
	TMenuItem *N1;
	TMenuItem *N2;
	TMenuItem *N3;
	TMenuItem *N4;
	TButton *Button1;
	TLabel *Label1;
	TLabel *Label2;
	TButton *Button3;
	TCheckBox *cbSearchOnly;
	TButton *Button4;
	TButton *Button5;
   TButton *Button6;
   TButton *Button7;
   TButton *Button8;
   TBevel *Bevel1;
   TLabel *Label6;
   TEdit *fldAuthor;
   TButton *Button9;
   TButton *Button10;
   TButton *Button11;
   TButton *Button12;
   TRadioButton *RadioButton1;
   TRadioButton *RadioButton2;
   TRadioButton *RadioButton3;
   TRadioButton *RadioButton4;
   TRadioButton *RadioButton5;
   TRadioButton *RadioButtonA;
   TButton *Button14;
   TButton *Button15;
   TLabel *Label7;
   TRadioButton *RadioButtonX;
   TButton *Button13;
   TButton *Button16;
   TButton *Button17;
   TButton *Button18;
   TButton *Button19;
   TButton *Button20;

   void __fastcall InitPanel();
   void __fastcall CreateButtons();
   void __fastcall GetExePath();
   void __fastcall RestoreCursor();
   void __fastcall ShowWaitCursor();
   int  __fastcall LoadFile(const UnicodeString name);
   void __fastcall ShowAlert(const UnicodeString msg);
   int  __fastcall YesNoAlert(const UnicodeString msg);
   int  __fastcall ConfirmAlert(const UnicodeString msg);
   int  __fastcall ChoiceAlert(const UnicodeString msg);

   int  __fastcall GetChapterSize(int i);
   int  __fastcall GetChapterSizes();
   bool __fastcall LoadChapter(int i);
   void __fastcall SaveChapter(int i);

   void __fastcall SetPdbHeader();
   void __fastcall SetPdbBookInfo();
   void __fastcall GetChapterBig5Sizes();
   void __fastcall ScanBufferIn();
   void __fastcall ScanBufferOut();
   void __fastcall WritePdb();

   void __fastcall fldSetupFileChange(TObject *Sender);
   void __fastcall fldDirectoryChange(TObject *Sender);
   void __fastcall fldChapterCountChange(TObject *Sender);
   void __fastcall fldAuthorChange(TObject *Sender);
   void __fastcall fldTitleChange(TObject *Sender);

   void __fastcall LoadIni();
   void __fastcall SaveIni();

   void __fastcall LoadSetupFile();
   void __fastcall SaveSetupFile();

   int  __fastcall ValidateINF(UnicodeString& name);

   int __fastcall GetPDBString();
   int __fastcall GetPDBNumber();
   int __fastcall GetPDBRecordOffset(int i);
   int __fastcall LoadPDBChapter();
   void __fastcall SavePDBChapter();
   void __fastcall LoadAuthor(char* header);
   int __fastcall LoadPDBBook(UnicodeString name);
   void __fastcall ExportTextFiles();

   void __fastcall btnOpenClick(TObject *Sender);
   void __fastcall btnSaveClick(TObject *Sender);
   void __fastcall btnSaveAsClick(TObject *Sender);

   void __fastcall btnPrevBookClick(TObject *Sender);
   void __fastcall btnNextBookClick(TObject *Sender);

   void __fastcall LogReplacement(int j, int length, UnicodeString target);
   void __fastcall LogBig5Replacement(int j, int length, UnicodeString target);
//   void __fastcall Replace(int i);

   void __fastcall btnExitClick(TObject *Sender);

   void __fastcall LoadEditFile();
   void __fastcall SaveEditFile();
   void __fastcall btnSaveFileClick(TObject *Sender);
   void __fastcall SetChapterNumber();
   void __fastcall btnEditFileClick(TObject *Sender);
   void __fastcall fldEditFileChange(TObject *Sender);

   void __fastcall btnInsertChar(TObject *Sender);
   void __fastcall ReplaceChar();
   void __fastcall btnReplaceChar(TObject *Sender);

   void __fastcall btnSearchDownClick(TObject *Sender);
   void __fastcall btnSearchUpClick(TObject *Sender);
   void __fastcall btnResumeClick(TObject *Sender);

   void __fastcall RestoreMemo();
   void __fastcall PreviewWindow();
   void __fastcall btnPreviewClick(TObject *Sender);

   void __fastcall PrevChapter(int last);
   void __fastcall btnPrevChapterClick(TObject *Sender);
   void __fastcall NextChapter();
   void __fastcall btnNextChapterClick(TObject *Sender);

   void __fastcall btnDirectoryClick(TObject *Sender);

   int  __fastcall ValidateSetupData();

   void __fastcall btnSavePositionClick(TObject *Sender);
   void __fastcall btnRestorePositionClick(TObject *Sender);
   void __fastcall btnPasteClick(TObject *Sender);

   bool __fastcall ConvertMarks();
   void __fastcall btnConvertMarksClick(TObject *Sender);
   void __fastcall btnExamineMarksClick(TObject *Sender);

   void __fastcall btnConvertClick(TObject *Sender);
   void __fastcall btnFontClick(TObject *Sender);

   void __fastcall btnRemoveSoftBreakClick(TObject *Sender);
   void __fastcall btnAddSpaceClick(TObject *Sender);
   void __fastcall btnRemoveSpaceClick(TObject *Sender);
   void __fastcall btnRemoveWhitesClick(TObject *Sender);
   void __fastcall btnSwitchClick(TObject *Sender);
   void __fastcall btnIndentClick(TObject *Sender);
   void __fastcall btnProduceContentsClick(TObject *Sender);

   void __fastcall ToChineseNumber(UnicodeString source, UnicodeString& target);
   void __fastcall ToChineseYear(UnicodeString source, UnicodeString& target);
   void __fastcall btnConvertNumberClick(TObject *Sender);
   void __fastcall PrevConvertNumber(TObject *Sender);

   void __fastcall LoadPlate();
   void __fastcall SavePlate();
   void __fastcall LayoutButtons();
   void __fastcall DrawPlateLine();

   void __fastcall TabControl2Change(TObject *Sender);
   void __fastcall TabControl3Change(TObject *Sender);
   void __fastcall CheckModified();

   void __fastcall FormPaint(TObject *Sender);
   void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);

	void __fastcall btnSetupFileClick(TObject *Sender);
	void __fastcall btnContentsClick(TObject *Sender);
	void __fastcall NewSetupFile(TObject *Sender);
	void __fastcall cbSearchOnlyClick(TObject *Sender);
	void __fastcall cbAutoReplaceClick(TObject *Sender);
   void __fastcall Backup(TObject *Sender);
   void __fastcall RestoreBackup(TObject *Sender);
   void __fastcall ProofRead();
   int  __fastcall LoadPRList();
   void __fastcall FreePRList();
   void __fastcall btnProofReadClick(TObject *Sender);
   void __fastcall ResumeProofRead(TObject *Sender);
   void __fastcall PrevProofRead(TObject *Sender);
   void __fastcall ConvertToPDB(TObject *Sender);
   bool __fastcall ConvertMarksVePub();
   bool __fastcall ConvertMarksHorizontal();
   void __fastcall btnConvertMarksHorizontal(TObject *Sender);
   void __fastcall ReplaceCRNL();
   void __fastcall ReplaceCRNLePub();
   void __fastcall WriteXHTML();
   void __fastcall WriteContentOpf();
   void __fastcall WriteTocNcx();
   void __fastcall WriteContainerXml();
   void __fastcall WriteMimeType();
   void __fastcall AddToZipFile(AnsiString name1, AnsiString name2);
   void __fastcall ConvertePub();
   void __fastcall btnConvertePubHClick(TObject *Sender);
   void __fastcall btnConvertePubVClick(TObject *Sender);
   void __fastcall ConvertToTXT(TObject *Sender);
   void __fastcall WriteXHTMLPRC();
   void __fastcall WriteTOCPRC();
   void __fastcall WriteContentOpfPRC();
   void __fastcall WriteTocNcxPRC();
   void __fastcall btnConvertPRCClick(TObject *Sender);

   boolean __fastcall RenameChapters(int head, int tail, int inc);
   void __fastcall SplitChapter(TObject *Sender);
   void __fastcall MergeChapters(TObject *Sender);
   void __fastcall InsertChapterBefore(TObject *Sender);
   void __fastcall InsertChapterAfter(TObject *Sender);
private: // User declarations


public: // User declarations

   __fastcall THome(TComponent* Owner);
   __fastcall virtual ~THome();

#ifdef DEMO
   int __fastcall DemoExpired();
   void __fastcall RegisterApplication();
#endif
 
   };
//---------------------------------------------------------------------------
extern PACKAGE THome *Home;
//---------------------------------------------------------------------------
#endif
