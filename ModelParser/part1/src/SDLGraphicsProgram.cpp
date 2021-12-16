#include "SDLGraphicsProgram.h"

// Initialization function
// Returns a true or false value based on successful completion of setup.
// Takes in dimensions of window.
SDLGraphicsProgram::SDLGraphicsProgram(int w, int h):screenWidth(w),screenHeight(h){
  // Initialization flag
	bool success = true;
  // String to hold any errors that occur.
  std::stringstream errorStream;
  // The window we'll be rendering to
  gWindow = NULL;
  // Render flag

	// Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO)< 0){
		errorStream << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
		success = false;
	}
	else{
		//Use OpenGL 3.3 core
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);


		gWindow = SDL_CreateWindow( "Lab",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                screenWidth,
                                screenHeight,
                                SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );


    if( gWindow == NULL ){
			errorStream << "Window could not be created! SDL Error: " << SDL_GetError() << "\n";
			success = false;
		}


		gContext = SDL_GL_CreateContext( gWindow );
		if( gContext == NULL){
			errorStream << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << "\n";
			success = false;
		}

    // Initialize GLAD Library
    if(!gladLoadGLLoader(SDL_GL_GetProcAddress)){
        errorStream << "Failed to iniitalize GLAD\n";
        success = false;
    }

  }


  if(!success){
    errorStream << "Failed to initialize!\n";
    std::string errors=errorStream.str();
    std::cout << errors << "\n";
  }else{
    std::cout << "No SDL, GLAD, or OpenGL, errors Detected\n\n";
  }

}

	// Proper shutdown and destroy initialized objects
	SDLGraphicsProgram::~SDLGraphicsProgram(){

	SDL_DestroyWindow( gWindow );

	gWindow = NULL;

  glDeleteProgram(shader);
	//Quit SDL subsystems
	SDL_Quit();
}

void SDLGraphicsProgram::parse(std::string file) {
    std::ifstream inFile(file);
    if(inFile.is_open()) {
        std::string line;
        std::vector<int> cache;
        while (getline(inFile, line))
        {
            double num1;
            double num2;
            double num3;

            if(line.at(0) == 'v' && line.at(1) == ' ') {
                sscanf(line.c_str(),"v %lf %lf %lf",&num1,&num2,&num3);
                    vertice.push_back(num1);
                    vertice.push_back(num2);
                    vertice.push_back(num3);
            } else if(line.at(0) == 'v' && line.at(1) == 'n') {
                sscanf(line.c_str(),"vn %lf %lf %lf",&num1,&num2,&num3);
                    vertex_normal.push_back(num1);
                    vertex_normal.push_back(num2);
                    vertex_normal.push_back(num3);
            } else if(line.at(0) == 'f' && line.at(1) == ' ') {
                int f1, f2, f3,f4,f5,f6;
                sscanf(line.c_str(),"f %d//%d %d//%d %d//%d", &f1, &f2, &f3,&f4,&f5,&f6);
                    face.push_back(f1);
                    face.push_back(f2);
                    face.push_back(f3);
                    face.push_back(f4);
                    face.push_back(f5);
                    face.push_back(f6);
            }
        }
    }
    inFile.close();
    transformData();
}

void SDLGraphicsProgram::transformData() {
    for(int i =0; i < face.size();) {
        glm::vec3 verti;
        glm::vec3 verte;
        verti.x = vertice[(face[i] -1) * 3 ];
        verti.y = vertice[(face[i] -1) * 3 +1];
        verti.z = vertice[(face[i] -1) * 3 +2];
        verte.x = vertex_normal[(face[i +1] -1) * 3 ];
        verte.y = vertex_normal[(face[i +1] -1) * 3 +1];
        verte.z = vertex_normal[(face[i +1] -1) * 3 +2];
        vertice_vec3.push_back(verti);
        vertex_normal_vec3.push_back(verte);
        i = i +2;
    }
}
void SDLGraphicsProgram::GenerateBuffers(){


	// VertexArrays
	glGenVertexArrays(1, &VAOId);
	glBindVertexArray(VAOId);

	// Vertex Buffer Object (VBO)
	glGenBuffers(1, &vertexPositionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexPositionBuffer);
		glBufferData(GL_ARRAY_BUFFER, vertice_vec3.size() * sizeof(glm::vec3), &vertice_vec3[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(  0,   // Attribute 0, which will match layout in shader
						    					3,   // size (Number of components (2=x,y)  (3=x,y,z), etc.)
													GL_FLOAT, // Type of data
													GL_FALSE, // Is the data normalized
						    			sizeof(float)*3,
												0
	);

}

// Initialize OpenGL

bool SDLGraphicsProgram::initGL(){
	//Success flag
	bool success = true;

	// Setup geometry
	GenerateBuffers();

  // Setup shaders
  std::string vertexShader = LoadShader("./shaders/vert.glsl");
  std::string fragmentShader = LoadShader("./shaders/frag.glsl");

  shader = CreateShader(vertexShader,fragmentShader);

	glUseProgram(shader);

	return success;
}


// Update OpenGL
void SDLGraphicsProgram::update()
{

}


// Render
void SDLGraphicsProgram::render(){
    // Initialize clear color
    // This is the background of the screen.
    glViewport(0, 0, screenWidth, screenHeight);
    glClearColor( 0.2f, 0.2f, 0.2f, 1.f );

  	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexPositionBuffer);
    glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);
		glDrawArrays(GL_TRIANGLES, 0, vertice_vec3.size() );
    glDisableVertexAttribArray(0);
}



