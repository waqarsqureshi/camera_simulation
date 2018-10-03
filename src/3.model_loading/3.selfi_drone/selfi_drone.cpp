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
#include <learnopengl/feature_selfi.h>
//opencv

#include<opencv2/opencv.hpp>

// csv header


#include <text/csv/ostream.hpp>    
#include "text/csv/istream.hpp"

// dlib header

#include <dlib/svm.h>
#include <dlib/matrix.h>

// c header

#include <fstream>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <iomanip>
#include <sstream>  
#include <string>
//using namespace std;
//using namespace cv;
//using namespace glm;
using namespace dlib;
// This is the object of csv reader
namespace csv = ::text::csv;   

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
int take=0;
int captureFrame=0;
	
typedef matrix<double,7,1> input_sample;// sample measurement size is 5 samples as shown above
// decision function from dlib	
typedef radial_basis_kernel<input_sample> kernel_type;
decision_function<kernel_type> e_posX ;
decision_function<kernel_type> e_posY ;
decision_function<kernel_type> e_posZ ;
decision_function<kernel_type> e_pitch ;
decision_function<kernel_type> e_yaw;
param d;

// hardcoded file paths
string pathToHaar = "/home/user/opencv-3.4.1/data/haarcascades/";
string pathTotestcsv = "/home/user/OBJ2IMG/resources/objects/human2/test_images/test.csv";
string pathToDat = "/home/user/regression_selfi/build/saved_function_4k.dat";//
string pathToObj1 = FileSystem::getPath("resources/objects/human1/CMan0203.obj");
string pathToObj2 = FileSystem::getPath("resources/objects/human2/CMan0206.obj");
string pathToTestImages = FileSystem::getPath("resources/objects/human2/test_images/");
string floorTexturePath = FileSystem::getPath("resources/textures/wood.png");
string wallTexturePath = FileSystem::getPath("resources/textures/image.JPG");
string wallRightlTexturePath = FileSystem::getPath("resources/textures/brickwall.jpg");
string wallLeftTexturePath = FileSystem::getPath("resources/textures/brickwall.jpg");
string pathToTemplateImages = FileSystem::getPath("resources/objects/human2/template_images/data/");
string pathToTemplateImagesSrc = FileSystem::getPath("resources/objects/human2/template_images/src/");


//opencv declaration
cv::Mat img(SCR_HEIGHT, SCR_WIDTH, CV_8UC3);
cv::Mat templ;//(SCR_HEIGHT,SCR_WIDTH,CV_8UC3);// for template read
//cv::Mat template_array[3];// create an array of template images
//variable tos ave size and center of the model
glm::vec3 Center;
glm::vec3 size;

//=========================================
// some helper functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void image_write(cv::Mat img,int i);
void image_show(cv::Mat img,int i);
cv::Mat image_read(int i,bool path);
int charToInt(const char* str);
void takeMeTo_position(param d,decision_function<kernel_type> e_posZ,decision_function<kernel_type> e_posY,decision_function<kernel_type> e_posX,decision_function<kernel_type> e_pitch,decision_function<kernel_type> e_yaw);
//=================================================
                /*
                float x_initial = camera.Position.x;
                float y_initial = camera.Position.y;
                float z_initial = camera.Position.z;
                float pitch_initial = camera.Pitch;
                float yaw_initial = camera.Yaw;
                float x = x_initial;
                float y = y_initial;
                float z = z_initial;
                float pitch = pitch_initial;
                float yaw = yaw_initial;
                float z_final = -100;//e_posZ(S);
                float y_final = 100;//e_posY(S);
                float x_final = 100;//e_posX(S);  
                */    
//=================================================

