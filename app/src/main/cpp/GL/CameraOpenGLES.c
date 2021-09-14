#include <jni.h>
#include <stdbool.h>






#include "opengles/esUtil.h"
#include "opengles/matrix.h"
#include "cameraShader.h"

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/pixfmt.h"
#include "libavutil/imgutils.h"
#include "libavutil/time.h"
#include "libavutil/error.h"
#include "libavutil/frame.h"
#include "libavutil/opt.h"


#include <GLES2/gl2.h>
#include <GLES3/gl3.h>

#define D_STR(XX)   #XX

//#define   NO_Fuc

#define UNIT 1
#define TEXTURE_COOR_UNIT 1

//顶点着色器脚本代码


GLuint mPboIds[2];                   // IDs of PBO
bool    bInitPbo=false;
int mRowStride=4;//对齐4字节
int mPboSize=1;


const char * codeVertexShader = D_STR(
        attribute vec3 aPosition; //顶点位置
        uniform mat4 uMVPMatrix; //总变换矩阵
        attribute vec2 aTexCoor; // 顶点纹理位置
        varying vec2 vTexCoor;  //用于传递给片元着色器的变量
        void main()
        {
            gl_Position = uMVPMatrix * vec4(aPosition, 1);
            vTexCoor = aTexCoor;
        }
);

//-------------MATH---------------
const char * codeFragShader = D_STR(
        precision mediump float;
        uniform sampler2D yTexture;
        uniform sampler2D uTexture;
        uniform sampler2D vTexture;
        varying vec2 vTexCoor;

        uniform int nType;


        void modifyColor(vec4 color){
            color.r=max(min(color.r,1.0),0.0);
            color.g=max(min(color.g,1.0),0.0);
            color.b=max(min(color.b,1.0),0.0);
            color.a=max(min(color.a,1.0),0.0);
        }

        void main()
        {

        	float y = texture2D(yTexture, vTexCoor).r;
    	    float u = texture2D(uTexture, vTexCoor).r;
         	float v = texture2D(vTexture, vTexCoor).r;

            if(nType==1) {
                u = 0.5;
                v = 0.5;
            }


	        vec3 yuv = vec3(y, u, v);
	        vec3 offset = vec3(0.0 / 255.0, 128.0 / 255.0, 128.0 / 255.0);
	        mat3 mtr = mat3(1.0, 1.0, 1.0, -0.001, -0.39, 2.03, 1.1402, -0.58, 0.001);
	        vec4 curColor = vec4(mtr * (yuv - offset), 1.0);



            //α*A+(1-α)*B

            vec4 mask = vec4(0.0,0.0,0.0,0.0);
            if(nType==2)
            {
                mask = vec4(1.0,0.0,0.0,0.21); //淡红
            }
            else if(nType==3)
            {
                mask = vec4(1.0,1.0,0.0,0.2); //淡黄
            }
            else if(nType==4)
            {
                mask = vec4(0.0,1.0,0.0,0.2); //淡绿
            }
            else if(nType==5)
            {
                mask = vec4(128.0/256.0, 69.0/256.0, 9.0/256.0, 0.5); //褐色
            }
            else if(nType==6)
            {
                mask = vec4(0.0, 0.0, 1.0, 0.20); //淡蓝
            }
            float r = mask.a*mask.r+(1.0-mask.a)*curColor.r;
            float g = mask.a*mask.g+(1.0-mask.a)*curColor.g;
            float b = mask.a*mask.b+(1.0-mask.a)*curColor.b;
            vec4 color1 = vec4(r,g,b,1.0);
            modifyColor(color1);
            gl_FragColor=color1;
        }
);



//渲染顶点坐标数据
const float dataVertex[] =
        {
                -1 * UNIT, 1 * UNIT, 0,
                -1 * UNIT, -1 * UNIT, 0,
                1 * UNIT, 1 * UNIT, 0,
                1 * UNIT, -1 * UNIT, 0
        };
const float dataVertex_Flip[] =
        {
                1 * UNIT, -1 * UNIT, 0,
                1 * UNIT, 1 * UNIT, 0,
                -1 * UNIT, -1 * UNIT, 0,
                -1 * UNIT, 1 * UNIT, 0
        };

const float dataVertex_90[] =
        {
                1 * UNIT, 1 * UNIT, 0,
                -1 * UNIT, 1 * UNIT, 0,
                1 * UNIT, -1 * UNIT, 0,
                -1 * UNIT, -1 * UNIT, 0
        };
