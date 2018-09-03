/*
This is a camera simuation program editing from the example of load model 
in Learnopengl.com
The camera is on the origin looking towards in the positive z direction
 i.e. with a yaw of +90 degree. The model is loaded at z=200 at with x,y=0 
 looking with a rotation of 180 degree
*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// learning learnopengl header files
#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include <learnopengl/vertexP.h>
//opencv

#include<opencv2/opencv.hpp>

// csv header


#include <text/csv/ostream.hpp>    
#include "text/csv/istream.hpp"

#include <fstream>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <iomanip>
#include <sstream>  
#include <string>
#include <thread>
//using namespace std;
//using namespace cv;
//using namespace glm;


// settings
const unsigned int SCR_WIDTH = 1280;//camera width and height
const unsigned int SCR_HEIGHT = 720;// camera width an height

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));// initial camera position in x y z direction
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
int iter=0;

//opencv declaration
cv::Mat img(SCR_HEIGHT, SCR_WIDTH, CV_8UC3);

//=========================================
// some helper functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window,glm::vec3 centers);
void image_write(cv::Mat img, Camera camera,int i);
void image_show(cv::Mat img, Camera camera, int i);
void draw_lines();
void estimate_position(Camera * camera);
//=================================================

// This is the object of csv reader
//==============================
namespace csv = ::text::csv;   
//==============================


int main()
{
//=========================================================
    std::ofstream fs("/home/user/OBJ2IMG/resources/objects/human1/images/train.csv");
    csv::csv_ostream csvs(fs);
    csvs << "Image" << "X" << "Y"<<"Z"<<"Zoom"<<"Pitch"<<"Yaw"<<csv::endl;
	
//=========================================================

    
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Camera Simulation", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR,GLFW_CURSOR_NORMAL);// GLFW_CURSOR_DISABLED); normal mouse is more suitable for me.

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);  // Enable lighting
    glEnable(GL_LIGHT0);    // Enable lighting
    // build and compile shaders
    // -------------------------
    Shader ourShader("camera_simulation.vs", "camera_simulation.fs");
    //Shader BboxShader("camera_simulation_2.vs", "camera_simulation_2.fs");//
    Shader rectShader("camera_simulation_3.vs", "camera_simulation_3.fs");//
    // load models
    // -----------
    Model ourModel(FileSystem::getPath("resources/objects/human1/CMan0203.obj"));
    //Model ourModel(FileSystem::getPath("resources/objects/human3/full_body.obj"));
    BBox bbox; // make a struct to contain bounding box (BBox is a structure defined in model.h)
    initbbox(&bbox);  
    //initialize color to draw a square shape
    glm::vec4 vColor = glm::vec4(0.0f,0.0f,1.0f,1.0f);
    // get the bounding box
    update_bbox(&ourModel,&bbox);
    
    glm::vec3 size = glm::vec3(bbox.maxx-bbox.minx, bbox.maxy-bbox.miny, bbox.maxz-bbox.minz);// size of the box
    glm::vec3 center = glm::vec3((bbox.maxx+bbox.minx)/2, (bbox.maxy+bbox.miny)/2, (bbox.maxz+bbox.minz)/2); // center of the box
    //std::cout<<"bbox = "<<bbox.minx<<" "<<bbox.miny<<" "<<bbox.minz<<" "<<bbox.maxx<<" "<<bbox.maxy<<" "<<bbox.maxz<<" "<<std::endl;
    //std::cout<<"center = "<<center.x<<" "<<center.y<<" "<<center.z<<std::flush;  
    camera.Position.y = center.y;  // set the camera position to the center of the model
//============================================================================================
    // upoad a unity cube to draw a bounding box
    Model cube(FileSystem::getPath("resources/objects/human3/bbox2.obj"));
//=========================================================
    // draw in model in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glEnable(GL_POLYGON_OFFSET_FILL);
    // enable for better view of cube
    //glPolygonOffset(1, 0);
//============================================================
// render loop   
    while (!glfwWindowShouldClose(window))//|!win.is_closed())
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window,center);

        // render the background as white
        // ------
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//============================================================================================
        // don't forget to enable shader 1
        ourShader.use();

        // get and set tranform for the loaded model
        // view/projection transformations fovy, aspect ratio, znear and z far
        glm::vec3 model_position = glm::vec3(0.0f, 0.0f, 160.0f);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 600.0f);
        glm::mat4 model;
        model = glm::translate(model, model_position); // translate it down so it's at the center of the scene
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // rotate
        model = glm::scale(model, glm::vec3(1.0f,1.0f,1.0f));	// it's a bit too big for our scene, so scale it down        
        ourShader.setMat4("model", model);
//============================================================================================
        // set the view matrix for model and the cube
        glm::mat4 view = camera.GetViewMatrix();
//=====================================================================================
        // print camera position
        std::cout << std::fixed;
        std::cout << std::setprecision(1);
        cout<<'\r'<<" x "<<std::setw(5)<< std::setfill('0')<<camera.Position.x<<" y "<< std::setw(5)<<camera.Position.y;
        cout<<" z "<< std::setw(5)<<camera.Position.z;
        cout<<" zoom "<< std::setw(5)<<camera.Zoom;
        cout<<" p "<< std::setw(5)<<camera.Pitch<<" y "<< std::setw(5)<<camera.Yaw<<std::flush;
//=================================================================================================
	//set projection and view of the model       
	ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        ourModel.Draw(ourShader);
//================================================================================================        
       /*
       // draw cube in wireframe
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        BboxShader.use()// don't forget to enable shader 2
        glm::mat4 transform;
        //transform = glm::translate(transform, center); //
        transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, 200.0f));
        //transform = glm::scale(transform, size);
        transform = glm::rotate(transform, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // rotate
        transform = glm::scale(transform, glm::vec3(0.1f, 0.1f,0.1f));

        BboxShader.setMat4("model", transform);
        BboxShader.setMat4("projection", projection);
        BboxShader.setMat4("view", view);
        cube.Draw(BboxShader);
        */ 