int main(int argc, char** argv)
{
    if (argc == 1)
    {
        cout << "Give a template image file as arguments to this program." << endl;
        return 0;
    }
    // read the template image
    
    int imageNum =  charToInt(argv[1]);
    //bool path = 
    templ = image_read(imageNum,0);
    namedWindow("Template", WINDOW_NORMAL);
    imshow("Template",templ);
    int key = cv::waitKey(0);
    if (key==27)
    destroyWindow("Template");
    templ = image_read(imageNum,1);
    
    d.desc1=0;d.desc2=0;d.desc3=0;d.desc4=0;d.desc5=0; d.desc6=0;d.desc7=0; // make desc to reset
    if(templ.empty())
    {
        cout << "The argument is not a valid image . We use the predefined desc to estimate camera parameter when T is pressed" << endl;
        return 0;             
    }
    // read the features from the templates
    d =  find_parameters(templ,pathToHaar);
    //if(d.desc2==0 && d.desc3==0 && d.desc4==0 && d.desc5==0 && d.desc6==0 && d.desc7==0) // check if face is detected, and return if No Face
    //{
    //std::cout<<"No Face detected"<<std::endl;
    //return 0;
    //}
    // else cout the parameters and run the rest of simulation
    
    cout<<d.desc1;cout<<": ";
    cout<<d.desc2;cout<<": ";
    cout<<d.desc3;cout<<": ";
    cout<<d.desc4;cout<<": ";
    cout<<d.desc5;cout<<": ";
    cout<<d.desc6;cout<<": ";
    cout<<d.desc7;cout<<endl;
//=========================================================
    std::ofstream fs(pathTotestcsv);
    csv::csv_ostream csvs(fs);
    csvs << "Image" << "X" << "Y"<<"Z"<<"Zoom"<<"Pitch"<<"Yaw"<<csv::endl;
//=========================================================
//read the regressor parameters
    deserialize(pathToDat)>>e_posZ>>e_posY >>e_posX >> e_pitch >>e_yaw;// please check how they are save while serializing
   // >> test_posZ>> test_posY >> test_posX >> test_pitch >> test_yaw; 
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Selfie Simulation", NULL, NULL);
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
    Shader ourShader1("camera_simulation.vs", "camera_simulation.fs"); // model1
    Shader ourShader2("camera_simulation.vs", "camera_simulation.fs"); //model 2
    Shader ourShaderFloor("2.stencil_testing.vs", "2.stencil_testing.fs");//floor shader
    Shader ourShaderWall("2.stencil_testing.vs", "2.stencil_testing.fs");//wall shader
    Shader ourShaderWallRight("2.stencil_testing.vs", "2.stencil_testing.fs");//wall right shader
    Shader ourShaderWallLeft("2.stencil_testing.vs", "2.stencil_testing.fs");//wall left shader
    
    unsigned int floorTexture = loadTexture(floorTexturePath.c_str());
    unsigned int wallTexture = loadTexture(wallTexturePath.c_str());//wallRightlTexturePath
    unsigned int wallRightTexture = loadTexture(wallRightlTexturePath.c_str());
    unsigned int wallLeftTexture = loadTexture(wallLeftTexturePath.c_str());
    ourShaderFloor.use();
    ourShaderFloor.setInt("texture1", 0);
    ourShaderWall.use();
    ourShaderWall.setInt("texture1", 0);
    ourShaderWallRight.use();
    ourShaderWallRight.setInt("texture1", 0);
    ourShaderWallLeft.use();
    ourShaderWallLeft.setInt("texture1", 0);
    // load models
    // -----------
    Model ourModel2(pathToObj2);
    Model ourModel1(pathToObj1);
    //=======================================================================  
    // compute bounding box
    BBox bbox; // make a struct to contain bounding box (BBox is a structure defined in model.h)
    initbbox(&bbox);  
    // get the bounding box
    update_bbox(&ourModel2,&bbox);
    
    size = glm::vec3(bbox.maxx-bbox.minx, bbox.maxy-bbox.miny, bbox.maxz-bbox.minz);// size of the box
    Center = glm::vec3((bbox.maxx+bbox.minx)/2, (bbox.maxy+bbox.miny)/2, (bbox.maxz+bbox.minz)/2); // Center of the box
    //std::cout<<"bbox = "<<bbox.minx<<" "<<bbox.miny<<" "<<bbox.minz<<" "<<bbox.maxx<<" "<<bbox.maxy<<" "<<bbox.maxz<<" "<<std::endl;
    //std::cout<<"Center = "<<Center.x<<" "<<Center.y<<" "<<Center.z<<std::flush;  
    camera.Position.y = Center.y;  // set the camera position to the Center of the model
    //=========================================================
    // draw in model in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glEnable(GL_POLYGON_OFFSET_FILL);
    // enable for better view of cube
    //glPolygonOffset(1, 0);
    //=========================================
    // don't forget to enable shader 1
    ourShader2.use();
    // get and set tranform for the loaded model
    // view/projection transformations fovy, aspect ratio, znear and z far
    glm::vec3 model_position = glm::vec3(0.0f, 0.0f, 160.0f);
    glm::mat4 projection ;
    projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 1000.0f);
    glm::mat4 model;
    model = glm::translate(model, model_position); // translate it down so it's at the Center of the scene
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // rotate
    model = glm::scale(model, glm::vec3(1.0f,1.0f,1.0f));	
    // it's a bit too big for our scene, so scale it down        
    ourShader2.setMat4("model", model);
    // set the view matrix for model and the cube
    glm::mat4 view = camera.GetViewMatrix();
	//set projection and view of the model       
	ourShader2.setMat4("projection", projection);
    ourShader2.setMat4("view", view);
    //ourModel2.Draw(ourShader2);
    //=====================================================
    // don't forget to enable floor shader 
    ourShaderFloor.use();
    ourShaderFloor.setMat4("projection", projection);
    ourShaderFloor.setMat4("model", glm::mat4());
    ourShaderFloor.setMat4("view", view);
    //DrawFloor(&ourShaderFloor,floorTexture);   
    //======================================================
    // don't forget to enable wall shader 
    ourShaderWall.use();
    ourShaderWall.setMat4("projection", projection);
    ourShaderWall.setMat4("model", glm::mat4());
    ourShaderWall.setMat4("view", view);
    //DrawWall(&ourShaderWall,wallTexture);   
    //=======================================================
    // don't forget to enable right wall shader 
    ourShaderWallRight.use();
    ourShaderWallRight.setMat4("projection", projection);
    ourShaderWallRight.setMat4("model", glm::mat4());
    ourShaderWallRight.setMat4("view", view);
    //DrawWallRight(&ourShaderWallRight,wallRightTexture);   
    //=========================================================
    // don't forget to enable left wall shader 
    ourShaderWallLeft.use();
    ourShaderWallLeft.setMat4("projection", projection);
    ourShaderWallLeft.setMat4("model", glm::mat4());
    ourShaderWallLeft.setMat4("view", view);
    //DrawWallLeft(&ourShaderWallLeft,wallLeftTexture);   
    //==========================================================
    
    // render loop   
    while (!glfwWindowShouldClose(window))//|!win.is_closed())
    {     
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;// time in second
        lastFrame = currentFrame;
        // clear the screen to draw
        glClearColor(0.0f, 0.49f, 0.74117f, 1.0f);//0 127 189
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
        
        // process the input
        processInput(window);
        
                float x_initial = camera.Position.x;
                float y_initial = camera.Position.y;
                float z_initial = camera.Position.z;
                float pitch_initial = camera.Pitch;
                float yaw_initial = camera.Yaw;
                float x = x_initial;
                float y = y_initial;
                float z = z_initial;
                float yaw = yaw_initial;
                float pitch = pitch_initial;
                
        if(take==1)
        {        
                // change the camera view to the new position based on the template parameters.
                input_sample S;
                //
                S(0,0) = d.desc1; S(1,0) = d.desc2;
                S(2,0) = d.desc3; S(3,0) = d.desc4;
                S(4,0) = d.desc5; S(5,0) = d.desc6;
                S(6,0) = d.desc7;
/*
        S(0,0) =0.0190476;//						
        S(1,0)= 0.267857;
        S(2,0) = 0.0820312;	
        S(3,0) =0.122449;	
        S(4,0) =0.457143;
        S(5,0) =  -0.178125;
        S(6,0) = -0.227778;
 */               
                float z_final = e_posZ(S);
                float y_final = e_posY(S);
                float x_final = e_posX(S);
                float pitch_final = e_pitch(S);
                yaw = e_yaw(S);
                float delta = (0.1f);//velocity * (deltaTime);// delta is the distance per frame
                int reach [10]= {0,0,0,0,0,0,0,0,0,0};
                int reach_x=2;int reach_y=2; int reach_z=2; int reach_pitch=2;
                
                while(!glfwWindowShouldClose(window))
                {
                    // very important to clear screen here
                    glClearColor(0.0f, 0.49f, 0.74117f, 1.0f);//0 127 189
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
                    //processInput(window);
                    
                        if (x_initial>x_final)
                        {
                                if(x>x_final)
                                {
                                        x = x-delta;
                                        camera.setCameraPosition(x,y,z,pitch,yaw);
                                        view = camera.GetViewMatrix();
                                 }
                                else
                                {    
                                    //do nothing if only x reached desired location
                                    
                                    if(x<=x_final)
                                    {
                                    
                                        //cout<<"A"<<x<<endl;
                                        reach[0]=1;
                                    }
                                }
                
                        }
                        else
                        {
                                if(x<x_final)
                                {
                                        x = x+delta;
                                        camera.setCameraPosition(x,y,z,pitch,yaw);
                                        view = camera.GetViewMatrix();
                                }
                                else
                                {    
                                    //do nothing if only y reached desired location
                                    
                                    if(x>=x_final)
                                    {
                                        
                                       // cout<<"B"<<x<<endl;
                                        reach[1]=1;
                                    }
                                }
                        }
                        if (y_initial>y_final)
                        {
                                if(y>y_final)
                                {
                                        y = y-delta;
                                        camera.setCameraPosition(x,y,z,pitch,yaw);
                                        view = camera.GetViewMatrix();
                                 }
                                else
                                {    
                                    //do nothing if only x reached desired location
                                    
                                    if(y<=y_final)
                                    {
                                        
                                        //cout<<"C"<<x<<endl;
                                        reach[2]=1;
                                    }
                                }
                
                        }
                        else
                        {
                                if(y<y_final)
                                {
                                        y = y+delta;
                                        camera.setCameraPosition(x,y,z,pitch,yaw);
                                        view = camera.GetViewMatrix();
                                }
                                else
                                {    
                                    //do nothing if ony y reached desired location
                                    
                                    if(y>=y_final)
                                    {
                                        
                                       // cout<<"D"<<x<<endl;
                                        reach[3]=1;
                                    }
                                }
                        }
                        if (z_initial>z_final)
                        {
                                if(z>z_final)
                                {
                                        z = z-delta;
                                        camera.setCameraPosition(x,y,z,pitch,yaw);
                                        view = camera.GetViewMatrix();
                                 }
                                else
                                {    
                                    //do nothing if only z reached desired location
                                    
                                    if(z<=z_final)
                                        //
                                    {
                                        
                                        //cout<<"E"<<x<<endl;
                                        reach[4]=1;
                                    }
                                }
                
                        }
                        else
                        {
                                if(z<z_final)
                                {
                                        z = z+delta;
                                        camera.setCameraPosition(x,y,z,pitch,yaw);
                                        view = camera.GetViewMatrix();
                                }
                                else
                                {    
                                    //do nothing if only z reached desired location
                                    
                                    if(z>=z_final)
                                    {
                                        
                                       // cout<<"F"<<x<<endl;
                                        reach[5]=1;
                                    }
                                }
                        }
                        
                        if (pitch_initial>pitch_final)
                        {
                                if(pitch>pitch_final)
                                {
                                        pitch = pitch-delta;
                                        camera.setCameraPosition(x,y,z,pitch,yaw);
                                        view = camera.GetViewMatrix();
                                 }
                                else
                                {    
                                    //do nothing if only z reached desired location
                                    
                                    if(pitch<=pitch_final)
                                    {
                                        
                                        //cout<<"G"<<x<<endl;
                                        reach[6]=1;
                                    }
                                }
                
                        }
                        else
                        {
                                if(pitch<pitch_final)
                                {
                                        pitch = pitch +delta;
                                        camera.setCameraPosition(x,y,z,pitch,yaw);
                                        view = camera.GetViewMatrix();
                                }
                                else
                                {    
                                    //do nothing if only z reached desired location
                                    
                                    if(pitch>=pitch_final)
                                    {
                                        
                                        //cout<<"H"<<x<<endl;
                                        reach[7]=1;
                                    }
                                }
                        }
                        
                        if (x_initial>x_final)
                            reach_x=reach[0];
                        else
                            reach_x=reach[1];
                        if (y_initial>y_final)
                            reach_y=reach[2];
                        else
                            reach_y=reach[3];
                        if (z_initial>z_final)
                            reach_z=reach[4];
                        else
                            reach_z=reach[5];
                        if (pitch_initial>pitch_final)
                            reach_pitch=reach[6];
                        else
                            reach_pitch=reach[7];  
                            
                        if(reach_x==1 && reach_y==1 && reach_z==1)
                        {
                            take=0;
                            break;                         
                        }               
                        //=========================================
                        // don't forget to enable shader 1
                        ourShader2.use();
                        // set the view matrix for model and the cube
                        view = camera.GetViewMatrix();
	                    //set projection and view of the model       
                        ourShader2.setMat4("view", view);
                        ourModel2.Draw(ourShader2);  
                        //=====================================================
                        // don't forget to enable floor shader 
                        ourShaderFloor.use();
                        ourShaderFloor.setMat4("view", view);
                        DrawFloor(&ourShaderFloor,floorTexture);   
                        //======================================================
                        // don't forget to enable wall shader 
                        ourShaderWall.use();
                        ourShaderWall.setMat4("view", view);
                        DrawWall(&ourShaderWall,wallTexture);   
                        //=======================================================
                        // don't forget to enable right wall shader 
                        ourShaderWallRight.use();
                        ourShaderWallRight.setMat4("view", view);
                        DrawWallRight(&ourShaderWallRight,wallRightTexture);   
                        //=========================================================
                        // don't forget to enable left wall shader 
                        ourShaderWallLeft.use();
                        ourShaderWallLeft.setMat4("view", view);
                        DrawWallLeft(&ourShaderWallLeft,wallLeftTexture);   
                        //==========================================================
                                // print camera position
                              
                        std::cout << std::fixed;
                        std::cout << std::setprecision(1);
                        cout<<'\r'<<" x "<<std::setw(5)<< std::setfill('0');
                        cout<<camera.Position.x<<" y "<< std::setw(5)<<camera.Position.y;
                        cout<<" z "<< std::setw(5)<<camera.Position.z;
                        cout<<" zoom "<< std::setw(5)<<camera.Zoom;
                        cout<<" p "<< std::setw(5)<<camera.Pitch<<" y ";
                        cout<< std::setw(5)<<camera.Yaw;
                        cout<<" fps: " <<std::setw(3)<<(1/(deltaTime));
                        cout<<std::endl;
                        //cout<<std::flush;
                        
                        // print x_final position
                        std::cout << std::fixed;
                        std::cout << std::setprecision(1);
                        cout<<'\r'<<" x_final "<<std::setw(5)<< std::setfill('0');
                        cout<<x_final<<" y_final "<< std::setw(5)<<y_final;
                        cout<<" z_final "<< std::setw(5)<<z_final;
                        cout<<" zoom "<< std::setw(5)<<camera.Zoom;
                        cout<<" p "<< std::setw(5)<<pitch_final<<" y ";
                        cout<< std::setw(5)<<yaw;
                        //cout<<std::flush;
                        cout<<std::endl;
                        //-----------------------
                        
                        //cout<<"x "<<x_final<<"y"<<y_final<<"z"<<z_final<<"p"<<pitch_final;
                        //cout<<std::flush;
                        // important to poll events and mouse events for rendering affect
                        glfwSwapBuffers(window);
                        glfwPollEvents();
                }// end of while (1)
                  
	    }//end of if take
	   
//============================================================================================
        else   
        {
            //=====================================================
            // don't forget to enable shader 1
            ourShader2.use();
            // set the view matrix for model and the cube
            view = camera.GetViewMatrix();
            ourShader2.setMat4("view", view);
            ourModel2.Draw(ourShader2);
            //=====================================================
            // don't forget to enable floor shader 
            ourShaderFloor.use();
            ourShaderFloor.setMat4("view", view);
            DrawFloor(&ourShaderFloor,floorTexture);   
            //=======================================================
            // don't forget to enable wall shader 
            ourShaderWall.use();
            ourShaderWall.setMat4("view", view);
            DrawWall(&ourShaderWall,wallTexture);   
            //========================================================
            // don't forget to enable right wall shader 
            ourShaderWallRight.use();
            ourShaderWallRight.setMat4("view", view);
            DrawWallRight(&ourShaderWallRight,wallRightTexture);   
            //=========================================================
            // don't forget to enable left wall shader 
            ourShaderWallLeft.use();
            ourShaderWallLeft.setMat4("view", view);
            DrawWallLeft(&ourShaderWallLeft,wallLeftTexture);   
            //===========================================================
            /* //model 1 shader
            ourShader1.use();
            // get and set tranform for the loaded model
            // view/projection transformations fovy, aspect ratio, znear and z far        
            ourShader1.setMat4("model", model);
	        //set projection and view of the model       
	        ourShader1.setMat4("projection", projection);
            ourShader1.setMat4("view", view);
            ourModel1.Draw(ourShader1);
            */
            //=============================================================
            // print camera position
            
            std::cout << std::fixed;
            std::cout << std::setprecision(1);
            cout<<'\r'<<" x "<<std::setw(5)<< std::setfill('0')<<camera.Position.x<<" y "<< std::setw(5)<<camera.Position.y;
            cout<<" z "<< std::setw(5)<<camera.Position.z;
            cout<<" zoom "<< std::setw(5)<<camera.Zoom;
            cout<<" p "<< std::setw(5)<<camera.Pitch<<" y ";
            cout<< std::setw(5)<<camera.Yaw;
            cout<<" fps: " <<std::setw(3)<<(1/(deltaTime));
            cout<<std::flush;
            //===========================================================================
            // This part of the code graps the view to pixel to be converted to opencv mat
            //----------------------------------------------------------------------------
	        if(captureFrame==1)
	        {
	            //use fast 4-byte alignment (default anyway) if possible
	            glPixelStorei(GL_PACK_ALIGNMENT, (img.step & 3) ? 1 : 4);
	            //set length of one complete row in destination data (doesn't need to equal img.cols)
	            glPixelStorei(GL_PACK_ROW_LENGTH, img.step/img.elemSize());
	            glReadPixels(0, 0, img.cols, img.rows, GL_BGR, GL_UNSIGNED_BYTE, img.data);
	            cv::flip(img, img, 0);
	            image_write(img,iter);
	            //param d_est =  find_parameters(img,pathToHaar);
	            stringstream tempStream;
                tempStream<<setfill('0')<<setw(4)<<iter;
                string imageName = "Image_"+tempStream.str()+".jpg";
                std::cout << std::fixed;
                std::cout << std::setprecision(1);
                csvs<<imageName<<camera.Position.x<<camera.Position.y<<camera.Position.z;
                csvs<<camera.Zoom<<camera.Pitch<<camera.Yaw;
                //csvs<<d_est.desc1<<d_est.desc2<<d_est.desc3<<d_est.desc4<<d_est.desc5;//d.desc1
                csvs<<csv::endl;
                iter++;
                captureFrame =0;
            } 
            //===============================================================================
            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            // -------------------------------------------------------------------------------
            glfwSwapBuffers(window);
            glfwPollEvents();  
        }// else when take=0       
    }// end of main while statement
    
    //=====================================================
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    cout<<"\n"<<endl;
    return 0;
}// end of main function


