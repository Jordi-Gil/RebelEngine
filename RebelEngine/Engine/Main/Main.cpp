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

enum main_states
{
	MAIN_CREATION,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

Application* App = NULL;

int main(int argc, char** argv) {
	
	int main_return = EXIT_FAILURE;
	main_states state = MAIN_CREATION;

	while (state != MAIN_EXIT) {
		
		BROFILER_FRAME("Rebel Engine main thread");

		switch (state) {

		case MAIN_CREATION:

			App = new Application();
			LOG(_INFO, "Application Creation");
			state = MAIN_START;
			break;

		case MAIN_START:

			LOG(_INFO, "Application Init");
			if (App->Init() == false) {
				LOG(_INFO, "Application Init exits with error");
				state = MAIN_EXIT;
			}
			else {
				state = MAIN_UPDATE;
				LOG(_INFO, "Application Update");
			}

			break;

		case MAIN_UPDATE: {
			int update_return = App->Update();

			if (update_return == UPDATE_ERROR) {
				LOG(_INFO, "Application Update exits with error");
				state = MAIN_EXIT;
			}

			if (update_return == UPDATE_STOP)
				state = MAIN_FINISH;
		}
						break;

		case MAIN_FINISH:

			LOG(_INFO, "Application CleanUp");
			if (App->CleanUp() == false) {
				LOG(_INFO, "Application CleanUp exits with error");
			}
			else main_return = EXIT_SUCCESS;

			state = MAIN_EXIT;

			break;

		}
	}

	delete App;
	return main_return;
}