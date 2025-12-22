#include <gl/freeglut.h>
#include <iostream>
#define PI 3.141592f

// 카메라 
float eyeX = 0.0f, eyeY = 0.0f, eyeZ = 2.5f;
float atX = 0.0f, atY = 0.0f, atZ = 0.0f;
float upAngle = 0.0f;
float upX = 0.0f, upY = 1.0f, upZ = 0.0f;

int projMode = 0;

// 조명 
GLfloat lightPos[4] = { 0.0f, 1.5f, 2.0f, 1.0f };
int lightType = 1;

GLfloat lightAmbient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
GLfloat lightDiffuse[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat lightSpecular[4] = { 0.3f, 0.3f, 0.3f, 1.0f };

// Material 
GLfloat matAmbient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
GLfloat matDiffuse[4] = { 0.8f, 0.5f, 0.2f, 1.0f };
GLfloat matSpecular[4] = { 0.0f, 0.0f, 0.0f, 1.0f };  
GLfloat matEmission[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat matShininess = 10.0f;                      

// Spotlight
GLfloat spotDir[3] = { 0.0f, -1.0f, -1.0f };
float spotCutoff = 20.0f;

// 애니메이션
bool isCamAnimating = false;
int  camFrame = 0;
int  frameSkip = 0;

bool isLightAnimating = false;
int  lightFrame = 0;
int  lightFrameSkip = 0;

bool isLMAnimating = false;  
int  lmFrame = 0;
int  lmFrameSkip = 0;

void drawBox(float x1, float y1, float x2, float y2, float depth) {
    float halfZ = depth * 0.5f;

    float left = (x1 < x2) ? x1 : x2;
    float right = (x1 < x2) ? x2 : x1;
    float bottom = (y1 < y2) ? y1 : y2;
    float top = (y1 < y2) ? y2 : y1;

    glBegin(GL_QUADS);
    // 앞면
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(left, bottom, halfZ);
    glVertex3f(right, bottom, halfZ);
    glVertex3f(right, top, halfZ);
    glVertex3f(left, top, halfZ);
    // 뒷면
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(left, bottom, -halfZ);
    glVertex3f(right, bottom, -halfZ);
    glVertex3f(right, top, -halfZ);
    glVertex3f(left, top, -halfZ);
    // 왼쪽
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(left, bottom, -halfZ);
    glVertex3f(left, bottom, halfZ);
    glVertex3f(left, top, halfZ);
    glVertex3f(left, top, -halfZ);
    // 오른쪽
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(right, bottom, -halfZ);
    glVertex3f(right, bottom, halfZ);
    glVertex3f(right, top, halfZ);
    glVertex3f(right, top, -halfZ);
    // 아래
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(left, bottom, -halfZ);
    glVertex3f(right, bottom, -halfZ);
    glVertex3f(right, bottom, halfZ);
    glVertex3f(left, bottom, halfZ);
    // 위
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(left, top, -halfZ);
    glVertex3f(right, top, -halfZ);
    glVertex3f(right, top, halfZ);
    glVertex3f(left, top, halfZ);
    glEnd();
}

// 이름(이가연)
void drawName_I_GA_YEON()
{
    float r = 0.5f, g = 0.8f, b = 1.0f;
    float d = 0.04f;

    // '이' - ㅇ
    glPushMatrix();
    glTranslatef(0.23f, -0.50f, 0.01f);
    glColor3f(r, g, b);
    glutSolidTorus(0.018f, 0.045f, 24, 36);
    glColor3f(1.0f, 1.0f, 1.0f);
    glutWireTorus(0.018f, 0.045f, 12, 18);
    glPopMatrix();

    // ㅣ
    glColor3f(r, g, b);
    drawBox(0.28f, -0.60f, 0.30f, -0.40f, d);

    // '가'
    drawBox(0.35f, -0.48f, 0.40f, -0.45f, d);
    drawBox(0.40f, -0.54f, 0.43f, -0.45f, d);
    drawBox(0.45f, -0.60f, 0.475f, -0.40f, d);
    drawBox(0.475f, -0.54f, 0.50f, -0.51f, d);

    // '연'
    glPushMatrix();
    glTranslatef(0.625f, -0.515f, 0.01f);
    glColor3f(r, g, b);
    glutSolidTorus(0.018f, 0.045f, 24, 36);
    glColor3f(1.0f, 1.0f, 1.0f);
    glutWireTorus(0.018f, 0.045f, 12, 18);
    glPopMatrix();

    glColor3f(r, g, b);
    drawBox(0.70f, -0.60f, 0.75f, -0.40f, d);
    drawBox(0.65f, -0.49f, 0.70f, -0.46f, d);
    drawBox(0.65f, -0.54f, 0.70f, -0.51f, d);
    drawBox(0.55f, -0.73f, 0.58f, -0.63f, d);
    drawBox(0.58f, -0.73f, 0.75f, -0.68f, d);
}

// 글린다 
void drawGlinda()
{
    float d = 0.05f;

    glColor3f(1.0f, 0.75f, 0.85f);
    drawBox(-0.80f, -0.60f, -0.60f, -0.10f, d);
    drawBox(-0.76f, -0.10f, -0.64f, 0.16f, d);

    glColor3f(1.0f, 0.90f, 0.80f);
    drawBox(-0.71f, 0.16f, -0.69f, 0.22f, d);

    glPushMatrix();
    glTranslatef(-0.70f, 0.30f, 0.0f);
    glutSolidSphere(0.085f, 32, 32);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0.97f, 0.88f, 0.45f);
    glTranslatef(-0.70f, 0.32f, -0.01f);
    glScalef(1.7f, 0.7f, 1.0f);
    glutSolidSphere(0.09f, 32, 32);
    glPopMatrix();

    glColor3f(1.0f, 0.90f, 0.80f);
    drawBox(-0.80f, -0.20f, -0.76f, 0.12f, d);
    drawBox(-0.64f, -0.20f, -0.60f, 0.12f, d);
    drawBox(-0.64f, -0.24f, -0.60f, -0.20f, d);

    glColor3f(1.0f, 0.80f, 0.95f);
    glBegin(GL_LINES);
    glVertex3f(-0.62f, -0.22f, 0.0f);
    glVertex3f(-0.50f, 0.10f, 0.0f);
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(-0.50f, 0.10f, 0.0f); glVertex3f(-0.48f, 0.12f, 0.0f);
    glVertex3f(-0.50f, 0.10f, 0.0f); glVertex3f(-0.52f, 0.12f, 0.0f);
    glVertex3f(-0.50f, 0.10f, 0.0f); glVertex3f(-0.48f, 0.08f, 0.0f);
    glVertex3f(-0.50f, 0.10f, 0.0f); glVertex3f(-0.52f, 0.08f, 0.0f);
    glEnd();
}

// 엘파바
void drawElphaba()
{
    float d = 0.05f;

    glColor3f(0.05f, 0.15f, 0.06f);
    drawBox(0.72f, -0.58f, 0.82f, 0.05f, d);

    glColor3f(0.0f, 0.60f, 0.0f);
    drawBox(0.69f, -0.30f, 0.72f, 0.05f, d);
    drawBox(0.82f, -0.30f, 0.85f, 0.05f, d);
    drawBox(0.74f, 0.05f, 0.80f, 0.11f, d);

    glPushMatrix();
    glColor3f(0.0f, 0.60f, 0.0f);
    glTranslatef(0.77f, 0.17f, 0.0f);
    glutSolidSphere(0.09f, 32, 32);
    glPopMatrix();

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
}

// 주전자
void drawTeapots()
{
    glDisable(GL_COLOR_MATERIAL);

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, matEmission);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, matShininess);

    glColor3f(1.0f, 1.0f, 1.0f);

    glPushMatrix();
    glTranslatef(-0.22f, -0.65f, 0.0f);
    glutSolidTeapot(0.18f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.15f, -0.65f, 0.0f);
    glutSolidTeapot(0.18f);
    glPopMatrix();

    glEnable(GL_COLOR_MATERIAL);
}

