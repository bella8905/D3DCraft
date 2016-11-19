

#include <windows.h>
#include <iostream>

#include "System.h"

// Function for entry into GUI program
// A way to show a console window for win app.
// - Setting Link - Subsystem to 'Console'
// - We will need a main instead of winmain function as entry point.
int WINAPI wWinMain( _In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow ) {

    CSystem* system = new CSystem( hInstance );
    if( !system ) {
        return 1;
    }

    // Initialize and run the system object.
    bool result = system->Initialize();
    if( result ) {
        system->Run();
    }

    // Shutdown and release the system object.
    system->ShutDown();

    delete system;
    system = 0;

    return 0;

}


int main() {
    // Calling the wWinMain function to start the GUI program
    // Parameters:
    // GetModuleHandle(nullptr) - To get a handle to the current instance
    // nullptr - Previous instance is not needed
    // nullptr - Command line parameters are not needed
    // 1 - To show the window normally
    wWinMain( GetModuleHandle( nullptr ), nullptr, nullptr, 1 );

    system( "pause" );
    return 0;
}