//=============================================================================================
// process all input: query GLFW whether relevant keys are 
// pressed/released this frame and react accordingly
// --------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) //waqar - not using delta time anymore
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
        {camera.resetCameraPosition(0,Center.y,0);take=0;}
    if(glfwGetKey(window,GLFW_KEY_T) ==GLFW_PRESS)
        {take=1;captureFrame=1;}
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

    //camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// This is a function to write images from the viewport
void image_write(cv::Mat img,int i)
{
   string imagePath, imageName, saveImage, info;
   stringstream tempStream;
   stringstream infoStream;
   std::fixed;
   std::setprecision(1);
   imagePath = pathToTestImages;
   tempStream<<setfill('0')<<setw(4)<<i;
   imageName = "Image_"+tempStream.str()+".jpg";
   
   infoStream << "xyzpyZ:"<<camera.Position.x<<'_'<<camera.Position.y<<'_'<<camera.Position.z<<'_'<<camera.Pitch<<'_'<<camera.Yaw<<'_'<<camera.Zoom;
   info = infoStream.str();

   saveImage = imagePath+imageName;
   //putText( img, info,cv::Point( img.cols/8, img.rows/8),2, 0.5, cv::Scalar(255, 0, 255) );
   cv::imwrite(saveImage,img);
}