void setProjection() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (projMode == 0) {
        glOrtho(-2.0, 2.0, -1.5, 1.5, 1.0, 20.0);
    }
    else {
        glFrustum(-1.0, 1.0, -0.75, 0.75, 1.0, 20.0);
    }

    glMatrixMode(GL_MODELVIEW);
}

void init() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    // Light
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glShadeModel(GL_SMOOTH);

    projMode = 0;
    setProjection();

    eyeX = 0.0f; eyeY = 0.8f; eyeZ = 4.0f;
    atX = 0.0f; atY = -0.10f; atZ = 0.0f;
    upAngle = 0.0f;
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (isCamAnimating) {
        frameSkip++;
        if (frameSkip >= 8) {
            frameSkip = 0;
            camFrame++;

            if (camFrame > 340) {
                camFrame = 340;
                isCamAnimating = false;
            }

            int f = camFrame;

            if (f < 80) {
                float s = f / 80.0f;

                float startEyeX = 0.0f, startEyeY = 1.3f, startEyeZ = 7.0f;
                float endEyeX = -1.4f, endEyeY = 0.4f, endEyeZ = 3.0f;

                eyeX = startEyeX + (endEyeX - startEyeX) * s;
                eyeY = startEyeY + (endEyeY - startEyeY) * s;
                eyeZ = startEyeZ + (endEyeZ - startEyeZ) * s;

                float cx = -0.70f;
                float cy = 0.05f;
                float cz = 0.0f;

                atX = cx - 0.05f;
                atY = cy + 0.20f;
                atZ = cz + 0.05f;

                upAngle = 0.0f;
            }
            else if (f < 160) {
                float s = (f - 80) / 80.0f;

                float cx = -0.70f;
                float cy = 0.05f;
                float cz = 0.0f;

                float radius = 1.2f;
                float angle = (-PI / 2.0f) + s * PI;

                eyeX = cx + radius * cosf(angle);
                eyeZ = cz + radius * sinf(angle);
                eyeY = 0.4f;

                atX = cx - 0.05f;
                atY = cy + 0.20f;
                atZ = cz + 0.05f;

                upAngle = (5.0f * PI / 180.0f) * sinf(angle * 2.0f);
            }
            else if (f < 200) {
                float s = (f - 160) / 40.0f;

                float startEyeX = eyeX;
                float startEyeY = eyeY;
                float startEyeZ = eyeZ;

                float endEyeX = 1.0f;
                float endEyeY = 2.0f;
                float endEyeZ = 7.0f;

                eyeX = startEyeX + (endEyeX - startEyeX) * s;
                eyeY = startEyeY + (endEyeY - startEyeY) * s;
                eyeZ = startEyeZ + (endEyeZ - startEyeZ) * s;

                float startAtX = -0.70f, startAtY = 0.20f, startAtZ = 0.0f;
                float endAtX = 0.77f, endAtY = 0.15f, endAtZ = 0.0f;

                atX = startAtX + (endAtX - startAtX) * s;
                atY = startAtY + (endAtY - startAtY) * s;
                atZ = startAtZ + (endAtZ - startAtZ) * s;

                upAngle = 0.0f;
            }
            else if (f < 280) {
                float s = (f - 200) / 80.0f;

                float cx = 0.77f;
                float cy = -0.05f;
                float cz = 0.0f;

                float radius = 3.5f;
                float angle = (7.0f * PI / 6.0f) + s * (2.0f * PI);

                eyeX = cx + radius * cosf(angle);
                eyeZ = cz + radius * sinf(angle);
                eyeY = 2.0f;

                atX = cx + 0.05f;
                atY = cy + 0.10f;
                atZ = cz;

                upAngle = (-15.0f * PI / 180.0f) * sinf(angle);
            }
            else {
                float s = (f - 280) / 60.0f;
                if (s > 1.0f) s = 1.0f;

                float startEyeX = eyeX;
                float startEyeY = eyeY;
                float startEyeZ = eyeZ;

                float endEyeX = 0.0f;
                float endEyeY = 0.8f;
                float endEyeZ = 6.5f;

                eyeX = startEyeX + (endEyeX - startEyeX) * s;
                eyeY = startEyeY + (endEyeY - startEyeY) * s;
                eyeZ = startEyeZ + (endEyeZ - startEyeZ) * s;

                float startAtX = atX;
                float startAtY = atY;
                float startAtZ = atZ;

                float endAtX = 0.0f;
                float endAtY = -0.15f;
                float endAtZ = 0.0f;

                atX = startAtX + (endAtX - startAtX) * s;
                atY = startAtY + (endAtY - startAtY) * s;
                atZ = startAtZ + (endAtZ - startAtZ) * s;

                upAngle *= (1.0f - s);
            }
        }
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    upX = sinf(upAngle);
    upY = cosf(upAngle);
    upZ = 0.0f;

    gluLookAt(eyeX, eyeY, eyeZ,
        atX, atY, atZ,
        upX, upY, upZ);

    if (isLightAnimating) {
        lightFrameSkip++;
        if (lightFrameSkip >= 5) {
            lightFrameSkip = 0;
            lightFrame++;

            if (lightFrame > 260) {
                lightFrame = 260;
                isLightAnimating = false;
            }

            int f = lightFrame;

            if (f < 90) {
                float s = f / 90.0f;
                lightType = 1;

                float startX = -0.6f;
                float endX = 0.7f;
                float y = 0.9f;
                float z = 2.0f;

                lightPos[0] = startX + (endX - startX) * s;
                lightPos[1] = y;
                lightPos[2] = z;
            }
            else if (f < 180) {
                float s = (f - 90) / 90.0f;
                lightType = 1;

                float startX = 0.05f, startY = 0.9f, startZ = 2.0f;
                float endX = 0.0f, endY = 0.2f, endZ = 2.5f;

                lightPos[0] = startX + (endX - startX) * s;
                lightPos[1] = startY + (endY - startY) * s;
                lightPos[2] = startZ + (endZ - startZ) * s;
            }
            else {
                float s = (f - 180) / 80.0f;
                if (s > 1.0f) s = 1.0f;

                lightType = 3;

                float targetX = 0.77f;
                float targetY = 0.17f;
                float targetZ = 0.0f;

                float radius = 2.2f;
                float baseHeight = 1.5f;

                float angle = (-PI / 2.0f) + s * (2.5f * PI);

                lightPos[0] = targetX + radius * cosf(angle);
                lightPos[2] = targetZ + radius * sinf(angle);
                lightPos[1] = baseHeight;

                spotDir[0] = targetX - lightPos[0];
                spotDir[1] = targetY - lightPos[1];
                spotDir[2] = targetZ - lightPos[2];

                spotCutoff = 18.0f;
            }
        }
    }

    if (isLMAnimating) {
        lmFrameSkip++;
        if (lmFrameSkip >= 12) {          
            lmFrameSkip = 0;
            lmFrame++;

            if (lmFrame > 420) {
                lmFrame = 420;
                isLMAnimating = false;

                lightType = 1;
                lightPos[0] = 0.0f; lightPos[1] = 1.5f; lightPos[2] = 2.0f;
                lightAmbient[0] = lightAmbient[1] = lightAmbient[2] = 0.1f;
                lightDiffuse[0] = lightDiffuse[1] = lightDiffuse[2] = 0.8f;
                lightSpecular[0] = lightSpecular[1] = lightSpecular[2] = 0.3f;

                matAmbient[0] = matAmbient[1] = matAmbient[2] = 0.1f;
                matDiffuse[0] = 0.8f; matDiffuse[1] = 0.5f; matDiffuse[2] = 0.2f;
                matSpecular[0] = matSpecular[1] = matSpecular[2] = 0.0f;
                matShininess = 10.0f;
                matEmission[0] = matEmission[1] = matEmission[2] = 0.0f;
                return;
            }

            int f = lmFrame;

            if (f < 140) {
                float s = f / 140.0f;   
                lightType = 1;       
                lightPos[0] = -1.2f + 2.0f * s;
                lightPos[1] = 1.6f - 0.6f * s;
                lightPos[2] = 1.8f;

                lightDiffuse[0] = 1.0f;
                lightDiffuse[1] = s;      
                lightDiffuse[2] = 1.0f;

                lightAmbient[0] = 0.02f + 0.03f * s;
                lightAmbient[1] = 0.01f + 0.03f * s;
                lightAmbient[2] = 0.02f + 0.03f * s;

                matShininess = 40.0f;
                matSpecular[0] = matSpecular[1] = matSpecular[2] = 0.3f + 0.3f * s;
                matEmission[0] = matEmission[1] = matEmission[2] = 0.0f;
            }
            else if (f < 280) {
                float s = (f - 140) / 140.0f;  

                lightType = 1;

                lightPos[0] = -0.7f + 1.4f * s;
                lightPos[1] = 0.4f;
                lightPos[2] = 1.0f;

                // Diffuse
                lightDiffuse[0] = 1.0f * (1.0f - s) + 0.4f * s;
                lightDiffuse[1] = 1.0f;
                lightDiffuse[2] = 1.0f * (1.0f - s) + 0.6f * s;

                // Ambient
                lightAmbient[0] = 0.03f;
                lightAmbient[1] = 0.05f + 0.03f * s;
                lightAmbient[2] = 0.04f;

                lightSpecular[0] = lightSpecular[1] = lightSpecular[2] = 1.0f;

                matShininess = 70.0f + 30.0f * s;             
                matSpecular[0] = matSpecular[1] = matSpecular[2] = 0.6f + 0.4f * s;

                // Emission
                float e = 0.6f * s;
                matEmission[0] = e;          // 노랑
                matEmission[1] = 0.8f * e;
                matEmission[2] = 0.4f * e;
            }
            else {
                float s = (f - 280) / 140.0f;     

                lightType = 3; 

                float targetX = 0.77f;
                float targetY = 0.17f;
                float targetZ = 0.0f;

                float radius = 1.2f;
                float baseHeight = 0.8f;
                float angle = (-PI / 2.0f) + s * (1.5f * PI);

                lightPos[0] = targetX + radius * cosf(angle);
                lightPos[2] = targetZ + radius * sinf(angle);
                lightPos[1] = baseHeight;

                spotDir[0] = targetX - lightPos[0];
                spotDir[1] = targetY - lightPos[1];
                spotDir[2] = targetZ - lightPos[2];

                lightDiffuse[0] = 0.0f;
                lightDiffuse[1] = 1.0f;
                lightDiffuse[2] = 0.0f;  

                lightAmbient[0] = 0.0f;
                lightAmbient[1] = 0.15f;
                lightAmbient[2] = 0.0f;

                lightSpecular[0] = lightSpecular[1] = lightSpecular[2] = 1.0f;

                spotCutoff = 16.0f + 4.0f * sinf(s * 2.0f * PI);

                float e = 0.7f + 0.3f * sinf(s * 6.0f * PI);   
                if (e < 0.4f) e = 0.4f;

                matEmission[0] = e;
                matEmission[1] = 0.7f * e;
                matEmission[2] = 0.5f * e;

                matShininess = 100.0f;
                matSpecular[0] = matSpecular[1] = matSpecular[2] = 1.0f;
            }
        }
    }


    GLfloat pos[4];
    if (lightType == 1) {
        pos[0] = lightPos[0];
        pos[1] = lightPos[1];
        pos[2] = lightPos[2];
        pos[3] = 1.0f;
        glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.0f);
    }
    else if (lightType == 2) {
        pos[0] = lightPos[0];
        pos[1] = lightPos[1];
        pos[2] = lightPos[2];
        pos[3] = 0.0f;
        glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.0f);
    }
    else {
        pos[0] = lightPos[0];
        pos[1] = lightPos[1];
        pos[2] = lightPos[2];
        pos[3] = 1.0f;
        glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDir);
        glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, spotCutoff);
        glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 20.0f);
    }

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    glLightfv(GL_LIGHT0, GL_POSITION, pos);

    glPushMatrix();
    glDisable(GL_LIGHTING);
    glTranslatef(lightPos[0], lightPos[1], lightPos[2]);
    if (lightType == 1)       glColor3f(1.0f, 1.0f, 0.0f);
    else if (lightType == 2)  glColor3f(0.6f, 0.8f, 1.0f);
    else                      glColor3f(1.0f, 0.4f, 1.0f);
    glutSolidCube(0.2f);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    glPushMatrix();
    glScalef(1.0f, 1.0f, 1.0f);

    glPushMatrix();
    glTranslatef(-0.55f, 0.75f, -1.5f);
    drawName_I_GA_YEON();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.1f);
    drawGlinda();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -0.3f);
    drawElphaba();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.8f);
    drawTeapots();
    glPopMatrix();

    glPopMatrix();
    glFlush();
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        // 카메라
    case 'a': eyeX -= 0.05f; break;
    case 'A': eyeX += 0.05f; break;
    case 'b': atX -= 0.05f; break;
    case 'B': atX += 0.05f; break;
    case 'c': upAngle += 5.0f * PI / 180.0f; break;
    case 'C': upAngle -= 5.0f * PI / 180.0f; break;

        // 조명 위치
    case 'f': lightPos[0] -= 0.2f; break;
    case 'F': lightPos[0] += 0.2f; break;

    case '1':
        lightType = 1;
        lightPos[0] = -0.8f; lightPos[1] = 0.6f; lightPos[2] = 2.0f;
        break;
    case '2':
        lightType = 2;
        lightPos[0] = 0.0f; lightPos[1] = 1.8f; lightPos[2] = 2.5f;
        break;
    case '3':
        lightType = 3;
        lightPos[0] = 0.8f; lightPos[1] = 0.6f; lightPos[2] = 2.0f;
        break;

    case 'e': case 'E':
        if (projMode == 0) {
            projMode = 1;
            setProjection();
            eyeX = 0.0f; eyeY = 0.4f; eyeZ = 3.0f;
            atX = 0.0f; atY = -0.4f; atZ = 0.8f;
            upAngle = 0.0f;
        }
        else {
            projMode = 0;
            setProjection();
            eyeX = 0.0f; eyeY = 0.8f; eyeZ = 4.0f;
            atX = 0.0f; atY = -0.10f; atZ = 0.0f;
            upAngle = 0.0f;
        }
        break;

        // 카메라 애니메이션
    case 'd': case 'D':
        isCamAnimating = true;
        camFrame = 0;
        frameSkip = 0;
        projMode = 1;
        setProjection();
        eyeX = 0.0f;  eyeY = 1.3f; eyeZ = 7.0f;
        atX = -0.70f; atY = 0.05f; atZ = 0.0f;
        upAngle = 0.0f;
        break;

        // 조명 애니메이션
    case 'g': case 'G':
        isLightAnimating = true;
        lightFrame = 0;
        lightFrameSkip = 0;
        lightType = 1;
        lightPos[0] = -0.6f;
        lightPos[1] = 0.9f;
        lightPos[2] = 2.0f;
        break;

        //Light 속성 조절
    case 'h':   // Ambient +
        for (int i = 0; i < 3; i++) {
            lightAmbient[i] += 0.2f;
            if (lightAmbient[i] > 1.0f) lightAmbient[i] = 1.0f;
        }
        break;
    case 'H':   // Ambient -
        for (int i = 0; i < 3; i++) {
            lightAmbient[i] -= 0.2f;
            if (lightAmbient[i] < 0.0f) lightAmbient[i] = 0.0f;
        }
        break;

    case 'i':   // Diffuse +
        for (int i2 = 0; i2 < 3; i2++) {
            lightDiffuse[i2] += 0.2f;
            if (lightDiffuse[i2] > 1.0f) lightDiffuse[i2] = 1.0f;
        }
        break;
    case 'I':   // Diffuse -
        for (int i2 = 0; i2 < 3; i2++) {
            lightDiffuse[i2] -= 0.2f;
            if (lightDiffuse[i2] < 0.0f) lightDiffuse[i2] = 0.0f;
        }
        break;

    case 'j':   // Specular +
        projMode = 1;
        setProjection();
        eyeX = 0.8f;
        eyeY = -0.2f;
        eyeZ = 3.0f;
        atX = 0.15f;
        atY = -0.65f;
        atZ = 0.8f;
        upAngle = 0.0f;

        if (matShininess < 50.0f) matShininess = 80.0f;
        if (matSpecular[0] < 0.5f && matSpecular[1] < 0.5f && matSpecular[2] < 0.5f) {
            matSpecular[0] = matSpecular[1] = matSpecular[2] = 0.7f;
        }
        for (int j2 = 0; j2 < 3; j2++) {
            lightSpecular[j2] += 0.3f;
            if (lightSpecular[j2] > 1.0f) lightSpecular[j2] = 1.0f;
        }
        break;

    case 'J':   // Specular -
        projMode = 1;
        setProjection();
        eyeX = 0.8f;
        eyeY = -0.2f;
        eyeZ = 3.0f;
        atX = 0.15f;
        atY = -0.65f;
        atZ = 0.8f;
        upAngle = 0.0f;

        for (int j2 = 0; j2 < 3; j2++) {
            lightSpecular[j2] -= 0.3f;
            if (lightSpecular[j2] < 0.0f) lightSpecular[j2] = 0.0f;
        }
        break;

        // Material 속성 조절
        // Ambient
    case 'k':
        for (int c = 0; c < 3; c++) {
            matAmbient[c] += 0.2f;
            if (matAmbient[c] > 1.0f) matAmbient[c] = 1.0f;
        }
        break;
    case 'K':
        for (int c = 0; c < 3; c++) {
            matAmbient[c] -= 0.2f;
            if (matAmbient[c] < 0.0f) matAmbient[c] = 0.0f;
        }
        break;

        // Diffuse
    case 'l':
        for (int c2 = 0; c2 < 3; c2++) {
            matDiffuse[c2] += 0.2f;
            if (matDiffuse[c2] > 1.0f) matDiffuse[c2] = 1.0f;
        }
        break;
    case 'L':
        for (int c2 = 0; c2 < 3; c2++) {
            matDiffuse[c2] -= 0.2f;
            if (matDiffuse[c2] < 0.0f) matDiffuse[c2] = 0.0f;
        }
        break;

        // Specular
    case 'm':
        for (int s = 0; s < 3; s++) {
            matSpecular[s] += 0.3f;
            if (matSpecular[s] > 1.0f) matSpecular[s] = 1.0f;
        }
        break;
    case 'M':
        for (int s = 0; s < 3; s++) {
            matSpecular[s] -= 0.3f;
            if (matSpecular[s] < 0.0f) matSpecular[s] = 0.0f;
        }
        break;

        // Shininess 
    case 'n':   // 유광
        matShininess = 100.0f;
        matSpecular[0] = matSpecular[1] = matSpecular[2] = 1.0f;
        break;
    case 'N':   // 무광
        matShininess = 5.0f;
        matSpecular[0] = matSpecular[1] = matSpecular[2] = 0.1f;
        break;

        // Emission
    case 'o':  
        matEmission[0] = 1.0f;
        matEmission[1] = 1.0f;
        matEmission[2] = 0.0f;
        break;
    case 'O': 
        matEmission[0] = matEmission[1] = matEmission[2] = 0.0f;
        break;

    case 'p': case 'P':
        isLMAnimating = true;
        lmFrame = 0;
        lmFrameSkip = 0;

        isLightAnimating = false;
        lightType = 1;
        lightPos[0] = 0.0f; lightPos[1] = 1.5f; lightPos[2] = 2.0f;
        lightAmbient[0] = lightAmbient[1] = lightAmbient[2] = 0.1f;
        lightDiffuse[0] = lightDiffuse[1] = lightDiffuse[2] = 0.8f;
        lightSpecular[0] = lightSpecular[1] = lightSpecular[2] = 0.3f;

        matAmbient[0] = matAmbient[1] = matAmbient[2] = 0.1f;
        matDiffuse[0] = 0.8f; matDiffuse[1] = 0.5f; matDiffuse[2] = 0.2f;
        matSpecular[0] = matSpecular[1] = matSpecular[2] = 0.5f;
        matShininess = 40.0f;
        matEmission[0] = matEmission[1] = matEmission[2] = 0.0f;
        break;

    default:
        break;
    }

    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_SINGLE);
    glutInitWindowSize(1000, 600);
    glutCreateWindow("Wicked Scene - 이가연");

    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}
