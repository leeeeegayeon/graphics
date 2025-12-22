#define _CRT_SECURE_NO_WARNINGS
#include <gl/freeglut.h>
#include <iostream>
#include <stdio.h>
#include <math.h>

using namespace std;

#ifndef GL_BGR_EXT
#define GL_BGR_EXT 0x80E0
#endif

#define PI 3.141592f

bool  glindaScene = false;   
float glindaArmAngle = 0.0f;   
float glindaArmDir = 1.0f;   
float wandSpin = 0.0f;       
float spotDelta = 0.03f;    
float glindaSpin = 0.0f;

float eyeX = 0.0f, eyeY = 1.0f, eyeZ = 4.0f;
float atX = 0.0f, atY = 0.2f, atZ = 0.0f;

float glindaPosX = -0.30f, glindaPosZ = 0.3f;
float elphPosX = 0.30f, elphPosZ = 0.3f;

float elphArmAngle = 0.0f;
float elphArmDir = 1.0f;

float upAngle = 0.0f;
float upX = 0.0f, upY = 1.0f, upZ = 0.0f;
float rainbowR = 1.0f, rainbowG = 1.0f, rainbowB = 1.0f;


GLfloat lightPos[4] = { 0.0f, 1.5f, 2.0f, 1.0f };
int     lightType = 1; // 1=point / 3=spotlight

