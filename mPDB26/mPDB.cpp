//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USEFORM("Main.cpp", Home);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        try
        {
                 Application->Initialize();
                 Application->Title = "mPDB";
                 Application->CreateForm(__classid(THome), &Home);
       Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        return 0;
}
//---------------------------------------------------------------------------
