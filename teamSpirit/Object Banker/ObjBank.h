//---------------------------------------------------------------------------
#ifndef ObjBankH
#define ObjBankH
//---------------------------------------------------------------------------

#define MAX_OBJBANKS 100
#define MAX_OBJECTS  255

typedef struct _OBJECT
{
  char fileName [ 256 ];
  int gamma;
  int shift;
  int flags;
  int remove;
} OBJECT;

typedef struct _OBJBANK
{
  char fileName  [ 256 ];
  char bankName  [ 256 ];
  char outputDir [ 256 ];
  int remove;

  int numObjects;
  OBJECT objectList [ MAX_OBJECTS ];
  
} OBJBANK;

extern OBJBANK objBankList [ MAX_OBJBANKS ];

extern int numObjectBanks;

void LoadConfig ( void );
void SaveConfig ( void );

void SaveBankFile ( int bankNum );
void LoadBankFile ( int bankNum );

#endif
