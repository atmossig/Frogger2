//---------------------------------------------------------------------------
#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------

enum{
OBECHUNK_OBJECT = 1,
OBECHUNK_OBJECTINFO,
OBECHUNK_MESH,
OBECHUNK_MAPPINGICONS,
OBECHUNK_VIEWPORTS,
OBECHUNK_ANIMTIME,
OBECHUNK_VERTEXFRAMES,
OBECHUNK_ANIMSEGMENTS,
OBECHUNK_CONSTRUCTIONGRID,
OBECHUNK_LIGHTINGINFO,
OBECHUNK_HISTORYENTRY,
OBECHUNK_MASTERSORTLIST,
OBECHUNK_SKININFO,
OBECHUNK_SKIN,
OBECHUNK_END =255
};



class TForm1 : public TForm
{
__published:	// IDE-managed Components
  TListBox *ListBox1;
  TButton *Button1;
  TButton *Button2;
  TListBox *ListBox2;
  TButton *Button4;
  TGroupBox *GroupBox1;
  TButton *Button5;
  TOpenDialog *OpenDialog2;
  TButton *Button3;
  TCheckBox *CheckBox1;
  TCheckBox *CheckBox2;
  TTrackBar *TrackBar1;
  TLabel *Label1;
  TLabel *Label2;
  TEdit *Edit1;
  TUpDown *UpDown1;
  TLabel *Label3;
  TCheckBox *CheckBox3;
  TCheckBox *CheckBox4;
  TCheckBox *CheckBox5;
  TCheckBox *CheckBox6;
  TCheckBox *CheckBox7;
  TCheckBox *CheckBox8;
  TCheckBox *CheckBox9;
  TCheckBox *CheckBox10;
  TCheckBox *CheckBox11;
  TCheckBox *CheckBox12;
  TCheckBox *CheckBox13;
  TButton *Button6;
  TProgressBar *ProgressBar1;
  TLabel *Label4;
  TLabel *Label5;
  void __fastcall Button1Click(TObject *Sender);
  void __fastcall Button2Click(TObject *Sender);
  void __fastcall Button4Click(TObject *Sender);
  void __fastcall Button3Click(TObject *Sender);
  void __fastcall ListBox1Click(TObject *Sender);
  void __fastcall TrackBar1Change(TObject *Sender);
  void __fastcall UpDown1Click(TObject *Sender, TUDBtnType Button);
  void __fastcall ListBox2Click(TObject *Sender);
  void __fastcall CheckBox1Click(TObject *Sender);
  void __fastcall CheckBox2Click(TObject *Sender);
  void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
  void __fastcall CheckBox3Click(TObject *Sender);
  void __fastcall CheckBox4Click(TObject *Sender);
  
  void __fastcall CheckBox5Click(TObject *Sender);
  void __fastcall CheckBox7Click(TObject *Sender);
  void __fastcall CheckBox8Click(TObject *Sender);
  void __fastcall CheckBox6Click(TObject *Sender);
  void __fastcall CheckBox9Click(TObject *Sender);
  void __fastcall CheckBox10Click(TObject *Sender);
  void __fastcall CheckBox11Click(TObject *Sender);
  void __fastcall CheckBox12Click(TObject *Sender);
  void __fastcall CheckBox13Click(TObject *Sender);
  void __fastcall Button5Click(TObject *Sender);
  void __fastcall Button6Click(TObject *Sender);
  void __fastcall Edit1Change(TObject *Sender);
  void __fastcall Edit1KeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
  void __fastcall Edit1Enter(TObject *Sender);
  void __fastcall Edit1KeyPress(TObject *Sender, char &Key);
  
  
private:	// User declarations
public:		// User declarations
  __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------

extern AnsiString configFileName;

#endif
