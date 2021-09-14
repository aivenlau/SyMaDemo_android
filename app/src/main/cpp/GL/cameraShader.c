#include <stdbool.h>
#include "cameraShader.h"

extern int nRotation;
extern bool bRotaHV;
extern const float dataVertex[];
extern const float dataVertex_Flip[];

extern const float dataVertex_90[];

extern const float dataVertex__90[];


extern const float dataTexCoor[];
extern float  *maMVPMatrix;

extern bool bGoble_Flip;

int nDispStyle = 0;           //显示风格
/*
 *   0 正常
 *   1 黑白
 *   2 淡红
 *   3 淡黄
 *   4 淡绿
 *   5 褐色
 *   6 淡蓝
 */

void
bindTexture(GLenum texture_n, GLuint texture_id, GLsizei width, GLsizei height, const void * buffer);

void
drawFrame(void* ins)
{
	Instance * instance = (Instance *)ins;
	if (instance == 0)
    {
			LOGW_EU("%s Program is nullptr return!", __FUNCTION__);
			return;
	}


	//选择着色程序
	glUseProgram(instance->pProgram);
    if(maMVPMatrix!=NULL) {
		glUniformMatrix4fv(instance->maMVPMatrixHandle, 1, GL_FALSE, maMVPMatrix);
	}

	if(bGoble_Flip)
	{
		glVertexAttribPointer(instance->maPositionHandle,
							  3,//GLint size X Y Z
							  GL_FLOAT,//GLenum type
							  GL_FALSE,//GLboolean normalized
							  3 * 4,//GLsizei stride
							  dataVertex_Flip//const GLvoid * ptr
		);

	}
	else {
    	if(!bRotaHV) {

			if (nRotation == 90) {
				glVertexAttribPointer(instance->maPositionHandle,
									  3,//GLint size X Y Z
									  GL_FLOAT,//GLenum type
									  GL_FALSE,//GLboolean normalized
									  3 * 4,//GLsizei stride
									  dataVertex_90//const GLvoid * ptr
				);
			} else if (nRotation == -90) {
				glVertexAttribPointer(instance->maPositionHandle,
									  3,//GLint size X Y Z
									  GL_FLOAT,//GLenum type
									  GL_FALSE,//GLboolean normalized
									  3 * 4,//GLsizei stride
									  dataVertex__90//const GLvoid * ptr
				);
			} else {
				glVertexAttribPointer(instance->maPositionHandle,
									  3,//GLint size X Y Z
									  GL_FLOAT,//GLenum type
									  GL_FALSE,//GLboolean normalized
									  3 * 4,//GLsizei stride
									  dataVertex//const GLvoid * ptr
				);
			}
		}
		else
		{
			if (nRotation == 90) {
				glVertexAttribPointer(instance->maPositionHandle,
									  3,//GLint size X Y Z
									  GL_FLOAT,//GLenum type
									  GL_FALSE,//GLboolean normalized
									  3 * 4,//GLsizei stride
									  dataVertex_90//const GLvoid * ptr
				);
			} else if (nRotation == -90 || nRotation == 270) {
				glVertexAttribPointer(instance->maPositionHandle,
									  3,//GLint size X Y Z
									  GL_FLOAT,//GLenum type
									  GL_FALSE,//GLboolean normalized
									  3 * 4,//GLsizei stride
									  dataVertex__90//const GLvoid * ptr
				);
			}else if (nRotation == 180) {
				glVertexAttribPointer(instance->maPositionHandle,
									  3,//GLint size X Y Z
									  GL_FLOAT,//GLenum type
									  GL_FALSE,//GLboolean normalized
									  3 * 4,//GLsizei stride
									  dataVertex_Flip//const GLvoid * ptr
				);
			}
			else {
				glVertexAttribPointer(instance->maPositionHandle,
									  3,//GLint size X Y Z
									  GL_FLOAT,//GLenum type
									  GL_FALSE,//GLboolean normalized
									  3 * 4,//GLsizei stride
									  dataVertex//const GLvoid * ptr
				);
			}
		}

	}
	//传入顶点纹理坐标
	glVertexAttribPointer(instance->maTexCoorHandle,
							2,//S T
							GL_FLOAT,//GLenum type
							GL_FALSE,//GLboolean normalized
							2 * 4,//GLsizei stride
							dataTexCoor//const GLvoid * ptr
	);
	//激活并绑定纹理
	bindTexture(GL_TEXTURE0, instance->yTexture, instance->pWidth, instance->pHeight, instance->yBuffer);
	bindTexture(GL_TEXTURE1, instance->uTexture, instance->pWidth / 2, instance->pHeight / 2, instance->uBuffer);
	bindTexture(GL_TEXTURE2, instance->vTexture, instance->pWidth / 2, instance->pHeight / 2, instance->vBuffer);
	glUniform1i(instance->myTextureHandle, 0);
	glUniform1i(instance->muTextureHandle, 1);
	glUniform1i(instance->mvTextureHandle, 2);


	glUniform1i(instance->nDispType, nDispStyle);

	//允许顶点数据数组
	glEnableVertexAttribArray(instance->maPositionHandle);
	glEnableVertexAttribArray(instance->maTexCoorHandle);
	//绘制纹理矩形
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

}

void
bindTexture(GLenum texture_n, GLuint texture_id, GLsizei width, GLsizei height, const void * buffer)
{
//	LOGI_EU("texture_n = %x, texture_id = %d, width = %d, height = %d", texture_n, texture_id, width, height);
	//处理纹理
	//		2.绑定纹理
	glActiveTexture(texture_n);//eg:GL_TEXTURE0
	//		1.1绑定纹理id
	glBindTexture(GL_TEXTURE_2D, texture_id);
	//		2.3设置采样模式
	//glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//		1.2输入纹理数据
	glTexImage2D(GL_TEXTURE_2D,
				0,//GLint level
				GL_LUMINANCE,//GLint internalformat
				width,//GLsizei width
				height,// GLsizei height,
				0,//GLint border,
				GL_LUMINANCE,//GLenum format,
				GL_UNSIGNED_BYTE,//GLenum type,
				buffer//const void * pixels
	);
};