GLfloat lightAmbient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
GLfloat lightDiffuse[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat lightSpecular[4] = { 0.3f, 0.3f, 0.3f, 1.0f };

GLfloat matAmbient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
GLfloat matDiffuse[4] = { 0.8f, 0.5f, 0.2f, 1.0f };
GLfloat matSpecular[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat matEmission[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat matShininess = 10.0f;

// spotlight
GLfloat spotDir[3] = { 0.0f, -1.0f, -1.0f };
float   spotCutoff = 20.0f;

unsigned char header[54];
unsigned int dataPos, imageSize, texWidth, texHeight;

GLuint texGlindaBG = 0;
GLuint texElphabaBG = 0;
GLuint texTwoBG = 0;

int   storyPhase = 0;      
float phaseT = 0.0f; 

float elphSkinMix = 0.0f;

float leftPotMix = 0.0f;   
float rightPotMix = 0.0f;   

float teapotSpin = 0.0f;
float flyT = 0.0f;    
bool  potInHand = false;   

int   lastTime = 0;
const float phaseDur[7] = {
    15.0f, 15.0f, 15.0f,
    15.0f, 15.0f, 15.0f,
    12.0f      
};

unsigned char* loadBMP(char* fname)
{
    FILE* file = fopen(fname, "rb");
    if (!file) return NULL;

    fread(header, 1, 54, file);

    dataPos = *(int*)&(header[0x0A]);
    texWidth = *(int*)&(header[0x12]);
    texHeight = *(int*)&(header[0x16]);
    imageSize = *(int*)&(header[0x22]);

    if (imageSize == 0) imageSize = texWidth * texHeight * 3;
    if (dataPos == 0) dataPos = 54;

    unsigned char* bitmap = new unsigned char[imageSize];
    fread(bitmap, 1, imageSize, file);
    fclose(file);
    return bitmap;
}

int initTexture()
{
    glEnable(GL_TEXTURE_2D);

    unsigned char* bitmap;

    bitmap = loadBMP((char*)"glinda.bmp");
    glGenTextures(1, &texGlindaBG);
    glBindTexture(GL_TEXTURE_2D, texGlindaBG);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0,
        GL_BGR_EXT, GL_UNSIGNED_BYTE, bitmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    delete[] bitmap;

    bitmap = loadBMP((char*)"elphaba.bmp");
    glGenTextures(1, &texElphabaBG);
    glBindTexture(GL_TEXTURE_2D, texElphabaBG);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0,
        GL_BGR_EXT, GL_UNSIGNED_BYTE, bitmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    delete[] bitmap;

    bitmap = loadBMP((char*)"two.bmp");
    glGenTextures(1, &texTwoBG);
    glBindTexture(GL_TEXTURE_2D, texTwoBG);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0,
        GL_BGR_EXT, GL_UNSIGNED_BYTE, bitmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    delete[] bitmap;

    return 0;
}

void drawBox(float x1, float y1, float x2, float y2, float depth)
{
    float halfZ = depth * 0.5f;

    float left = min(x1, x2);
    float right = max(x1, x2);
    float bottom = min(y1, y2);
    float top = max(y1, y2);

    glBegin(GL_QUADS);

    glNormal3f(0, 0, 1);
    glVertex3f(left, bottom, halfZ);
    glVertex3f(right, bottom, halfZ);
    glVertex3f(right, top, halfZ);
    glVertex3f(left, top, halfZ);

    glNormal3f(0, 0, -1);
    glVertex3f(left, bottom, -halfZ);
    glVertex3f(right, bottom, -halfZ);
    glVertex3f(right, top, -halfZ);
    glVertex3f(left, top, -halfZ);

    glNormal3f(-1, 0, 0);
    glVertex3f(left, bottom, -halfZ);
    glVertex3f(left, bottom, halfZ);
    glVertex3f(left, top, halfZ);
    glVertex3f(left, top, -halfZ);

    glNormal3f(1, 0, 0);
    glVertex3f(right, bottom, -halfZ);
    glVertex3f(right, bottom, halfZ);
    glVertex3f(right, top, halfZ);
    glVertex3f(right, top, -halfZ);

    glNormal3f(0, -1, 0);
    glVertex3f(left, bottom, -halfZ);
    glVertex3f(right, bottom, -halfZ);
    glVertex3f(right, bottom, halfZ);
    glVertex3f(left, bottom, halfZ);

    glNormal3f(0, 1, 0);
    glVertex3f(left, top, -halfZ);
    glVertex3f(right, top, -halfZ);
    glVertex3f(right, top, halfZ);
    glVertex3f(left, top, halfZ);

    glEnd();
}

void drawName_I_GA_YEON()
{
    float r = 0.5f, g = 0.8f, b = 1.0f;
    float d = 0.04f;

    glPushMatrix();
    glTranslatef(0.23f, -0.50f, 0.01f);
    glColor3f(r, g, b);
    glutSolidTorus(0.018f, 0.045f, 24, 36);
    glColor3f(1, 1, 1);
    glutWireTorus(0.018f, 0.045f, 12, 18);
    glPopMatrix();

    glColor3f(r, g, b);
    drawBox(0.28f, -0.60f, 0.30f, -0.40f, d);

    drawBox(0.35f, -0.48f, 0.40f, -0.45f, d);
    drawBox(0.40f, -0.54f, 0.43f, -0.45f, d);
    drawBox(0.45f, -0.60f, 0.475f, -0.40f, d);
    drawBox(0.475f, -0.54f, 0.50f, -0.51f, d);

    glPushMatrix();
    glTranslatef(0.625f, -0.515f, 0.01f);
    glColor3f(r, g, b);
    glutSolidTorus(0.018f, 0.045f, 24, 36);
    glColor3f(1, 1, 1);
    glutWireTorus(0.018f, 0.045f, 12, 18);
    glPopMatrix();

    glColor3f(r, g, b);
    drawBox(0.70f, -0.60f, 0.75f, -0.40f, d);
    drawBox(0.65f, -0.49f, 0.70f, -0.46f, d);
    drawBox(0.65f, -0.54f, 0.70f, -0.51f, d);
    drawBox(0.55f, -0.73f, 0.58f, -0.63f, d);
    drawBox(0.58f, -0.73f, 0.75f, -0.68f, d);

    glPopMatrix(); 
}


void drawGlinda()
{
    float d = 0.05f;
    glPushMatrix();

    glTranslatef(-0.70f, -0.20f, 0.0f);

    if (glindaScene && (storyPhase == 0 || storyPhase == 1 || storyPhase == 2 || storyPhase == 3))
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

    glTranslatef(0.70f, 0.20f, 0.0f);

    glColor3f(1, 0.75f, 0.85f);
    drawBox(-0.80f, -0.60f, -0.60f, -0.10f, d);
    drawBox(-0.76f, -0.10f, -0.64f, 0.16f, d);

    glColor3f(1, 0.90f, 0.80f);
    drawBox(-0.71f, 0.16f, -0.69f, 0.22f, d);

    glPushMatrix();
    glTranslatef(-0.70f, 0.30f, 0);
    glutSolidSphere(0.085f, 32, 32);
    glPopMatrix();
    //머리카락
    glPushMatrix();
    glColor3f(0.97f, 0.88f, 0.45f);
    glTranslatef(-0.70f, 0.33f, -0.01f); 
    glScalef(1.7f, 0.7f, 1.0f);
    glutSolidSphere(0.09f, 32, 32);
    glPopMatrix();

    glPushMatrix();
    glColor3f(1.0f, 0.95f, 0.4f);              
    glTranslatef(-0.70f, 0.37f, 0.0f);       
    glutSolidTorus(0.008f, 0.10f, 24, 36);   
    glPopMatrix();

    glColor3f(1.0f, 0.98f, 0.6f);
    drawBox(-0.78f, 0.37f, -0.76f, 0.44f, d);
    drawBox(-0.72f, 0.38f, -0.70f, 0.47f, d);
    drawBox(-0.66f, 0.37f, -0.64f, 0.44f, d);

    glColor3f(1, 0.90f, 0.80f);
    drawBox(-0.80f, -0.20f, -0.76f, 0.12f, d);

    // 팔 + 마법봉
    glPushMatrix();
    glTranslatef(-0.62f, -0.04f, 0);
    glRotatef(glindaArmAngle, 0, 0, 1);
    glTranslatef(0.62f, 0.04f, 0);

    drawBox(-0.64f, -0.20f, -0.60f, 0.12f, d);
    drawBox(-0.64f, -0.24f, -0.60f, -0.20f, d);

    glColor3f(1, 0.80f, 0.95f);
    glBegin(GL_LINES);
    glVertex3f(-0.62f, -0.22f, 0);
    glVertex3f(-0.50f, 0.10f, 0);
    glEnd();

    glPushMatrix();
    glTranslatef(-0.50f, 0.10f, 0);
    glRotatef(wandSpin, 0, 0, 1);
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0); glVertex3f(0.04f, 0.04f, 0);
    glVertex3f(0, 0, 0); glVertex3f(-0.04f, 0.04f, 0);
    glVertex3f(0, 0, 0); glVertex3f(0.04f, -0.04f, 0);
    glVertex3f(0, 0, 0); glVertex3f(-0.04f, -0.04f, 0);
    glEnd();
    glPopMatrix();

    glPopMatrix();

    if (potInHand)
    {
        glPushMatrix();

        glDisable(GL_COLOR_MATERIAL);

        GLfloat potDiff[4] = { 0.1f, 0.8f, 0.3f, 1.0f };  // 초록
        GLfloat potSpec[4] = { 1.0f, 1.0f, 1.0f, 1.0f };  // 강한 specular
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, potDiff);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, potSpec);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 120.0f);

        glTranslatef(-0.70f, -0.10f, 0.08f);
        glRotatef(teapotSpin, 0, 1, 0);
        glScalef(0.4f, 0.4f, 0.4f);

        glutSolidTeapot(0.18f);

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, matShininess);
        glEnable(GL_COLOR_MATERIAL);

        glPopMatrix();
    }
	glPopMatrix();
}