const float dataVertex__90[] =
        {
                -1 * UNIT, -1 * UNIT, 0,
                1 * UNIT, -1 * UNIT, 0,
                -1 * UNIT, 1 * UNIT, 0,
                1 * UNIT, 1 * UNIT, 0
        };

//渲染纹理坐标数据
const float dataTexCoor[] =
        {
                0 * TEXTURE_COOR_UNIT, 0 * TEXTURE_COOR_UNIT,
                0 * TEXTURE_COOR_UNIT, 1 * TEXTURE_COOR_UNIT,
                1 * TEXTURE_COOR_UNIT, 0 * TEXTURE_COOR_UNIT,
                1 * TEXTURE_COOR_UNIT, 1 * TEXTURE_COOR_UNIT
        };



AVFrame *gl_Frame = NULL;
//全局变量
Instance *instance =NULL;

int gwwwww_=1;
int ghhhhh_=1;
int gdddddd_=0;

int nRotation=0;
bool  bRotaHV=false;
float * maMVPMatrix = NULL;
void F_InitGPL(int WW,int HH)
{
#ifndef NO_Fuc

	jint pWidth = WW;
	jint pHeight = HH;

    if(maMVPMatrix==NULL)
        maMVPMatrix = getRotateM(NULL, 0, 0, 0, 0, 1);


	if(instance!=NULL)
	{
		free(instance->yBuffer);
		free(instance->uBuffer);
		free(instance->vBuffer);
		instance->yBuffer = 0;
		free(instance);
		instance = 0;
	}
    instance = (Instance *)malloc(sizeof(Instance));
    memset(instance, 0, sizeof(Instance));
    //	1.初始化着色器
    GLuint shaders[2] = {0};

    shaders[0] =(GLuint) initShader(codeVertexShader, GL_VERTEX_SHADER);
    shaders[1] =(GLuint) initShader(codeFragShader, GL_FRAGMENT_SHADER);
    instance->pProgram =(unsigned int) initProgram(shaders, 2);

    instance->maMVPMatrixHandle =(unsigned int) glGetUniformLocation(instance->pProgram, "uMVPMatrix");
    instance->maPositionHandle = (unsigned int)glGetAttribLocation(instance->pProgram, "aPosition");
    instance->maTexCoorHandle = (unsigned int)glGetAttribLocation(instance->pProgram, "aTexCoor");

    instance->myTextureHandle = (unsigned int)glGetUniformLocation(instance->pProgram, "yTexture");
    instance->muTextureHandle = (unsigned int)glGetUniformLocation(instance->pProgram, "uTexture");
    instance->mvTextureHandle = (unsigned int)glGetUniformLocation(instance->pProgram, "vTexture");

    instance->nDispType = (unsigned int)glGetUniformLocation(instance->pProgram, "nType");

    //	2.初始化纹理
    //   2.1生成纹理id
    glGenTextures(1, &instance->yTexture);
    glGenTextures(1, &instance->uTexture);
    glGenTextures(1, &instance->vTexture);


    //	3.分配Yuv数据内存
    instance->yBufferSize = sizeof(char) * pWidth * pHeight;
    instance->uBufferSize = sizeof(char) * pWidth / 2 * pHeight / 2;
    instance->vBufferSize = sizeof(char) * pWidth / 2 * pHeight / 2;
    instance->yBuffer = (char *)malloc(instance->yBufferSize);
    instance->uBuffer = (char *)malloc(instance->uBufferSize);
    instance->vBuffer = (char *)malloc(instance->vBufferSize);
    memset(instance->yBuffer, 0, instance->yBufferSize);
    memset(instance->uBuffer, 128, instance->uBufferSize);
    memset(instance->vBuffer, 128, instance->vBufferSize);
    instance->pHeight =(unsigned int) pHeight;
    instance->pWidth = (unsigned int)pWidth;
 //   LOGI_EU("width = %d, height = %d", instance->pWidth, instance->pHeight);
    //清理背景
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //允许深度检测
//	glEnable(GL_DEPTH_TEST);
 //  LOGI_EU("%s %d error = %d", __FILE__,__LINE__, glGetError());
#endif
}




JNIEXPORT
void
Java_com_joyhonest_wifination_wifination_init(JNIEnv *env, jobject obj)//, jint pWidth, jint pHeight)
{
    LOGI_EU("init()");
    F_InitGPL(1920, 1080);

}

