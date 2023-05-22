#include "App/App.h"
#include "_App/Application.h"

int main(int argc, char** args)
{
	Application application;
	if (application.Init())
	{
		application.Run();
	}
	application.Destroy();

	return 0;
}