//====================================================
// This is a fuction to show images from the viewport
void image_show(cv::Mat img, int i)
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

//=========================================================
// take me to the new position

void takeMeTo_position(param d,decision_function<kernel_type> e_posZ,decision_function<kernel_type> e_posY,decision_function<kernel_type> e_posX,decision_function<kernel_type> e_pitch,decision_function<kernel_type> e_yaw)
{
    input_sample S;
    //
    S(0,0) = d.desc1;S(1,0) = d.desc2;S(2,0) = d.desc3;S(3,0) = d.desc4;S(4,0) = d.desc5;	
    S(5,0) = d.desc6; S(6,0) = d.desc7;
    float z = e_posZ(S);
    float y = e_posY(S);
    float x = e_posX(S);
    float pitch = e_pitch(S);
    float yaw = e_yaw(S);
    //cout<<x;cout<<": ";cout<<y;cout<<": ";cout<<z;cout<<": ";cout<<pitch;cout<<": ";cout<<yaw;cout<<endl;
    camera.setCameraPosition(x,y,z,pitch,yaw);
}
//=====================================

 int compare_float(float f1, float f2) 
 {
  float precision = 0.0001;
  if (((f1 - precision) < f2) && 
      ((f1 + precision) > f2))
   {
    return 1;
   }
  else
   {
    return 0;
   }
 }
//=========================================
// A function to read an image from a specified path if path is zero it can read from one path if path is one
// it can read from another path
cv::Mat image_read(int i,bool path)
{
   string imagePath, imageName, saveImage;
   stringstream tempStream;
   if(path==1)
   {
    imagePath = pathToTemplateImages;// to change template path see the variable declaration
    tempStream<<setfill('0')<<setw(4)<<i;
    imageName = "Image_"+tempStream.str()+".png";
   }
   else
   {
    imagePath = pathToTemplateImagesSrc;
    tempStream<<setfill('0')<<setw(4)<<i;
    imageName = "Image_"+tempStream.str()+".JPG";
   }

   saveImage = imagePath+imageName;
   cv::Mat temp = imread(saveImage);
   return temp;
}

//====================================================

int charToInt(const char* str)
{

char *p;
int num;

errno = 0;
long conv = strtol(str, &p, 10);

// Check for errors: e.g., the string does not represent an integer
// or the integer is larger than int
if (errno != 0 || *p != '\0' || conv > INT_MAX) 
{
    // Put here the handling of the error, like exiting the program with
    // an error message
} 
else 
{
    // No error
    num = conv;    
    return num;
}

}


//=====================================================
