/*
 *
 * Copyright (c) 2019 Texas Instruments Incorporated
 *
 * All rights reserved not granted herein.
 *
 * Limited License.
 *
 * Texas Instruments Incorporated grants a world-wide, royalty-free, non-exclusive
 * license under copyrights and patents it now or hereafter owns or controls to make,
 * have made, use, import, offer to sell and sell ("Utilize") this software subject to the
 * terms herein.  With respect to the foregoing patent license, such license is granted
 * solely to the extent that any such patent is necessary to Utilize the software alone.
 * The patent license shall not apply to any combinations which include this software,
 * other than combinations with devices manufactured by or for TI ("TI Devices").
 * No hardware patent is licensed hereunder.
 *
 * Redistributions must preserve existing copyright notices and reproduce this license
 * (including the above copyright notice and the disclaimer and (if applicable) source
 * code license limitations below) in the documentation and/or other materials provided
 * with the distribution
 *
 * Redistribution and use in binary form, without modification, are permitted provided
 * that the following conditions are met:
 *
 * *       No reverse engineering, decompilation, or disassembly of this software is
 * permitted with respect to any software provided in binary form.
 *
 * *       any redistribution and use are licensed by TI for use only with TI Devices.
 *
 * *       Nothing shall obligate TI to provide you with source code for the software
 * licensed and provided to you in object code.
 *
 * If software source code is provided to you, modification and redistribution of the
 * source code are permitted provided that the following conditions are met:
 *
 * *       any redistribution and use of the source code, including any resulting derivative
 * works, are licensed by TI for use only with TI Devices.
 *
 * *       any redistribution and use of any object code compiled from the source code
 * and any resulting derivative works, are licensed by TI for use only with TI Devices.
 *
 * Neither the name of Texas Instruments Incorporated nor the names of its suppliers
 *
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * DISCLAIMER.
 *
 * THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "render.h"
#include "OGLES2Tools.h"
#include "box.h"

#define GLM_FORCE_RADIANS
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4, glm::ivec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
extern glm::mat4 mProjection;
// Camera matrix
extern glm::mat4 mView;
// Model matrix : an identity matrix (model will be at the origin)
//extern glm::mat4 mModel_bowl;  // Changes for each model !
glm::mat4 mMVP_box;
static const GLushort boxes3d_indices[] = {
                    0, 1, 2, 3, 7, 1, 5, 4, 7, 6, 2, 4, 0, 1
                };

static unsigned int num_boxes = 0;
#define BB_HALF_WIDTH 30
/******************************************************************************
 Defines
******************************************************************************/
// Index to bind the attributes to vertex shaders
#define VERTEX_ARRAY    0
#define NORMAL_ARRAY    1
#define TEXCOORD_ARRAY  2

static const char* vshader = "\
    precision mediump float; \
	attribute vec4 aVertexPosition;\
    uniform mat4 uMVMatrix;\
    void main(void) {\
    gl_Position = uMVMatrix * vec4(((aVertexPosition.x/5.0)), ((aVertexPosition.y/5.0)), (aVertexPosition.z/5.0), 1.0);\
    }";

static const char* fshader = "\
    uniform vec4 uColor; \
    precision mediump float; \
    void main(void) {\
        gl_FragColor = uColor;\
    }";



static const char billboard_vshader[] =
"    attribute vec4 aVertexPosition;\n"
"    attribute vec2 aTextureCoord;\n"
"    varying vec2 oTextureCoord;\n"
"    uniform mat4 uMVMatrix;\n"
"    void main(void) {\n"
"                  gl_Position = uMVMatrix * aVertexPosition;\n"
"               oTextureCoord = aTextureCoord;\n"
"    } \n";

static const char billboard_fshader[] =
#ifndef STANDALONE
" #extension GL_OES_EGL_image_external : require \n"
#endif
" precision mediump float;\n "
#ifndef STANDALONE
" uniform samplerExternalOES uSampler0;\n "
#else
" uniform sampler2D uSampler0;\n "
#endif
" varying vec2 oTextureCoord;\n"
" void main(void) {\n"
"       gl_FragColor = texture2D(uSampler0, oTextureCoord);\n"
"    } \n";


static GLuint billboard_vbo[MAX_BOXES];//Vertex buffer and index buffer
static GLuint billboard_index_vbo;
static int billboard_program;
static GLuint billboard_uniform_mvm;
static GLuint billboard_uniform_sampler;
static GLuint billboard_attr_vertex_pos;
static GLuint billboard_attr_tex_coord;
static unsigned short billboard_index_mesh[] = {0, 1, 2, 0, 2, 3};

int points_program;
GLuint points_textureID[4];
GLuint vertexAttribPosition;
GLint points_mvMatrixOffsetLoc;
GLint box_colorLoc;
GLint points_samplerLoc;
GLuint boxes_vbo[MAX_BOXES];
GLuint boxes_index_vbo[1];
float points_carMouse[5];
float points_delta[5];
float points_carMouseMax[5];
float points_carMouseMin[5];


