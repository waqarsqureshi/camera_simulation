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

//=====================================================//
// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

//==================================================
void DrawWall(Shader *shader,unsigned int floorTexture)
{
 
        float planeVertices[] = {
        // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
         500.0f, 900.0f,  500.0f,  2.0f, 0.0f,
        -500.0f, 900.0f,  500.0f,  0.0f, 0.0f,
        -500.0f, 0.0f, 500.0f,  0.0f, 2.0f,

         500.0f,  900.0f,  500.0f,  2.0f, 0.0f,
        -500.0f,  0.0f, 500.0f,  0.0f, 2.0f,
         500.0f,  0.0f, 500.0f,  2.0f, 2.0f
        };
        
            // floor plane VAO
    
        unsigned int planeVAO, planeVBO;
        glGenVertexArrays(1, &planeVAO);
        glGenBuffers(1, &planeVBO);
        glBindVertexArray(planeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glBindVertexArray(0);
        shader->use();                
        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0); 
}
//=======================================

void DrawFloor(Shader *shader,unsigned int floorTexture)
{
 
        float planeVertices[] = {
        // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
         500.0f, 0.0f,  500.0f,  2.0f, 0.0f,
        -500.0f, 0.0f,  500.0f,  0.0f, 0.0f,
        -500.0f, 0.0f, -500.0f,  0.0f, 2.0f,

         500.0f, 0.0f,  500.0f,  2.0f, 0.0f,
        -500.0f, 0.0f, -500.0f,  0.0f, 2.0f,
         500.0f, 0.0f, -500.0f,  2.0f, 2.0f
        };
        
            // floor plane VAO
    
        unsigned int planeVAO, planeVBO;
        glGenVertexArrays(1, &planeVAO);
        glGenBuffers(1, &planeVBO);
        glBindVertexArray(planeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glBindVertexArray(0);
        shader->use();                
        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0); 
}
//=======================================
//==================================================
void DrawWallLeft(Shader *shader,unsigned int floorTexture)
{
 
        float planeVertices[] = {
        // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
         -500.0f, 900.0f,  500.0f,  2.0f, 0.0f,
         -500.0f, 900.0f,  -500.0f,  0.0f, 0.0f,
         -500.0f, 0.0f, -500.0f,  0.0f, 2.0f,

         -500.0f,  900.0f,  500.0f,  2.0f, 0.0f,
         -500.0f,  0.0f, -500.0f,  0.0f, 2.0f,
         -500.0f,  0.0f, 500.0f,  2.0f, 2.0f
        };
        
            // floor plane VAO
    
        unsigned int planeVAO, planeVBO;
        glGenVertexArrays(1, &planeVAO);
        glGenBuffers(1, &planeVBO);
        glBindVertexArray(planeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glBindVertexArray(0);
        shader->use();                
        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0); 
}
//==================================================
void DrawWallRight(Shader *shader,unsigned int floorTexture)
{
 
        float planeVertices[] = {
        // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
         500.0f, 900.0f,  500.0f,  2.0f, 0.0f,
         500.0f, 900.0f,  -500.0f,  0.0f, 0.0f,
         500.0f, 0.0f, -500.0f,  0.0f, 2.0f,

         500.0f,  900.0f,  500.0f,  2.0f, 0.0f,
         500.0f,  0.0f, -500.0f,  0.0f, 2.0f,
         500.0f,  0.0f, 500.0f,  2.0f, 2.0f
        };
        
            // floor plane VAO
    
        unsigned int planeVAO, planeVBO;
        glGenVertexArrays(1, &planeVAO);
        glGenBuffers(1, &planeVBO);
        glBindVertexArray(planeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glBindVertexArray(0);
        shader->use();                
        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0); 
}

//======================================================

#endif