void drawGlindaWall()
{
    if (!texGlindaBG) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texGlindaBG);

    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glTexCoord2f(0, 0); glVertex3f(-0.9f, -0.7f, -0.08f);
    glTexCoord2f(1, 0); glVertex3f(0.9f, -0.7f, -0.08f);
    glTexCoord2f(1, 1); glVertex3f(0.9f, 0.7f, -0.08f);
    glTexCoord2f(0, 1); glVertex3f(-0.9f, 0.7f, -0.08f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void drawElphaba()
{
    float d = 0.05f;
    glPushMatrix();

    glTranslatef(0.77f, -0.25f, 0.0f);

    if (glindaScene && (storyPhase == 1 || storyPhase == 2 || storyPhase == 3))
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);

    glTranslatef(-0.77f, 0.25f, 0.0f);

    // 몸통
    glColor3f(0.05f, 0.15f, 0.06f);
    drawBox(0.72f, -0.58f, 0.82f, 0.05f, d);

    // 초록색에서 살색
    float gR = 0.0f, gG = 0.60f, gB = 0.0f;
    float sR = 1.0f, sG = 0.90f, sB = 0.80f;

    float skinR = gR * (1.0f - elphSkinMix) + sR * elphSkinMix;
    float skinG = gG * (1.0f - elphSkinMix) + sG * elphSkinMix;
    float skinB = gB * (1.0f - elphSkinMix) + sB * elphSkinMix;

    // 왼팔 (고정)
    glColor3f(skinR, skinG, skinB);
    drawBox(0.69f, -0.30f, 0.72f, 0.05f, d);

    glPushMatrix();
    glTranslatef(0.835f, 0.05f, 0.0f);      
    glRotatef(elphArmAngle, 1.0f, 0.0f, 0.0f);
    glTranslatef(-0.835f, -0.05f, 0.0f);
    drawBox(0.82f, -0.30f, 0.85f, 0.05f, d);
    glPopMatrix();

    // 목
    glColor3f(skinR, skinG, skinB);
    drawBox(0.74f, 0.05f, 0.80f, 0.11f, d);

    // 얼굴
    glPushMatrix();
    glColor3f(skinR, skinG, skinB);
    glTranslatef(0.77f, 0.17f, 0);
    glutSolidSphere(0.09f, 32, 32);
    glPopMatrix();

    // 모자/머리
    glPushMatrix();
    glColor3f(0.02f, 0.02f, 0.02f);
    glTranslatef(0.77f, 0.23f, -0.01f);
    glScalef(1.9f, 0.55f, 1.0f);
    glutSolidSphere(0.09f, 32, 32);
    glPopMatrix();

    glColor3f(0.02f, 0.02f, 0.02f);
    glBegin(GL_TRIANGLES);
    glVertex3f(0.75f, 0.26f, 0.02f);
    glVertex3f(0.79f, 0.26f, 0.02f);
    glVertex3f(0.77f, 0.42f, 0.02f);
    glEnd();

    if (potInHand)
    {
        glPushMatrix();

        glDisable(GL_COLOR_MATERIAL);

        GLfloat potDiff[4] = { 1.0f, 0.4f, 0.7f, 1.0f };  
        GLfloat potSpec[4] = { 1.0f, 1.0f, 1.0f, 1.0f };  
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, potDiff);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, potSpec);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 120.0f);

        glTranslatef(0.77f, -0.05f, 0.08f);
        glRotatef(teapotSpin, 0, 1, 0);
        glScalef(0.4f, 0.4f, 0.4f);

        glutSolidTeapot(0.18f);

        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, matShininess);
        glEnable(GL_COLOR_MATERIAL);

        glPopMatrix();
    }
    glPopMatrix();
}

void drawElphabaWall()
{
    if (!texElphabaBG) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texElphabaBG);

    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glTexCoord2f(0, 0); glVertex3f(-0.9f, -0.7f, -0.08f);
    glTexCoord2f(1, 0); glVertex3f(0.9f, -0.7f, -0.08f);
    glTexCoord2f(1, 1); glVertex3f(0.9f, 0.7f, -0.08f);
    glTexCoord2f(0, 1); glVertex3f(-0.9f, 0.7f, -0.08f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void drawTeapots()
{
    glDisable(GL_COLOR_MATERIAL);

    //ambient, emission
    GLfloat amb[4] = { 0.08f, 0.08f, 0.08f, 1.0f };
    GLfloat emi[4] = { 0.0f,  0.0f,  0.0f,  1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emi);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, matShininess);

    if (!potInHand)
    {
        // 흰색에서 초록색
        float baseR1 = 1.0f, baseG1 = 1.0f, baseB1 = 1.0f;
        float greenR = 0.1f, greenG = 0.8f, greenB = 0.3f;

        float leftR = baseR1 * (1.0f - leftPotMix) + greenR * leftPotMix;
        float leftG = baseG1 * (1.0f - leftPotMix) + greenG * leftPotMix;
        float leftB = baseB1 * (1.0f - leftPotMix) + greenB * leftPotMix;

        GLfloat diffL[4] = { leftR, leftG, leftB, 1.0f };
        GLfloat specL[4] = { leftR * 0.8f, leftG * 0.8f, leftB * 0.8f, 1.0f };

        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffL);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specL);

        glPushMatrix();
        glTranslatef(-0.22f, -0.65f + 0.2f * flyT, 0);
        glRotatef(teapotSpin, 0, 1, 0);
        glutSolidTeapot(0.18f);
        glPopMatrix();

        //흰색에서 핑크색
        float baseR2 = 1.0f, baseG2 = 1.0f, baseB2 = 1.0f;
        float pinkR = 1.0f, pinkG = 0.4f, pinkB = 0.7f;

        float rightR = baseR2 * (1.0f - rightPotMix) + pinkR * rightPotMix;
        float rightG = baseG2 * (1.0f - rightPotMix) + pinkG * rightPotMix;
        float rightB = baseB2 * (1.0f - rightPotMix) + pinkB * rightPotMix;

        GLfloat diffR[4] = { rightR, rightG, rightB, 1.0f };
        GLfloat specR[4] = { rightR * 0.8f, rightG * 0.8f, rightB * 0.8f, 1.0f };

        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffR);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specR);

        glPushMatrix();
        glTranslatef(0.15f, -0.65f + 0.2f * flyT, 0);
        glRotatef(teapotSpin, 0, 1, 0);
        glutSolidTeapot(0.18f);
        glPopMatrix();
    }

    glEnable(GL_COLOR_MATERIAL);
}


