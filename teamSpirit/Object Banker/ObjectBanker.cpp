//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("ObjectBanker.res");
USEFORM("unit1.cpp", Form1);
USEUNIT("ObjBank.cpp");
USEFORM("NewObjectBankModule.cpp", Form2);
USEFORM("PropObjectBankModule.cpp", Form3);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
  try
  {
     Application->Initialize();
     Application->CreateForm(__classid(TForm1), &Form1);
     Application->Run();
  }
  catch (Exception &exception)
  {
     Application->ShowException(&exception);
  }
  return 0;
}
//---------------------------------------------------------------------------
