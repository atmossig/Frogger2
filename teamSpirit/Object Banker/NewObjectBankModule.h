//---------------------------------------------------------------------------
#ifndef NewObjectBankModuleH
#define NewObjectBankModuleH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TForm2 : public TForm
{
__published:	// IDE-managed Components
  TGroupBox *GroupBox1;
  TGroupBox *GroupBox2;
  TEdit *Edit1;
  TLabel *Label1;
  TEdit *Edit2;
  TLabel *Label2;
  TEdit *Edit3;
  TLabel *Label3;
  TButton *Button1;
  TButton *Button2;
  TButton *Button3;
  TButton *Button4;
  TOpenDialog *OpenDialog1;
  TOpenDialog *OpenDialog2;
  void __fastcall Button4Click(TObject *Sender);
  void __fastcall Button3Click(TObject *Sender);
  void __fastcall Button1Click(TObject *Sender);
  
  void __fastcall Button2Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
  __fastcall TForm2(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm2 *Form2;
//---------------------------------------------------------------------------
#endif