void SDLGraphicsProgram::loop(){
    // If this is quit = 'true' then the program terminates.
    bool quit = false;

    SDL_Event e;
    // Enable text input
    SDL_StartTextInput();
    // While application is running
    while(!quit){

      static int state =0;
      while(SDL_PollEvent( &e ) != 0){
        // User posts an event to quit
        // An example is hitting the "x" in the corner of the window.
        if(e.type == SDL_QUIT ){
          quit = true;
        }
        if(e.type == SDL_KEYDOWN) {
            switch( e.key.keysym.sym )
                {
                    case SDLK_q:
                    return;

                    case SDLK_w:
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    break;

                    case SDLK_1:
                    state =1;
                    break;
            }
		}
				
      }
			update();

      if(state == 1) {
        render();
      }
      SDL_GL_SwapWindow(getSDLWindow());
    }

    SDL_StopTextInput();
}


// Loads a shader
std::string SDLGraphicsProgram::LoadShader(const std::string& fname){
		std::string result;

		std::string line;
		std::ifstream myFile(fname.c_str());

		if(myFile.is_open()){
			while(getline(myFile,line)){
					result += line + '\n';
			}
		}
		// Close file
		myFile.close();
		return result;
}


unsigned int SDLGraphicsProgram::CreateShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource){

    // Create a new program
    unsigned int program = glCreateProgram();
    // Compile our shaders
    unsigned int myVertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int myFragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    // Link our program
    glAttachShader(program,myVertexShader);
    glAttachShader(program,myFragmentShader);
    // Link our programs that have been 'attached'
    glLinkProgram(program);
    glValidateProgram(program);

    glDetachShader(program,myVertexShader);
    glDetachShader(program,myFragmentShader);

    glDeleteShader(myVertexShader);
    glDeleteShader(myFragmentShader);

    if(!CheckLinkStatus(program)){
        std::cout << "ERROR, shader did not link!\n";
    }

    return program;
}


unsigned int SDLGraphicsProgram::CompileShader(unsigned int type, const std::string& source){
  // Compile our shaders
  unsigned int id;

  if(type == GL_VERTEX_SHADER){
    id = glCreateShader(GL_VERTEX_SHADER);
  }else if(type == GL_FRAGMENT_SHADER){
    id = glCreateShader(GL_FRAGMENT_SHADER);
  }
  const char* src = source.c_str();
  // The source of our shader
  glShaderSource(id, 1, &src, nullptr);

  glCompileShader(id);

  // Retrieve the result of our compilation
  int result;

  glGetShaderiv(id, GL_COMPILE_STATUS, &result);
  if(result == GL_FALSE){
      int length;
      glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
      char* errorMessages = new char[length];
      glGetShaderInfoLog(id, length, &length, errorMessages);
      if(type == GL_VERTEX_SHADER){
        std::cout << "ERROR: GL_VERTEX_SHADER compilation failed!\n" << errorMessages << "\n";
      }else if(type == GL_FRAGMENT_SHADER){
        std::cout << "ERROR: GL_FRAGMENT_SHADER compilation failed!\n" << errorMessages << "\n";
      }

      delete[] errorMessages;
      // Delete our broken shader
      glDeleteShader(id);
      return 0;
  }

  return id;
}

// Check to see if linking was successful
bool SDLGraphicsProgram::CheckLinkStatus(GLuint programID){
  // Link our shaders

  unsigned int id;


    int result;

    glGetProgramiv(programID, GL_LINK_STATUS, &result);
    if(result == GL_FALSE){
      int length;
      glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &length);
      char* errorMessages = new char[length];
      glGetProgramInfoLog(programID, length, &length, errorMessages);

      std::cout << "ERROR in linking process " << errorMessages << "\n";
      delete[] errorMessages;
      return false;
    }
    return true;
}


// Get Pointer to Window
SDL_Window* SDLGraphicsProgram::getSDLWindow(){
  return gWindow;
}

// Helper Function to get OpenGL Version Information
void SDLGraphicsProgram::getOpenGLVersionInfo(){
  std::cout << "Vendor: " << glGetString(GL_VENDOR) << "\n";
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";
  std::cout << "Version: " << glGetString(GL_VERSION) << "\n";
  std::cout << "Shading language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
}
