//---------------------------------------------------------------------------
#ifndef PropObjectBankModuleH
#define PropObjectBankModuleH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TForm3 : public TForm
{
__published:	// IDE-managed Components
  TGroupBox *GroupBox1;
  TLabel *Label1;
  TLabel *Label2;
  TButton *Button1;
  TGroupBox *GroupBox2;
  TLabel *Label3;
  TButton *Button2;
  TButton *Button3;
  TButton *Button4;
  TOpenDialog *OpenDialog1;
  TOpenDialog *OpenDialog2;
  TEdit *Edit1;
  TEdit *Edit2;
  TEdit *Edit3;
  void __fastcall Button4Click(TObject *Sender);
  void __fastcall Button3Click(TObject *Sender);
  void __fastcall Button1Click(TObject *Sender);
  void __fastcall Button2Click(TObject *Sender);
  
  
private:	// User declarations
public:		// User declarations
  __fastcall TForm3(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm3 *Form3;
//---------------------------------------------------------------------------
#endif
