//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <stdlib.h>
#include <stdio.h>
#include "unit1.h"
#include "ObjBank.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

OBJBANK objBankList [ MAX_OBJBANKS ];

int numObjectBanks = 0;


void SaveConfig ( void )
{
  int fileHandle;

  if ( ( fileHandle = FileCreate ( "x:\\teamspirit\\psxversion\\config.cfg" ) ) != -1 )
  {
    FileWrite ( fileHandle, &(numObjectBanks), sizeof ( numObjectBanks ) );

    for ( register counter = 0; counter < numObjectBanks; counter++ )
    {
      FileWrite ( fileHandle, (char*)&(objBankList [ counter ].bankName ), sizeof ( objBankList [ counter ].bankName ) );
      FileWrite ( fileHandle, (char*)&(objBankList [ counter ].fileName ), sizeof ( objBankList [ counter ].fileName ) );
    }
    // ENDFOR
    FileClose ( fileHandle );
  }
  // ENDIF

}

void LoadConfig ( void )
{
 int fileHandle;

 if ( ( fileHandle = FileOpen ( "x:\\teamspirit\\psxversion\\config.cfg", fmOpenRead ) ) != -1 )
  {
    FileRead ( fileHandle, &(numObjectBanks), sizeof ( numObjectBanks ) );

    for ( register counter = 0; counter < numObjectBanks; counter++ )
    {
      objBankList [ counter ].remove = 0;
      FileRead ( fileHandle, (char*)&( objBankList [ counter ].bankName ), sizeof ( objBankList [ counter ].bankName ) );
      FileRead ( fileHandle, (char*)&( objBankList [ counter ].fileName ), sizeof ( objBankList [ counter ].fileName ) );
//      FileRead ( fileHandle, (char*)&( objBankList [ counter ].outputDir ), sizeof ( objBankList [ counter ].outputDir ) );

      Form1->ListBox1->Items->Add ( objBankList [ counter ].bankName );
    }
    // ENDFOR

    FileClose ( fileHandle );
  }
  // ENDIF
}

void SaveBankFile ( int bankNum )
{
    char dbg[16];
  int fileHandle;
  int newObjectCount = objBankList [ bankNum ].numObjects;

  sprintf(dbg, "%i\n", bankNum);
  OutputDebugString(dbg);
  for ( register counter = 0; counter < objBankList [ bankNum ].numObjects; counter++ )
  {
    if ( objBankList [ bankNum ].objectList [ counter ] .remove )
    {
      OutputDebugString ( "Removing Object" );
      newObjectCount--;
    }
    // ENDIF
  }
  // ENDFOR

  if ( ( fileHandle = FileCreate ( objBankList [ bankNum ].fileName ) ) != -1 )
  {
    FileWrite ( fileHandle, (char*)&(objBankList [ bankNum ].outputDir ), sizeof ( objBankList [ bankNum ].outputDir ) );

    FileWrite ( fileHandle, &(newObjectCount), sizeof ( newObjectCount ) );

    for ( register counter = 0; counter < objBankList[bankNum].numObjects; counter++ )
    {
      if ( objBankList [ bankNum ].objectList [ counter ].remove )
       continue;
      // ENDIF

      FileWrite ( fileHandle, (char*)&( objBankList [ bankNum ].objectList [ counter ].fileName ), sizeof ( objBankList [ bankNum ].objectList [ counter ].fileName ) );
      FileWrite ( fileHandle, &( objBankList [ bankNum ].objectList [ counter ].gamma ), sizeof ( objBankList [ bankNum ].objectList [ counter ].gamma ) );
      FileWrite ( fileHandle, &( objBankList [ bankNum ].objectList [ counter ].shift ), sizeof ( objBankList [ bankNum ].objectList [ counter ].shift ) );
      FileWrite ( fileHandle, &( objBankList [ bankNum ].objectList [ counter ].flags ), sizeof ( objBankList [ bankNum ].objectList [ counter ].flags ) );

//      FileWrite ( fileHandle, (char*)&(objBankList [ counter ].fileName ), sizeof ( objBankList [ counter ].fileName ) );
//      FileWrite ( fileHandle, (char*)&(objBankList [ counter ].outputDir ), sizeof ( objBankList [ counter ].outputDir ) );
    }
    // ENDFOR
    FileClose ( fileHandle );
  }
  // ENDIF
}

void LoadBankFile ( int bankNum )
{
  int index;
  int fileHandle;

  AnsiString newFile;

  newFile = objBankList [ bankNum ].fileName;

  /*while ( ( index = newFile.Pos ( AnsiString ( "\\" ) ) ) )
  {
    newFile.Insert ( AnsiString ( "\\" ), index );
  }
  // ENDWHILE */

  if ( ( fileHandle = FileOpen ( newFile, fmOpenRead ) ) != -1 )
  {
    FileRead ( fileHandle, (char*)&( objBankList [ bankNum ].outputDir ), sizeof ( objBankList [ bankNum ].outputDir ) );

    FileRead ( fileHandle, &( objBankList [ bankNum ].numObjects ), sizeof ( objBankList [ bankNum ].numObjects ) );

    for ( register counter = 0; counter < objBankList [ bankNum ].numObjects; counter++ )
    {
      objBankList [ bankNum ].objectList [ counter ].remove = 0;

      FileRead ( fileHandle, (char*)&( objBankList [ bankNum ].objectList [ counter ].fileName ), sizeof ( objBankList [ bankNum ].objectList [ counter ].fileName ) );

      Form1->ListBox2->Items->Add ( objBankList [ bankNum ].objectList [ counter ].fileName );

      FileRead ( fileHandle, &( objBankList [ bankNum ].objectList [ counter ].gamma ), sizeof ( objBankList [ bankNum ].objectList [ counter ].gamma ) );
      FileRead ( fileHandle, &( objBankList [ bankNum ].objectList [ counter ].shift ), sizeof ( objBankList [ bankNum ].objectList [ counter ].shift ) );
      FileRead ( fileHandle, &( objBankList [ bankNum ].objectList [ counter ].flags ), sizeof ( objBankList [ bankNum ].objectList [ counter ].flags ) );

//      FileWrite ( fileHandle, (char*)&(objBankList [ counter ].fileName ), sizeof ( objBankList [ counter ].fileName ) );
//      FileWrite ( fileHandle, (char*)&(objBankList [ counter ].outputDir ), sizeof ( objBankList [ counter ].outputDir ) );
    }
    // ENDFOR
    FileClose ( fileHandle );
  }
  // ENDIF
}