//====================================================================================================
        /* turn off the bouding box
        rectShader.use();//// don't forget to enable shader 3
        rectShader.setMat4("projection", projection);
        rectShader.setMat4("view", view);
        rectShader.setMat4("model", model);// same as model    
        DrawRect (&rectShader,&bbox,vColor);
        */
//=============================================
// This part of the code graps the view to pixel to be converted to opencv mat
//----------------------------------------------------------------------------
	//use fast 4-byte alignment (default anyway) if possible
	glPixelStorei(GL_PACK_ALIGNMENT, (img.step & 3) ? 1 : 4);

	//set length of one complete row in destination data (doesn't need to equal img.cols)
	glPixelStorei(GL_PACK_ROW_LENGTH, img.step/img.elemSize());
	glReadPixels(0, 0, img.cols, img.rows, GL_BGR, GL_UNSIGNED_BYTE, img.data);
	cv::flip(img, img, 0);

//--------------------------------------------	
	if(capture==1)
	{
	   image_write(img,camera,iter);
	   capture =0;
	   stringstream tempStream;
           tempStream<<setfill('0')<<setw(4)<<iter;
           string imageName = "Image_"+tempStream.str()+".jpg";
           std::cout << std::fixed;
           std::cout << std::setprecision(1);
           csvs<<imageName<<camera.Position.x<<camera.Position.y<<camera.Position.z<<camera.Zoom<<camera.Pitch<<camera.Yaw<<csv::endl;
           iter++;
	} 
	//image_show(img,camera,debug);	

//--------------------------------------------

//==============================================



        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
        //std::this_thread::sleep_for(std::chrono::milliseconds());
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    cout<<"\n"<<endl;
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window,glm::vec3 center) //waqar - not using delta time anymore
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if(glfwGetKey(window,GLFW_KEY_UP) ==GLFW_PRESS)
        camera.ProcessKeyboard(UP_ARROW, deltaTime);
    if(glfwGetKey(window,GLFW_KEY_DOWN) ==GLFW_PRESS)
        camera.ProcessKeyboard(DOWN_ARROW, deltaTime);
    if(glfwGetKey(window,GLFW_KEY_RIGHT) ==GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT_ARROW, deltaTime);
    if(glfwGetKey(window,GLFW_KEY_LEFT) ==GLFW_PRESS)
        camera.ProcessKeyboard(LEFT_ARROW, deltaTime);//GLFW_KEY_SPACE  
    if(glfwGetKey(window,GLFW_KEY_U) ==GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if(glfwGetKey(window,GLFW_KEY_J) ==GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
    if(glfwGetKey(window,GLFW_KEY_R) ==GLFW_PRESS)
        camera.resetCameraPosition(0,center.y,0);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// This is a function to write images from the viewport
void image_write(cv::Mat img, Camera camera,int i)
{
   string imagePath, imageName, saveImage, info;
   stringstream tempStream;
   stringstream infoStream;
   std::fixed;
   std::setprecision(1);
   imagePath = FileSystem::getPath("resources/objects/human1/images/");
   tempStream<<setfill('0')<<setw(4)<<i;
   imageName = "Image_"+tempStream.str()+".jpg";
   
   infoStream << "xyzpyz "<<camera.Position.x<<'_'<<camera.Position.y<<'_'<<camera.Position.z<<'_'<<camera.Pitch<<'_'<<camera.Yaw<<'_'<<camera.Zoom;
   info = infoStream.str();
   saveImage = imagePath+imageName;
   //putText( img, info,cv::Point( img.cols/8, img.rows/8),2, 0.5, cv::Scalar(255, 0, 255) );
   cv::imwrite(saveImage,img);
}

//====================================================
// This is a fuction to show images from the viewport
void image_show(cv::Mat img, Camera camera, int i)
{
   string imagePath, imageName, saveImage;
   stringstream imageStream;
   std::fixed;
   std::setprecision(1);
   imageStream << "Image_"<<camera.Position.x<<'_'<<camera.Position.y<<'_'<<camera.Position.z<<'_'<<camera.Pitch<<'_'<<camera.Yaw<<'_'<<camera.Zoom<<".jpg";
   //imageName = "Image_"+ std::to_string(camera.Position.x)+'_'+ std::to_string(camera.Position.y)+'_'+std::to_string(camera.Position.z)+'_'+std::to_string(camera.Pitch)+'_'+std::to_string(camera.Yaw)+'_'+std::to_string(camera.Zoom)+".jpg";
   
   imageName = imageStream.str();
   putText( img, imageName,cv::Point( img.cols/8, img.rows/8),3, 0.3, cv::Scalar(255, 0, 255) );
   cv::imshow("Image capture",img);
   unsigned char key = cv::waitKey(1);
      if (key == 27)
      {
      //
      }
}


//=======================================


