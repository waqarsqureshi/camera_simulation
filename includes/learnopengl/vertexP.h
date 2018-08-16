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
//=======================================
void DrawRect (Shader *shader,BBox *bbox,glm::vec4 color)
{
        glm::vec3 center = glm::vec3((bbox->maxx+bbox->minx)/2, (bbox->maxy+bbox->miny)/2, (bbox->maxz+bbox->minz)/2); // center of the box
        float z = center.z; 
        float rectangle [] = {
           bbox->minx, bbox->miny, z,  // up 
           bbox->minx, bbox->maxy, z,  // 
           bbox->maxx, bbox->maxy, z,  // up-r
           bbox->minx, bbox->maxy, z,   //
           bbox->maxx, bbox->maxy, z,  // down-r
           bbox->maxx, bbox->miny, z,
           bbox->maxx, bbox->miny, z,
           bbox->minx, bbox->miny, z,     
        };
        unsigned int VAO;
        unsigned int VBO;
        glLineWidth(5);
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
        glBindVertexArray(0);

}
//=======================================

#endif
