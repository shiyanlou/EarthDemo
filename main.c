#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <FreeImage.h>

#define  GLUT_WHEEL_UP 3           //定义滚轮操作  
#define  GLUT_WHEEL_DOWN 4  

// 定义Bmp的结构体
struct _AUX_RGBImageRec {
	unsigned long sizeX;		// X方向大小
	unsigned long sizeY;		// y方向大小
	unsigned char *data;		// 数据地址
};

typedef struct _AUX_RGBImageRec AUX_RGBImageRec;

/*
**	定义变量
*/
GLuint  texture[1];         // 存储一个纹理
GLfloat rtri;				// 存储旋转变量,不断改变其值
GLfloat zoom = 1.0f;		// 缩放程度,默认为1,用于方大和缩小
GLfloat xpos = 0.f;			// x方向移动
GLfloat ypos = 0.f;			// y方向移动


/* 
** 利用freeimage加载bmp图像 
** 此函数在Linux系统上可以作为常用util调用
*/
GLboolean LoadBmp(const char* filename,
	AUX_RGBImageRec* texture_image) {

	FREE_IMAGE_FORMAT fifmt = FreeImage_GetFileType(filename, 0);
	FIBITMAP *dib = FreeImage_Load(fifmt, filename, 0);
	dib = FreeImage_ConvertTo24Bits(dib);

	int width = FreeImage_GetWidth(dib);
	int height = FreeImage_GetHeight(dib);

	BYTE *pixels = (BYTE*) FreeImage_GetBits(dib);
	int pix = 0;

	if (texture_image == NULL)
	return FALSE;

	texture_image->data = (BYTE *) malloc(width * height * 3);
	texture_image->sizeX = width;
	texture_image->sizeY = height;

	for (pix = 0; pix < width * height; pix++) {
		texture_image->data[pix * 3 + 0] = pixels[pix * 3 + 2];
		texture_image->data[pix * 3 + 1] = pixels[pix * 3 + 1];
		texture_image->data[pix * 3 + 2] = pixels[pix * 3 + 0];

	}

	FreeImage_Unload(dib);

	return TRUE;
}

int LoadGLTextures()
	// 载入位图(调用上面的代码)并转换成纹理
{
	int Status=FALSE; // 状态指示器
	AUX_RGBImageRec *textureImage; // 创建纹理的存储空间
	textureImage = malloc(sizeof(AUX_RGBImageRec));

	// 载入位图，检查有无错误，如果位图没找到则退出
	if ( LoadBmp("earth.bmp", textureImage) )
	{
		Status=TRUE; // 将 Status 设为 TRUE
		glGenTextures(1, &texture[0]); // 创建纹理
		// 使用来自位图数据生成 的典型纹理
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		// 生成纹理
		glTexImage2D(GL_TEXTURE_2D, 0, 3, textureImage->sizeX, textureImage->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, textureImage->data);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // 线形滤波
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // 线形滤波
	}

	if (textureImage) // 纹理是否存在
	{
		if (textureImage->data) // 纹理图像是否存在
		{
			free(textureImage->data); // 释放纹理图像占用的内存
		}
		free(textureImage); // 释放图像结构
	}

	return Status; // 返回 Status
}

// 初始化opengl
void init(void){
	// 载入文理
    LoadGLTextures();

	glEnable(GL_TEXTURE_2D); 				// 启用纹理映射
	glShadeModel(GL_SMOOTH); 				// 启用阴影平滑
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f); 	// 黑色背景
	glClearDepth(1.0f); 					// 设置深度缓存
	glEnable(GL_DEPTH_TEST); 				// 启用深度测试
	glDepthFunc(GL_LEQUAL); 				// 所作深度测试的类型
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // 真正精细的透视修正
}

