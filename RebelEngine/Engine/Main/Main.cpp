#include <cstdlib>

//#if __has_include(<vld.h>)
//#ifdef _DEBUG
//#include <vld.h>
//#endif // DEBUG
//#endif


#include "Application.h"

#include "CoreModules/ModuleRender.h"
#include "Utils/Globals.h"

#include <SDL/SDL.h>
#pragma comment( lib, "External-Libraries/SDL/lib/x86/SDL2.lib" )
#pragma comment( lib, "External-Libraries/SDL/lib/x86/SDL2main.lib" )

#include <Brofiler.h>

enum class MainStates
{
	kMAIN_CREATION,
	kMAIN_START,
	kMAIN_UPDATE,
	kMAIN_FINISH,
	kMAIN_EXIT
};

Application* App = NULL;

int main(int argc, char** argv) {
	
	int main_return = EXIT_FAILURE;
	MainStates state = MainStates::kMAIN_CREATION;

	while (state != MainStates::kMAIN_EXIT) {
		
		BROFILER_FRAME("Rebel Engine main thread");

		switch (state) {

		case MainStates::kMAIN_CREATION:

			App = new Application();
			LOG(_INFO, "Application Creation");
			state = MainStates::kMAIN_START;
			break;

		case MainStates::kMAIN_START:

			LOG(_INFO, "Application Init");
			if (App->Init() == false) {
				LOG(_INFO, "Application Init exits with error");
				state = MainStates::kMAIN_EXIT;
			}
			else {
				state = MainStates::kMAIN_UPDATE;
				LOG(_INFO, "Application Update");
			}

			break;

		case MainStates::kMAIN_UPDATE: {
			int update_return = App->Update();

			if (update_return == UPDATE_ERROR) {
				LOG(_INFO, "Application Update exits with error");
				state = MainStates::kMAIN_EXIT;
			}

			if (update_return == UPDATE_STOP)
				state = MainStates::kMAIN_FINISH;
		}
						break;

		case MainStates::kMAIN_FINISH:

			LOG(_INFO, "Application CleanUp");
			if (App->CleanUp() == false) {
				LOG(_INFO, "Application CleanUp exits with error");
			}
			else main_return = EXIT_SUCCESS;

			state = MainStates::kMAIN_EXIT;

			break;

		}
	}

	delete App;
	return main_return;
}