//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#pragma hdrstop

#include "ObjBank.h"
#include "NewObjectBankModule.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm2 *Form2;
//---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner)
  : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm2::Button4Click(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------
void __fastcall TForm2::Button3Click(TObject *Sender)
{
  objBankList [ numObjectBanks ].numObjects = 0;

  sprintf ( objBankList [ numObjectBanks ].bankName, Edit1->Text.c_str() );
  sprintf ( objBankList [ numObjectBanks ].fileName, Edit2->Text.c_str() );
  sprintf ( objBankList [ numObjectBanks++ ].outputDir, Edit3->Text.c_str() );

  Close();
}
//---------------------------------------------------------------------------
void __fastcall TForm2::Button1Click(TObject *Sender)
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

void __fastcall TForm2::Button2Click(TObject *Sender)
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
/*The following example uses a button, a string grid, and a Save dialog box on a form. When the button is clicked, the user is prompted for a filename. When the user clicks OK, the contents of the string grid are written to the specified file. Additional information is also written to the file so that it can be read easily with the FileRead function.

#include <dir.h>
void __fastcall TForm1::Button1Click(TObject *Sender)
{
  char szFileName[MAXFILE+4];
  int iFileHandle;
  int iLength;
  if (SaveDialog1->Execute())
  {
    if (FileExists(SaveDialog1->FileName))
    {
      fnsplit(SaveDialog1->FileName.c_str(), 0, 0, szFileName, 0);
      strcat(szFileName, ".BAK");
      RenameFile(SaveDialog1->FileName, szFileName);
    }
    iFileHandle = FileCreate(SaveDialog1->FileName);

    // Write out the number of rows and columns in the grid.
    FileWrite(iFileHandle, (char*)&(StringGrid1->ColCount), sizeof
(StringGrid1->ColCount));
    FileWrite(iFileHandle, (char*)&(StringGrid1->RowCount), sizeof
(StringGrid1->RowCount));
    for (int x=0;x<StringGrid1->ColCount;x++)
    {
      for (int y=0;y<StringGrid1->RowCount;y++)
      {
        // Write out the length of each string, followed by the string itself.

        iLength = StringGrid1->Cells[x][y].Length();
        FileWrite(iFileHandle, (char*)&iLength, sizeof(iLength));
        FileWrite(iFileHandle, StringGrid1->Cells[x][y].c_str(), StringGrid1->Cells[x][y].Length());
      }
    }
    FileClose(iFileHandle);
  }
*/
