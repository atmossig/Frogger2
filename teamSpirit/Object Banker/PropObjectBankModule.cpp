//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "ObjBank.h"
#include "PropObjectBankModule.h"
#include "unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm3 *Form3;
//---------------------------------------------------------------------------
__fastcall TForm3::TForm3(TComponent* Owner)
  : TForm(Owner)
{

}
//---------------------------------------------------------------------------

void __fastcall TForm3::Button4Click(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TForm3::Button3Click(TObject *Sender)
{
  sprintf ( objBankList [ Form1->ListBox1->ItemIndex ].bankName, Edit1->Text.c_str() );
  sprintf ( objBankList [ Form1->ListBox1->ItemIndex ].fileName, Edit2->Text.c_str() );
  sprintf ( objBankList [ Form1->ListBox1->ItemIndex ].outputDir, Edit3->Text.c_str() );
  
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TForm3::Button1Click(TObject *Sender)
{
  char *compare;
  char tempChar [ 256 ];

  if ( OpenDialog1->Execute() )
  {
    if ( !( compare = strstr ( OpenDialog1->FileName.c_str(), ".bnk" ) ) )
    {
      sprintf ( tempChar, OpenDialog1->FileName.c_str() );
      strcat ( tempChar, ".bnk" );
      OpenDialog1->FileName = tempChar;
    }
    // ENDIF

    Edit2->Text = OpenDialog1->FileName;
  }
  // ENDIF
}
//---------------------------------------------------------------------------

void __fastcall TForm3::Button2Click(TObject *Sender)
{
  char *compare;
  char tempChar [ 256 ];
  
  if ( OpenDialog2->Execute() )
  {
    if ( !( compare = strstr ( OpenDialog2->FileName.c_str(), ".bff" ) ) )
    {
      sprintf ( tempChar, OpenDialog2->FileName.c_str() );
      strcat ( tempChar, ".bff" );
      OpenDialog2->FileName = tempChar;
    }
    // ENDIF

    Edit3->Text = OpenDialog2->FileName;
  }
  // ENDIF
}
//---------------------------------------------------------------------------



