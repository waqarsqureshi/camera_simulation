// this is a class to save a primitive vertex 
// and draw primitives for example line or point

#ifndef VERTEX_H
#define VERTEX_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <learnopengl/shader.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;


class vertexP{
public:
        vector<glm::vec3> verticesP;
        unsigned int VAO;
        glm::vec4 color;
        
        VertexP(vector<glm::vec3> verticesP,glm::vec3 color)
        {
                this->verticesP = verticesP;
                this->color = color;
                setupVertexP();      
        }
        
        void Draw (Shader shader)
        {
                shader.setVec4( "ourColor", color)
                glDrawArrays(GL_LINE_STRIP,0, verticesP.size());
        }
        
private:
        unsigned int VBO;
        /*  Functions    */
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
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (void*)0);
                glEnableVertexAttribArray(0);
                // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
                // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
                glBindVertexArray(0);

        }   
};
#endif
