
#include "SDLGraphicsProgram.h"
int main(int argc, char** argv){

	// Create an instance of an object for a SDLGraphicsProgram
	SDLGraphicsProgram mySDLGraphicsProgram(1280,720);
	// Print out some useful information
	std::string file;
	std::cin>> file;
	mySDLGraphicsProgram.parse(file);
	mySDLGraphicsProgram.getOpenGLVersionInfo();
	mySDLGraphicsProgram.initGL();
	// Run our program forever
	mySDLGraphicsProgram.loop();
	// When our program ends, it will exit scope, the
	// destructor will then be called and clean up the program.
	return 0;
}