JNIEXPORT
void
Java_com_joyhonest_wifination_wifination_changeLayout(JNIEnv *env, jobject obj, jint width, jint height) {
    LOGI_EU("changeLayout() width = %d, height = %d", width, height);
#ifndef NO_Fuc
    if (instance == 0)
        return;
    if (instance != 0)
    {
        instance->vWidth =(unsigned int) width;
        instance->vHeight = (unsigned int)height;
        float df = ((float)(instance->vHeight))/instance->vWidth;
        ghhhhh_ =  instance->vHeight;
        gwwwww_ =  (int)(ghhhhh_*df);
        gdddddd_ =  (instance->vWidth-gwwwww_)/2;
    }

///  PB0
    mRowStride = (width * 4);
    if (bInitPbo)
    {
        glDeleteBuffers(2, mPboIds);
    }
    mPboSize =mRowStride*height;
    //生成2个PBO
    glGenBuffers(2, mPboIds);

    //绑定到第一个PBO
    glBindBuffer(GL_PIXEL_PACK_BUFFER, mPboIds[0]);
    //设置内存大小
    glBufferData(GL_PIXEL_PACK_BUFFER, mPboSize, NULL,GL_STATIC_READ);
    //绑定到第2个PBO
    glBindBuffer(GL_PIXEL_PACK_BUFFER, mPboIds[1]);
    //设置内存大小
    glBufferData(GL_PIXEL_PACK_BUFFER, mPboSize, NULL,GL_STATIC_READ);
    //解除绑定PBO
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    bInitPbo = true;
///////



    glViewport(0, 0, instance->vWidth, instance->vHeight);
#endif
}

extern pthread_mutex_t m_gl_lock;

extern bool bGoble_3D;


extern bool bWhitClolor;

JNIEXPORT
void
Java_com_joyhonest_wifination_wifination_drawFrame(JNIEnv *env, jobject obj) //, jbyteArray yuvDatas, jint size)
{
#ifndef NO_Fuc
    if (instance == 0)
        return;
    if (gl_Frame == NULL) {
        return;
    }

    if(!bWhitClolor)
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    } else
    {
        glClearColor(0.95f, 0.95f, 0.95f, 1.0f);
    }
    glClear(GL_COLOR_BUFFER_BIT);

    instance->pWidth = (unsigned int) gl_Frame->width;
    instance->pHeight = (unsigned int) gl_Frame->height;
    jbyte *pY = (jbyte *) gl_Frame->data[0];
    jbyte *pU = (jbyte *) gl_Frame->data[1];
    jbyte *pV = (jbyte *) gl_Frame->data[2];

    instance->yBufferSize = (unsigned long) ((unsigned long)gl_Frame->linesize[0] * gl_Frame->height);
    instance->uBufferSize = (unsigned long) ((unsigned long)gl_Frame->linesize[1] * (gl_Frame->height >> 1));
    instance->vBufferSize = (unsigned long) ((unsigned long)gl_Frame->linesize[2] * (gl_Frame->height >> 1));
    memcpy(instance->yBuffer, pY, instance->yBufferSize);
    memcpy(instance->uBuffer, pU, instance->uBufferSize);
    memcpy(instance->vBuffer, pV, instance->uBufferSize);
    //memset(instance->uBuffer, 0, instance->uBufferSize);
    //memset(instance->vBuffer, 0, instance->uBufferSize);
    if(bGoble_3D)
    {
        int ww = instance->vWidth/2;
        int hh = instance->vHeight/2;
        glViewport(0, hh/2, ww, hh);
        drawFrame(instance);
        glViewport(ww, hh/2, ww, hh);
        drawFrame(instance);

    }
    else
    {
        //清除深度缓冲,颜色缓冲
        if(bRotaHV && nRotation != 0 &&  nRotation != 180) {
            glViewport(gdddddd_, 0, gwwwww_, ghhhhh_);
        } else {
            glViewport(0, 0, instance->vWidth, instance->vHeight);
        }

        drawFrame(instance);
    }
#endif
}


void F_Release_OpenGL(void)
{
#ifndef NO_Fuc
    if (instance != 0) {
        free(instance->yBuffer);
        free(instance->uBuffer);
        free(instance->vBuffer);
        instance->yBuffer = 0;
        free(instance);
        instance = 0;

    }
    if(maMVPMatrix==NULL)
    {
        free(maMVPMatrix);
    }
#endif
}

JNIEXPORT
void
Java_com_joyhonest_wifination_wifination_release(JNIEnv *env, jobject obj) {
    LOGI_EU("release()");
    F_Release_OpenGL();
}






