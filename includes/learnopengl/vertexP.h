// this is a class to save a primitive vertex 
// and draw primitives for example line or point

#ifndef VERTEXP_H
#define VERTEXP_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <learnopengl/shader.h> 
#include <learnopengl/model.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;

/*
class vertexP{
public:
        vector<glm::vec3> verticesP;
        unsigned int VAO;
        glm::vec4 color;
        
        vertexP ()
        {

        }
        vertexP(vector<glm::vec3> verticesP, glm::vec4 color)
        {
                this->verticesP = verticesP;
                this->color = color;
                setupVertexP();      
        }
        
        void Draw (Shader shader)
        {
                shader.setVec4( "ourColor", color);
                glBindVertexArray(VAO);
                glDrawArrays(GL_LINE_STRIP,0, 2);
                //glDrawArrays(GL_LINE_STRIP,2, 2);
                //glDrawArrays(GL_LINE_STRIP,3, 2);
                glBindVertexArray(0);

        }
        
private:
        unsigned int VBO;
        //  Functions    //
        // initializes all the buffer objects/arrays     

        void setupVertexP()
        {
                glGenVertexArrays(1, &VAO);
                glGenBuffers(1, &VBO);
                // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
                glBindVertexArray(VAO);
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(verticesP), &verticesP[0], GL_STATIC_DRAW);
                // set the vertex attribute pointers
                // vertex Positions
                
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec4), (void*)0);
                glEnableVertexAttribArray(0);
                // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
                // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
                glBindVertexArray(0);

        }   
};

*/
//=======================================
void DrawRect (Shader *shader,BBox *bbox,glm::vec4 color)
{
        float rectangle [] = {
           bbox->minx, bbox->miny, 0,  // up 
           bbox->minx, bbox->maxy, 0,  // 
           bbox->maxx, bbox->maxy, 0,  // up-r
           bbox->minx, bbox->maxy, 0,   //
           bbox->maxx, bbox->maxy, 0,  // down-r
           bbox->maxx, bbox->miny, 0,
           bbox->maxx, bbox->miny, 0,
           bbox->minx, bbox->miny, 0, 
           
        };
        unsigned int VAO;
        unsigned int VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle), rectangle, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);	// Vertex attributes stay the same
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
        
        shader->setVec4( "ourColor", color);
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINE_LOOP,0, 8);
        //glDrawArrays(GL_LINE_STRIP,2, 2);
        //glDrawArrays(GL_LINE_STRIP,3, 2);
        glBindVertexArray(0);

}
//=======================================

#endif
