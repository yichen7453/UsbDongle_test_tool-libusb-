// UsbDongle_Test.cpp: �D�n�M���ɡC

#include "stdafx.h"

#include "GUI.h"

using namespace UsbDongle_Test;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	Application::Run(gcnew GUI());

    return 0;
}
