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
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

//opencv

#include<opencv2/opencv.hpp>

// dlib library
#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
//

#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <iomanip>

using namespace dlib;
//using namespace std;
//using namespace cv;
//using namespace glm;


// settings
const unsigned int SCR_WIDTH = 640;
const unsigned int SCR_HEIGHT = 480;

// camera
Camera camera(glm::vec3(0.0f, 100.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
int capture=0;
int i=0;

//opencv declaration
cv::Mat img(SCR_HEIGHT, SCR_WIDTH, CV_8UC3);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void image_write(cv::Mat img, Camera camera,int i);
void image_show(cv::Mat img, Camera camera, int i);

int main()
{
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
    Shader ourShader("1.model_loading.vs", "1.model_loading.fs");

    // load models
    // -----------
    Model ourModel(FileSystem::getPath("resources/objects/human2/CMan0206.obj"));
//    Model ourModel(FileSystem::getPath("resources/objects/human3/full_body.obj"));
//-------------------------------------------------------
//============dlib part===================================

   // image_window win;
    // Load face detection and pose estimation models.
  //  frontal_face_detector detector = get_frontal_face_detector();
  //  shape_predictor pose_model;
  //  deserialize("shape_predictor_68_face_landmarks.dat") >> pose_model;
//--------------------------------------------------------

//=========================================================
    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//=========================================================


    // render loop
//---------------------------------------------------------
   
    while (!glfwWindowShouldClose(window))//|!win.is_closed())
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // view/projection transformations fovy, aspect ratio, znear and z far
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 300.0f);
        glm::mat4 view = camera.GetViewMatrix();
        cout<<'\r'<<" x "<<std::setw(10)<< std::setfill('0')<<camera.Position.x<<" y "<< std::setw(10)<<camera.Position.y;
        cout<<':'<< std::setw(10)<<camera.Position.z;
        cout<<" z "<< std::setw(10)<<camera.Zoom;
        cout<<" p "<< std::setw(10)<<camera.Pitch<<" y "<< std::setw(10)<<camera.Yaw<<std::flush;

	//waqar        
	ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model;
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 200.0f)); // translate it down so it's at the center of the scene
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // rotate
        model = glm::scale(model, glm::vec3(1.0f, 1.0f,1.0f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);
//=============================================
// This part of the code graps the view to pixel to be converted to opencv mat
//----------------------------------------------------------------------------
	//use fast 4-byte alignment (default anyway) if possible
	glPixelStorei(GL_PACK_ALIGNMENT, (img.step & 3) ? 1 : 4);

	//set length of one complete row in destination data (doesn't need to equal img.cols)
	glPixelStorei(GL_PACK_ROW_LENGTH, img.step/img.elemSize());
	glReadPixels(0, 0, img.cols, img.rows, GL_BGR, GL_UNSIGNED_BYTE, img.data);
	cv::flip(img, img, 0);
//------------------------------------------------------------------------------
// Turn OpenCV's Mat into something dlib can deal with.  Note that this just
            // wraps the Mat object, it doesn't copy anything.  So cimg is only valid as
            // long as temp is valid.  Also don't do anything to temp that would cause it
            // to reallocate the memory which stores the image as that will make cimg
            // contain dangling pointers.  This basically means you shouldn't modify temp
            // while using cimg.
            //cv_image<bgr_pixel> cimg(img);

            // Detect faces 
            //std::vector<rectangle> faces = detector(cimg);
            // Find the pose of each face.
            //std::vector<full_object_detection> shapes;
            //for (unsigned long i = 0; i < faces.size(); ++i)
            //    shapes.push_back(pose_model(cimg, faces[i]));

            // Display it all on the screen
            //win.clear_overlay();
            //win.set_image(cimg);
            //win.add_overlay(render_face_detections(shapes));

//--------------------------------------------	
	if(capture==1)
	{
	   image_write(img,camera,i);
	   capture =0;
	   i++;
	} 
	//image_show(img,camera,debug);	

        
//--------------------------------------------

//==============================================



        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    cout<<"\n"<<endl;
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
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
    if(glfwGetKey(window,GLFW_KEY_C) ==GLFW_PRESS)
        capture=1;
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
   imagePath = FileSystem::getPath("resources/objects/human2/images/");
   imageName = "Image_"+std::to_string(i)+".jpg";
   info = std::to_string(camera.Position.x)+'_'+ std::to_string(camera.Position.y)+'_'+std::to_string(camera.Position.z)+'_'+std::to_string(camera.Pitch)+'_'+std::to_string(camera.Yaw)+'_'+std::to_string(camera.Zoom);
   saveImage = imagePath+imageName;
   putText( img, info,cv::Point( img.cols/8, img.rows/8),3, 0.3, cv::Scalar(255, 0, 255) );
   cv::imwrite(saveImage,img);
}

//====================================================
// This is a fuction to show images from the viewport
void image_show(cv::Mat img, Camera camera, int i)
{
   string imagePath, imageName, saveImage;
   imageName = "Image_"+ std::to_string(camera.Position.x)+'_'+ std::to_string(camera.Position.y)+'_'+std::to_string(camera.Position.z)+'_'+std::to_string(camera.Pitch)+'_'+std::to_string(camera.Yaw)+'_'+std::to_string(camera.Zoom)+".jpg";
   putText( img, imageName,cv::Point( img.cols/8, img.rows/8),3, 0.3, cv::Scalar(255, 0, 255) );
   cv::imshow("Image capture",img);
   unsigned char key = cv::waitKey(1);
      if (key == 27)
      {
      //
      }
}

//===================================================