void billboard_init_shader()
{
    billboard_program = render_createProgram(billboard_vshader, billboard_fshader);
    if(billboard_program == 0)
    {
       perror("Could not create world map program");
    }

    //set the program
    glUseProgram(billboard_program);
    GL_CHECK(glUseProgram);

    //locate uniforms
    billboard_uniform_mvm = glGetUniformLocation(billboard_program, "uMVMatrix");
    GL_CHECK(glGetUniformLocation);

    //locate uniforms
    billboard_uniform_sampler = glGetUniformLocation(billboard_program, "uSampler0");
    GL_CHECK(glGetUniformLocation);

    //locate attributes
    billboard_attr_vertex_pos = glGetAttribLocation(billboard_program, "aVertexPosition");
    GL_CHECK(glGetAttribLocation);

    //locate attributes
    billboard_attr_tex_coord = glGetAttribLocation(billboard_program, "aTextureCoord");
    GL_CHECK(glGetAttribLocation);
}


int boxes_shader_init()
{
    /* Create program and link */
    GLuint uiFragShader, uiVertShader;		// Used to hold the fragment and vertex shader handles

    // Create the fragment shader object
    uiFragShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Load the source code into it
    glShaderSource(uiFragShader, 1, (const char**)&fshader, NULL);
    // Compile the source code
    glCompileShader(uiFragShader);

    // Check if compilation succeeded
    GLint bShaderCompiled;
    glGetShaderiv(uiFragShader, GL_COMPILE_STATUS, &bShaderCompiled);

    if (!bShaderCompiled)
    {
        D_PRINTF("Error in frag shader!\n");
    }
    // Loads the vertex shader in the same way
    uiVertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(uiVertShader, 1, (const char**)&vshader, NULL);

    glCompileShader(uiVertShader);
    glGetShaderiv(uiVertShader, GL_COMPILE_STATUS, &bShaderCompiled);

    if (!bShaderCompiled)
    {
        GLint infoLogLength;
	glGetShaderiv(uiVertShader, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar* strInfoLog = new GLchar[infoLogLength + 1];
	glGetShaderInfoLog(uiVertShader, infoLogLength, NULL, strInfoLog);

        fprintf(stderr, "Compilation error in shader: %s\n", strInfoLog);
        delete[] strInfoLog;

        D_PRINTF("Error: compiling vert shader\n");
    }
    // Create the shader program
    points_program = glCreateProgram();

    // Attach the fragment and vertex shaders to it
    glAttachShader(points_program, uiFragShader);
    glAttachShader(points_program, uiVertShader);

    // Link the program
    glLinkProgram(points_program);

    // Check if linking succeeded in the same way we checked for compilation success
    GLint bLinked;
    glGetProgramiv(points_program, GL_LINK_STATUS, &bLinked);

    //set the program
    glUseProgram(points_program);

    if (!bLinked)
    {
        D_PRINTF("Error: linking prog\n");
    }


    //locate sampler uniforms
    points_mvMatrixOffsetLoc = glGetUniformLocation(points_program, "uMVMatrix");
    GL_CHECK(glGetUniformLocation);

    //locate color uniform
    box_colorLoc = glGetUniformLocation(points_program, "uColor");
    GL_CHECK(glGetUniformLocation);

    //locate attributes
    vertexAttribPosition = glGetAttribLocation(points_program, "aVertexPosition");
    GL_CHECK(glGetAttribLocation);


    /* Setup billboard shaders */
    billboard_init_shader();


    return 0;
}

int boxes_init()
{

	//int i;
    //Shader
    boxes_shader_init();

    glGenBuffers(MAX_BOXES, boxes_vbo);
    glGenBuffers(1, boxes_index_vbo);

    glGenBuffers(MAX_BOXES, billboard_vbo);
    glGenBuffers(1, &billboard_index_vbo);

#if 0
    /* vertex buffer */

    for (i = 0; i < MAX_BOXES; i++)
    {
    	glBindBuffer(GL_ARRAY_BUFFER, boxes_vbo[i]);
    	glBufferData(GL_ARRAY_BUFFER, sizeof(Box3D_f), boxes3d_ongrid, GL_STATIC_DRAW);
    	glVertexAttribPointer(vertexAttribPosition, 3, GL_UNSIGNED_SHORT, GL_FALSE, 0, 0);
    	glEnableVertexAttribArray(vertexAttribPosition);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxes_vbo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(boxes3d_indices), boxes3d_indices, GL_STATIC_DRAW);
#endif
    return 0;
}


void boxes_draw(ObjectBox *object_list, Pose3D_f *pose, GLuint *texYuv)
{

    int i, j;
    int tex, image_x0, image_y0, image_x1, image_y1;
    float inputimage_width = 1280.0f;
    float inputimage_height = 720.0f;
    float x, y, z;
    //float billboard_mesh[20];

    static int billboard_stride = 5;


    x = pose->t.x/5.0;
    y = pose->t.y/5.0;
    z = pose->t.z/5.0;

    float m[16] = {pose->R.xx, pose->R.xy, pose->R.xz, 0,
			    pose->R.yx, pose->R.yy, pose->R.yz, 0,
			    pose->R.zx, pose->R.zy, pose->R.zz, 0,
			    x, y, z, 1};

	glm::mat4 mModel_box = glm::make_mat4(m);

//	glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -80.0f, 0.0f));
//    mModel_box = glm::rotate(mModel_box, degreesToRadians(45), glm::vec3(0.0f, 0.0f, 1.0f));
    mMVP_box        = mProjection * mView * mModel_box;

    glUseProgram(points_program);
    GL_CHECK(glUseProgram);

    glUniformMatrix4fv(points_mvMatrixOffsetLoc, 1, GL_FALSE, &mMVP_box[0][0]);

    //r, g, b, a
    glUniform4f(box_colorLoc, 1.0f, 0.0f, 0.0f, 0.5f);

    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    num_boxes = 0;

    for (i = 0; i < MAX_BOXES; i++)
    {
    	if(object_list[i].age > 0)
    	{
    		glBindBuffer(GL_ARRAY_BUFFER, boxes_vbo[num_boxes]);
    		glBufferData(GL_ARRAY_BUFFER, sizeof(Box3D_f), (void *)&object_list[num_boxes].box, GL_STATIC_DRAW);
    		glVertexAttribPointer(vertexAttribPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
    		glEnableVertexAttribArray(vertexAttribPosition);
    		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxes_index_vbo[0]);
    		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(boxes3d_indices), boxes3d_indices, GL_STATIC_DRAW);
    		glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_SHORT,  0);
    		GL_CHECK(glDrawElements);
    		num_boxes++;
    	}
    }
    glDisable(GL_BLEND);


    glUseProgram(billboard_program);
    GL_CHECK(glUseProgram);

    glm::mat4 mMV_billboard = mView * mModel_box;

    for( i=0; i<3; i++ )
    {
	    for( j=0; j<3; j++ ) {
		    if ( i==j )
			    mMV_billboard[i][j] = 1.0;
		    else
			    mMV_billboard[i][j] = 0.0;
	    }
    }

    glm::mat4 mMVP_billboard = mProjection * mMV_billboard;
    glUniformMatrix4fv(billboard_uniform_mvm, 1, GL_FALSE, &mMVP_billboard[0][0]);
    glUniform1i(billboard_uniform_sampler, 0);
    glActiveTexture(GL_TEXTURE0);


    num_boxes = 0;

    for (i = 0; i < MAX_BOXES; i++)
    {
        if(object_list[i].age > 0)
        {

		int image_width, image_height;

                /* These should be coming from objectbox */
		tex = 0;
		image_x0 = 400;
		image_y0 = 200;
		image_x1 = 600;
		image_y1 = 400;


		image_width = abs(image_x1 - image_x0);
		image_height = abs(image_y1 - image_y0);


		float billboard_mesh[] = {object_list[num_boxes].center.x - BB_HALF_WIDTH,
			object_list[num_boxes].center.y + BB_HALF_WIDTH * image_height/image_width,
			object_list[num_boxes].box.vertex[7].z,
			(float)image_x0/inputimage_width, (float)image_y0/inputimage_height,
			object_list[num_boxes].center.x - BB_HALF_WIDTH,
			object_list[num_boxes].center.y - BB_HALF_WIDTH * image_height/image_width,
			object_list[num_boxes].box.vertex[7].z,
			(float)image_x0/inputimage_width, (float)image_y1/inputimage_height,
			object_list[num_boxes].center.x + BB_HALF_WIDTH,
			object_list[num_boxes].center.y - BB_HALF_WIDTH * image_height/image_width,
			object_list[num_boxes].box.vertex[7].z,
			(float)image_x1/inputimage_width, (float)image_y1/inputimage_height,
			object_list[num_boxes].center.x + BB_HALF_WIDTH,
			object_list[num_boxes].center.y + BB_HALF_WIDTH * image_height/image_width,
			object_list[num_boxes].box.vertex[7].z,
			(float)image_x1/inputimage_width, (float)image_y0/inputimage_height};

                glBindBuffer(GL_ARRAY_BUFFER, billboard_vbo[num_boxes]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(billboard_mesh), billboard_mesh, GL_STATIC_DRAW);
		glVertexAttribPointer(billboard_attr_vertex_pos, 3, GL_FLOAT, GL_FALSE, (billboard_stride)*sizeof(float), 0);
		glVertexAttribPointer(billboard_attr_tex_coord, 2, GL_FLOAT, GL_FALSE, (billboard_stride)*sizeof(float), (GLvoid*)(3*sizeof(float)));
		GL_CHECK(glVertexAttribPointer);

		//Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, billboard_index_vbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(billboard_index_mesh), billboard_index_mesh, GL_STATIC_DRAW);
		GL_CHECK(glBufferData);

		//Enable for the rendering
		glEnableVertexAttribArray(billboard_attr_vertex_pos);
		glEnableVertexAttribArray(billboard_attr_tex_coord);

#ifndef STANDALONE
		glBindTexture(GL_TEXTURE_EXTERNAL_OES, texYuv[tex]);
#else
		glBindTexture(GL_TEXTURE_2D, texYuv[tex]);
#endif
		GL_CHECK(glBindTexture);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT,  0);
		GL_CHECK(glDrawElements);
                num_boxes++;
        }
    }
}