void drawTwoWall()
{
    if (!texTwoBG) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texTwoBG);

    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glTexCoord2f(0, 0); glVertex3f(-0.9f, -0.7f, -0.08f);
    glTexCoord2f(1, 0); glVertex3f(0.9f, -0.7f, -0.08f);
    glTexCoord2f(1, 1); glVertex3f(0.9f, 0.7f, -0.08f);
    glTexCoord2f(0, 1); glVertex3f(-0.9f, 0.7f, -0.08f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

int projMode = 1;

void setProjection()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (projMode == 0)
        glOrtho(-2, 2, -1.5, 1.5, 1, 20);
    else
        glFrustum(-1, 1, -0.75, 0.75, 1, 20);

    glMatrixMode(GL_MODELVIEW);
}

void init()
{
    glClearColor(1, 1, 1, 1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glShadeModel(GL_SMOOTH);

    projMode = 1;
    setProjection();

    eyeX = 0.0f; eyeY = 0.8f; eyeZ = 4.0f;
    atX = 0.0f; atY = -0.10f; atZ = 0.0f;
    upAngle = 0.0f;

    initTexture();

    lastTime = glutGet(GLUT_ELAPSED_TIME);
}

void display()
{
    if (storyPhase == 6)
    {
        float total = phaseDur[6];
        float endStart = total * 0.8f;        
        if (phaseT < endStart)
        {
            glClearColor(0.05f, 0.05f, 0.07f, 1.0f);   
        }
        else
        {
            float fade = (phaseT - endStart) / (total - endStart);
            if (fade > 1.0f) fade = 1.0f;

            float bg = 0.05f + (1.0f - 0.05f) * fade;
            glClearColor(bg, bg, bg, 1.0f);      
        }
    }
    else
    {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    upX = sinf(upAngle);
    upY = cosf(upAngle);
    upZ = 0.0f;

    if (glindaScene)
    {
        gluLookAt(eyeX, eyeY, eyeZ,
            atX, atY, atZ,
            upX, upY, upZ);
    }
    else
    {
        gluLookAt(0.0f, 0.8f, 4.0f,
            0.0f, -0.10f, 0.0f,
            0.0f, 1.0f, 0.0f);
    }

    if (lightType == 1)
    {
        glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.0f);
    }
    else
    {
        glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDir);
        glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, spotCutoff);
        glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 25.0f);
    }

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    // Material
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, matEmission);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, matShininess);

    glPushMatrix();
    glDisable(GL_LIGHTING);
    glTranslatef(lightPos[0], lightPos[1], lightPos[2]);
    if (lightType == 1) glColor3f(1.0f, 1.0f, 0.0f);
    else                glColor3f(0.4f, 1.0f, 0.8f);
    glutSolidCube(0.15f);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    bool drawGlindaW = false;
    bool drawElphW = false;
    bool drawTwoW = false;
    bool drawTeapot = false;
    bool drawGlindaC = false;
    bool drawElphC = false;

    if (!glindaScene)
    {
        drawGlindaW = drawElphW = drawTwoW = true;
        drawTeapot = drawGlindaC = drawElphC = true;
    }
    else
    {
        switch (storyPhase)
        {
        case 0: 
            drawGlindaW = true;
            drawGlindaC = true;
            break;

        case 1: 
        case 2: 
        case 3: 
            drawGlindaW = true;
            drawElphW = true;
            drawGlindaC = true;
            drawElphC = true;
            break;

        case 4: 
        case 5: 
            drawGlindaW = true;
            drawElphW = true;
            drawTwoW = true;
            drawTeapot = true;
            drawGlindaC = true;
            drawElphC = true;
            break;

        case 6: 
            drawGlindaW = true;
            drawElphW = true;
            drawTwoW = true;
            drawTeapot = true;
            drawGlindaC = true;
            drawElphC = true;
            break;

        }
    }

    glPushMatrix();

    // 이름
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 1.4f);        
    glTranslatef(-0.50f, 0.55f, 0.0f);  
    glScalef(1.2f, 1.2f, 1.0f);

    if (storyPhase == 6)
    {
        float nameE = 0.4f + 0.4f * fabs(sinf(phaseT * 4.0f));
        GLfloat nameEmi[4] = { rainbowR * nameE, rainbowG * nameE, rainbowB * nameE, 1.0f };
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, nameEmi);
    }
    drawName_I_GA_YEON();
    if (storyPhase == 6)
    {
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, matEmission);
    }

    glPopMatrix();


    // 왼쪽 벽
    if (drawGlindaW)
    {
        glPushMatrix();
        glTranslatef(-1.4f, 0.0f, 0.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        drawGlindaWall();
        glPopMatrix();
    }

    // 오른쪽 벽
    if (drawElphW)
    {
        glPushMatrix();
        glTranslatef(1.4f, 0.0f, 0.0f);
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
        drawElphabaWall();
        glPopMatrix();
    }

    // 뒤쪽 벽
    if (drawTwoW)
    {
        glPushMatrix();
        glTranslatef(0.0f, 0.0f, -1.4f);
        drawTwoWall();
        if (drawTeapot) drawTeapots();
        glPopMatrix();
    }

    // 글린다
    if (drawGlindaC)
    {
        GLfloat glindaEmiOff[4] = { 0,0,0,1 };

        glPushMatrix();
        glTranslatef(glindaPosX, 0.0f, glindaPosZ);

        float bob = 0.02f * sinf(glindaSpin * PI / 90.0f);
        glTranslatef(0.0f, bob, 0.0f);

        GLfloat glindaEmiOn[4] = {
            0.25f + 0.15f * sinf(glindaSpin * PI / 60.0f),
            0.22f,
            0.32f,
            1.0f
        };

        if (glindaScene)
            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, glindaEmiOn);

        drawGlinda();

        if (glindaScene)
            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, glindaEmiOff);

        glPopMatrix();
    }

    // 엘파바
    if (drawElphC)
    {
        glPushMatrix();
        glTranslatef(elphPosX, 0.0f, elphPosZ);
        drawElphaba();
        glPopMatrix();
    }

    glPopMatrix();

    glFlush();
}