/* 当窗口大小发生变化的时候调用 */
void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ( );
	// 利用glOrtho创建一个正交平行的视景体
	if (w <= h){
		glOrtho (-1.5, 1.5, -1.5 * ( GLfloat ) h / ( GLfloat ) w,
			1.5 * ( GLfloat ) h / ( GLfloat ) w, -10.0, 10.0 );
	}
	else{
		glOrtho (-1.5 * ( GLfloat ) w / ( GLfloat ) h,
			1.5 * ( GLfloat ) w / ( GLfloat ) h, -1.5, 1.5, -10.0, 10.0);
	}
	
	glMatrixMode ( GL_MODELVIEW );
	glLoadIdentity ( ) ;
} 

/* 定义对键盘的响应函数 */
void keyboard ( unsigned char key, int x, int y)
{
	/*按Esc键退出*/
	switch (key) {
		case 27:
			exit ( 0 );
			break;
		// 按下+的时候zoom缩放增加
		case '+':
			zoom += 0.03;
			break;
		// 按下-的时候zoom缩放减少
		case '-':
			zoom -= 0.03;
			break;
		// w,s 分别使模型上下移动
		case 'w':
			ypos += 0.03;
			break;
		case 's':
			ypos -= 0.03;
			break;
		// a,d 分别使模型左右移动
		case 'a':
			xpos -= 0.03;
			break;
		case 'd':
			xpos += 0.03;
			break;
	}
} 

void display(void){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清除屏幕和深度缓存
	glLoadIdentity(); // 重置当前的模型观察矩阵
	
	// 此处加上xpos,ypos 产生移动效果
	glTranslatef(0.0f+xpos,0.0f+ypos,-5.0f); // 移入屏幕 5 个单位
	// 此处进行缩放,x,y,z方向均按此比例缩放
	glScalef(zoom, zoom, zoom); 

	glRotatef(rtri,0.0f,1.0f,0.0f); // 绕Y轴旋转
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glBegin(GL_QUADS);//绘制四边形
	GLUquadric* quadricObj=gluNewQuadric(); //gluNewQuadric 创建一个新的二次曲面对象
	gluQuadricTexture(quadricObj,GL_TRUE);
	gluSphere(quadricObj,1,100,100);  //参数1：二次曲面对象指针，参数2：球半径，参数3：Z轴方向片数，经度方向，参数4：Y轴方向片数，维度方向
	gluDeleteQuadric(quadricObj); //gluDeleteQuadric 删除一个二次曲面对象
	glEnd();//结束绘制
	
	// 通知硬件绘制图形
	glFinish();
    return;
}

/* 处理鼠标事件 */
void processMouse(int button, int state, int x, int y)  
{  
	if (state == GLUT_UP && button == GLUT_WHEEL_UP)                    
	{  	// 滚轮向上,表示方大,增加缩放比例
		zoom += 0.02;  
		glutPostRedisplay();  
	}  
	if (state == GLUT_UP && button == GLUT_WHEEL_DOWN)  
	{  
		// 滚轮向下,表示缩小,减少缩放比例
		if(zoom > 0.02){
			// 防止zoom太小
			zoom -= 0.02;  
		}
		
		glutPostRedisplay();  
	}  
}  

// 自动旋转函数,用于修改绘制时所需要的参数
void changeParam( void )
{
	rtri += 0.05f;			// 旋转变量自动加上0.05
	glutPostRedisplay ();
} 



int main(int argc,char *argv[]){
	/* GLUT环境初始化*/
	glutInit (&argc, argv);
	/* 显示模式初始化 */
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	/* 定义窗口大小 */
	glutInitWindowSize(600,600);
	/* 定义窗口位置 */
	glutInitWindowPosition (400, 400);
	/* 显示窗口，窗口标题为执行函数名 */
	glutCreateWindow("EarthDemo");

    /* 调用OpenGL初始化函数 */
	init ( );
	/* 注册OpenGL绘图函数 */
	glutDisplayFunc ( display );
	/* 注册窗口大小改变时的响应函数 */
	glutReshapeFunc ( reshape );
	/* 注册键盘响应函数 */
	glutKeyboardFunc ( keyboard );
	/* 注册鼠标事件 */
	glutMouseFunc( processMouse );
	/* 注册自动旋转的函数 */
	glutIdleFunc( changeParam );	
	/* 进入GLUT消息循环，开始执行程序 */
	glutMainLoop( );

    return 0;
}