void idle()
{
    if (!glindaScene) return;

    int   now = glutGet(GLUT_ELAPSED_TIME);
    float dt = (now - lastTime) / 1000.0f;
    lastTime = now;

    glindaArmAngle += glindaArmDir * (0.2f * dt * 60.0f);
    if (glindaArmAngle > 25.0f) { glindaArmAngle = 25.0f; glindaArmDir = -1.0f; }
    if (glindaArmAngle < -10.0f) { glindaArmAngle = -10.0f; glindaArmDir = 1.0f; }

    wandSpin += 1.0f * dt * 60.0f;
    if (wandSpin > 360.0f) wandSpin -= 360.0f;

    glindaSpin += 0.25f * dt * 60.0f;
    if (glindaSpin > 360.0f) glindaSpin -= 360.0f;

    phaseT += dt;

    float camBob = 0.03f * sinf(glindaSpin * PI / 180.0f);

    switch (storyPhase)
    {
    case 0:
    {
        float total = phaseDur[0];
        float t = phaseT / total; if (t > 1.0f) t = 1.0f;

        if (t < 0.5f)
        {
            float s = t / 0.5f;

            float sx = 0.0f, sy = 1.5f, sz = 7.0f;
            float ex = -0.8f, ey = 0.9f, ez = 3.5f;

            eyeX = sx + (ex - sx) * s;
            eyeY = sy + (ey - sy) * s + camBob;
            eyeZ = sz + (ez - sz) * s;

            atX = glindaPosX;
            atY = 0.18f;
            atZ = 0.05f;

            upAngle = 2.0f * PI / 180.0f * s;
        }
        else
        {
            float s = (t - 0.5f) / 0.5f;

            float cx = glindaPosX;
            float cz = 0.0f;

            float radius = 1.2f;
            float angle = -PI * 0.6f + s * PI * 0.7f;

            eyeX = cx + radius * cosf(angle);
            eyeZ = cz + radius * sinf(angle);
            eyeY = 0.55f + 0.10f * sinf(s * PI) + camBob;

            atX = cx - 0.02f;
            atY = 0.22f;
            atZ = cz + 0.02f;

            upAngle = (5.0f * PI / 180.0f) * sinf(s * 2.0f * PI);
        }

        lightType = 3;

        lightPos[0] = -0.4f;
        lightPos[1] = 1.6f;
        lightPos[2] = 2.2f;
        lightPos[3] = 1.0f;

        spotDir[0] = glindaPosX - lightPos[0];
        spotDir[1] = 0.25f - lightPos[1];
        spotDir[2] = 0.05f - lightPos[2];

        spotCutoff = 18.0f - 4.0f * t;

        lightAmbient[0] = 0.06f;
        lightAmbient[1] = 0.05f;
        lightAmbient[2] = 0.08f;

        lightDiffuse[0] = 0.8f + 0.2f * t;
        lightDiffuse[1] = 0.4f + 0.4f * t;
        lightDiffuse[2] = 0.9f;

        lightSpecular[0] = 0.6f;
        lightSpecular[1] = 0.6f;
        lightSpecular[2] = 0.8f;

        float shineFactor = 0.5f + 0.5f * sinf(phaseT * 2.0f);  

        matSpecular[0] = shineFactor;
        matSpecular[1] = shineFactor;
        matSpecular[2] = shineFactor;

        matShininess = 5.0f + 115.0f * shineFactor;

        if (phaseT > total) { storyPhase = 1; phaseT = 0.0f; }
        break;

    }
    
	case 1: // 글린다와 엘파바 등장
    {
        float total = phaseDur[1];
        float moveDur = total * 0.6f;
        float t = phaseT / moveDur; if (t > 1.0f) t = 1.0f;

        glindaPosZ = 0.3f;
        if (phaseT < moveDur)
            glindaPosX = -0.3f + (-0.30f - (-0.30f)) * t;
        else
            glindaPosX = -0.30f;

        elphPosX = 0.30f;
        elphPosZ = 0.3f;

        if (phaseT < moveDur)
        {
            float sx = 1.6f, sy = 1.5f, sz = 5.2f;
            float ex = -1.2f, ey = 1.6f, ez = 4.2f;

            eyeX = sx + (ex - sx) * t;
            eyeY = sy + (ey - sy) * t + camBob;
            eyeZ = sz + (ez - sz) * t;

            atX = glindaPosX;
            atY = 0.25f;
            atZ = 0.05f;

            upAngle = 2.0f * PI / 180.0f;
        }
        else
        {
            float s = (phaseT - moveDur) / (total - moveDur); if (s > 1.0f) s = 1.0f;

            eyeX = -0.4f + 0.4f * s;
            eyeY = 1.7f + camBob;
            eyeZ = 5.5f - 0.5f * s;

            atX = 0.0f;
            atY = 0.25f;
            atZ = 0.0f;

            upAngle = (3.0f * PI / 180.0f) * sinf(s * PI);
        }

        lightType = 1;

        lightPos[0] = -0.2f;
        lightPos[1] = 1.9f;
        lightPos[2] = 3.5f;
        lightPos[3] = 1.0f;

        lightAmbient[0] = 0.09f;
        lightAmbient[1] = 0.09f;
        lightAmbient[2] = 0.09f;

        lightDiffuse[0] = 0.9f;
        lightDiffuse[1] = 0.9f;
        lightDiffuse[2] = 0.9f;

        lightSpecular[0] = 0.7f;
        lightSpecular[1] = 0.7f;
        lightSpecular[2] = 0.7f;

        matSpecular[0] = 0.4f;
        matSpecular[1] = 0.4f;
        matSpecular[2] = 0.4f;
        matShininess = 60.0f;

        if (phaseT > total) { storyPhase = 2; phaseT = 0.0f; }
        break;
    }

	case 2: // 엘파바의 피부색이 초록색에서 살색으로 
    {
        float total = phaseDur[2];
        float t = phaseT / total;
        if (t > 1.0f) t = 1.0f;

        float cx = 0.2f;     
        float cz = 0.0f;    
        float radius = 1.6f;

        float angleStart = -PI * 0.3f;
        float angleEnd = PI * 0.5f;
        float angle = angleStart + (angleEnd - angleStart) * t;

        eyeX = cx + radius * cosf(angle);
        eyeZ = cz + radius * sinf(angle);
        eyeY = 1.7f + 0.1f * sinf(t * PI) + camBob * 0.5f;  

        atX = cx;
        atY = 0.22f;
        atZ = cz;

        upAngle = (2.0f * PI / 180.0f) * sinf(t * PI);

        if (phaseT > total * 0.2f && elphSkinMix < 1.0f)
        {
            elphSkinMix += 0.4f * dt;
            if (elphSkinMix > 1.0f) elphSkinMix = 1.0f;
        }

        lightType = 3;

        lightPos[0] = 1.3f;
        lightPos[1] = 2.0f;
        lightPos[2] = 3.0f;
        lightPos[3] = 1.0f;

        spotDir[0] = 0.77f - lightPos[0];
        spotDir[1] = 0.20f - lightPos[1];
        spotDir[2] = 0.00f - lightPos[2];

        spotCutoff = 20.0f;

        lightAmbient[0] = 0.10f;
        lightAmbient[1] = 0.08f;
        lightAmbient[2] = 0.06f;

        lightDiffuse[0] = 1.0f;
        lightDiffuse[1] = 0.8f;
        lightDiffuse[2] = 0.6f;

        lightSpecular[0] = 0.9f;
        lightSpecular[1] = 0.8f;
        lightSpecular[2] = 0.7f;

        matSpecular[0] = 0.8f;
        matSpecular[1] = 0.8f;
        matSpecular[2] = 0.7f;
        matShininess = 80.0f;

        if (phaseT > total)
        {
            elphSkinMix = 1.0f;     
            storyPhase = 3;
            phaseT = 0.0f;
        }

        break;
    }

    case 3:
    {
        float total = phaseDur[3];
        float t = phaseT / total; if (t > 1.0f) t = 1.0f;

        float sx = 0.15f, sy = 1.65f, sz = 3.3f; 
        float ex = 0.40f, ey = 1.70f, ez = 2.9f;

        eyeX = sx + (ex - sx) * t;
        eyeY = sy + (ey - sy) * t + camBob * 0.5f; 
        eyeZ = sz + (ez - sz) * t;

        atX = 0.55f;
        atY = 0.22f;
        atZ = 0.0f;

        upAngle = (-3.0f * PI / 180.0f) * sinf(t * 2.0f * PI);

        elphArmAngle += elphArmDir * (1.0f * dt * 60.0f);
        if (elphArmAngle > 35.0f) { elphArmAngle = 35.0f; elphArmDir = -1.0f; }
        if (elphArmAngle < -35.0f) { elphArmAngle = -35.0f; elphArmDir = 1.0f; }

        elphSkinMix = 1.0f - t * t;          
      
        if (elphSkinMix < 0.0f) elphSkinMix = 0.0f;

        lightType = 3;

        lightPos[0] = 0.6f + 0.4f * sinf(t * 2.0f * PI);
        lightPos[1] = 1.9f;
        lightPos[2] = 2.6f;
        lightPos[3] = 1.0f;

        spotDir[0] = 0.77f - lightPos[0];
        spotDir[1] = 0.20f - lightPos[1];
        spotDir[2] = 0.00f - lightPos[2];

        spotCutoff = 14.0f + 3.0f * sinf(t * 3.0f * PI);

        lightAmbient[0] = 0.03f;
        lightAmbient[1] = 0.14f;
        lightAmbient[2] = 0.03f;

        lightDiffuse[0] = 0.2f;
        lightDiffuse[1] = 1.0f;
        lightDiffuse[2] = 0.4f;

        lightSpecular[0] = 1.0f;
        lightSpecular[1] = 1.0f;
        lightSpecular[2] = 1.0f;

        matSpecular[0] = 0.9f;
        matSpecular[1] = 1.0f;
        matSpecular[2] = 0.9f;
        matShininess = 100.0f;

        if (phaseT > total)
        {
            elphArmAngle = 0.0f;
            storyPhase = 4;
            phaseT = 0.0f;
        }
        break;
    }

    // 둘이 주전자 앞으로 이동
    case 4:
    {
        float total = phaseDur[4];
        float moveDur = total;
        float t = phaseT / moveDur; if (t > 1.0f) t = 1.0f;

        float gStartX = -0.30f, gStartZ = 0.3f;
        float gEndX = -0.30f, gEndZ = -0.6f;
        glindaPosX = gStartX + (gEndX - gStartX) * t;
        glindaPosZ = gStartZ + (gEndZ - gStartZ) * t;

        float eStartX = 0.30f, eStartZ = 0.3f;
        float eEndX = 0.30f, eEndZ = -0.6f;
        elphPosX = eStartX + (eEndX - eStartX) * t;
        elphPosZ = eStartZ + (eEndZ - eStartZ) * t;

        float sx = 0.0f, sy = 1.9f, sz = 3.4f; 
        float ex = 0.0f, sy2 = 2.0f, ez = 2.8f;

        eyeX = sx + (ex - sx) * t;             
        eyeY = sy + (sy2 - sy) * t + camBob * 0.5f;
        eyeZ = sz + (ez - sz) * t;           

        atX = 0.0f;
        atY = -0.20f;
        atZ = -0.7f;

        upAngle = (2.0f * PI / 180.0f) * sinf(t * PI);

        lightType = 1;

        lightPos[0] = 0.0f;
        lightPos[1] = 2.5f;
        lightPos[2] = 1.5f;
        lightPos[3] = 1.0f;

        lightAmbient[0] = 0.08f;
        lightAmbient[1] = 0.08f;
        lightAmbient[2] = 0.08f;

        lightDiffuse[0] = 0.9f;
        lightDiffuse[1] = 0.9f;
        lightDiffuse[2] = 0.9f;

        lightSpecular[0] = 0.6f;
        lightSpecular[1] = 0.6f;
        lightSpecular[2] = 0.6f;

        matSpecular[0] = 0.5f;
        matSpecular[1] = 0.5f;
        matSpecular[2] = 0.5f;
        matShininess = 60.0f;
        matEmission[0] = matEmission[1] = matEmission[2] = 0.0f;

        if (phaseT > total) { storyPhase = 5; phaseT = 0.0f; }
        break;
    }

    case 5: // 주전자 두개의 색깔 변화
    {
        float total = phaseDur[5];
        float t = phaseT / total; if (t > 1.0f) t = 1.0f;

        eyeX = 0.0f;
        eyeY = 0.9f + 0.08f * sinf(t * PI) + camBob * 0.2f;
        eyeZ = 1.9f;   

        atX = 0.0f;
        atY = -0.55f;
        atZ = -1.4f;

        upAngle = (2.0f * PI / 180.0f) * sinf(phaseT * 0.5f);

        float colorStart = total * 0.15f;  
        float colorEnd = total * 0.40f;  

        if (!potInHand)
        {
            flyT += 0.5f * dt;
            if (flyT > 1.0f) flyT = 1.0f;

            float spinSpeed = (phaseT < colorStart ? 3.0f : 2.0f);
            teapotSpin += spinSpeed * dt * 60.0f;
            if (teapotSpin > 360.0f) teapotSpin -= 360.0f;

            if (phaseT > colorStart && phaseT < colorEnd)
            {
                float localT = (phaseT - colorStart) / (colorEnd - colorStart);
                if (localT > 1.0f) localT = 1.0f;

                leftPotMix = localT;
                rightPotMix = localT;
            }
            else if (phaseT >= colorEnd)
            {
                leftPotMix = 1.0f;
                rightPotMix = 1.0f;
            }
        }
        else
        {
            teapotSpin += 1.0f * dt * 60.0f;
            if (teapotSpin > 360.0f) teapotSpin -= 360.0f;
        }

        float glow = leftPotMix;  

        lightType = 3;

        lightPos[0] = 0.0f;
        lightPos[1] = 1.8f;
        lightPos[2] = -0.3f;
        lightPos[3] = 1.0f;

        spotDir[0] = 0.0f - lightPos[0];
        spotDir[1] = -0.55f - lightPos[1];
        spotDir[2] = -1.4f - lightPos[2];
        spotCutoff = 20.0f;
        glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 25.0f);

        lightAmbient[0] = 0.05f + 0.15f * glow;
        lightAmbient[1] = 0.05f + 0.15f * glow;
        lightAmbient[2] = 0.08f + 0.12f * glow;

        lightDiffuse[0] = 0.7f + 0.3f * glow;
        lightDiffuse[1] = 0.7f + 0.3f * glow;
        lightDiffuse[2] = 0.9f;

        lightSpecular[0] = 0.6f + 0.4f * glow;
        lightSpecular[1] = 0.6f + 0.4f * glow;
        lightSpecular[2] = 0.8f + 0.2f * glow;

        matShininess = 40.0f + 80.0f * glow;

        if (phaseT > total)
        {
            potInHand = true;   
            storyPhase = 6;
            phaseT = 0.0f;
        }

        break;
    }

        case 6: //카메라 고정 + 조명효과
        {
            teapotSpin = 0.0f;
            float total = phaseDur[6];
            float t = phaseT / total;
            if (t > 1.0f) t = 1.0f;

            eyeX = 0.0f;
            eyeY = 0.85f;   
            eyeZ = 2.6f;     

            atX = 0.0f;
            atY = 0.55f;
            atZ = 1.4f;

            upAngle = 0.0f;

            glindaPosX = -0.55f;
            glindaPosZ = 1.10f;

            elphPosX = 0.55f;
            elphPosZ = 1.10f;

            potInHand = true;  

            float ang = phaseT * 1.2f;  
            float rad = 1.8f;             
            float centerX = 0.0f;
            float centerY = 1.0f;
            float centerZ = 1.4f;

            lightPos[0] = centerX;
            lightPos[1] = centerY + rad * cosf(ang);
            lightPos[2] = centerZ + rad * sinf(ang);
            lightPos[3] = 1.0f;

            spotDir[0] = 0.0f - lightPos[0];
            spotDir[1] = 0.55f - lightPos[1];
            spotDir[2] = 1.4f - lightPos[2];
            spotCutoff = 22.0f;

            lightType = 3;

            float cycle = fmod(phaseT, 6.0f);   
            float r, g, b;

            if (cycle < 2.0f)
            {
                r = 0.10f; g = 0.90f; b = 0.30f;
            }
            else if (cycle < 4.0f)
            {
                r = 1.00f; g = 0.50f; b = 0.90f;
            }
            else
            {
                r = 1.00f; g = 1.00f; b = 1.00f;
            }

            rainbowR = r;
            rainbowG = g;
            rainbowB = b;

            float envFactor = 0.2f + 0.2f * (0.5f + 0.5f * sinf(phaseT * 1.5f)); 
            float diffFactor = 0.7f;                                             
            float specPulse = 0.2f + 0.8f * (0.5f + 0.5f * sinf(phaseT * 2.0f)); 

            // ambient light
            lightAmbient[0] = r * envFactor;
            lightAmbient[1] = g * envFactor;
            lightAmbient[2] = b * envFactor;

            // diffuse light
            lightDiffuse[0] = r * diffFactor;
            lightDiffuse[1] = g * diffFactor;
            lightDiffuse[2] = b * diffFactor;

            // specular light
            lightSpecular[0] = r * specPulse;
            lightSpecular[1] = g * specPulse;
            lightSpecular[2] = b * specPulse;

            // material ambient
            matAmbient[0] = 0.15f;
            matAmbient[1] = 0.15f;
            matAmbient[2] = 0.18f;
            matAmbient[3] = 1.0f;

            // material diffuse
            matDiffuse[0] = 0.8f;
            matDiffuse[1] = 0.8f;
            matDiffuse[2] = 0.8f;
            matDiffuse[3] = 1.0f;

            // material specular, shininess
            float shineFactor = 0.3f + 0.7f * (0.5f + 0.5f * sinf(phaseT * 2.5f)); // 0.3~1.0
            matSpecular[0] = shineFactor;
            matSpecular[1] = shineFactor;
            matSpecular[2] = shineFactor;
            matSpecular[3] = 1.0f;

            matShininess = 20.0f + 100.0f * shineFactor;

            float emisFactor = 0.0f + 0.4f * (0.5f + 0.5f * sinf(phaseT * 3.0f)); // 0~0.4
            matEmission[0] = r * emisFactor;
            matEmission[1] = g * emisFactor;
            matEmission[2] = b * emisFactor;
            matEmission[3] = 1.0f;

            if (phaseT > total)
                phaseT = total; 

            break;
        }

    }

    glutPostRedisplay();
}
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'q':
    case 'Q':
        glindaScene = true;
        lightType = 3;

        lightAmbient[0] = 0.02f; lightAmbient[1] = 0.02f; lightAmbient[2] = 0.03f;
        lightDiffuse[0] = 0.8f;  lightDiffuse[1] = 0.8f;  lightDiffuse[2] = 0.8f;
        lightSpecular[0] = 0.3f;  lightSpecular[1] = 0.3f;  lightSpecular[2] = 0.3f;

        spotCutoff = 18.0f;
        spotDelta = 0.03f;

        storyPhase = 0;
        phaseT = 0.0f;
        elphSkinMix = 0.0f;
        leftPotMix = 0.0f;
        rightPotMix = 0.0f;
        teapotSpin = 0.0f;
        flyT = 0.0f;
        potInHand = false;

        glindaPosX = -0.30f; glindaPosZ = 0.3f;
        elphPosX = 0.30f; elphPosZ = 0.3f;
        elphArmAngle = 0.0f;
        elphArmDir = 1.0f;

        eyeX = 0.0f;  eyeY = 1.5f; eyeZ = 7.0f;
        atX = glindaPosX; atY = 0.05f; atZ = 0.0f;
        upAngle = 0.0f;

        lastTime = glutGet(GLUT_ELAPSED_TIME);
        break;

    case 27:
        exit(0);
        break;
    }

    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_SINGLE);
    glutInitWindowSize(1000, 600);
    glutCreateWindow("Wicked Scene - 이가연 ");

    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}
