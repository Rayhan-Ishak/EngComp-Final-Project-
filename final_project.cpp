//
//  final_project_yshanker.cpp
//  Project_Final
//
//  Created by Yashraj Shanker on 11/14/23.
//

//#include "final_project_yshanker.hpp"


#define GL_SILENCE_DEPRECATION
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <math.h>
#include "fssimplewindow.h"
#include "yspng.h"
#include <vector>
#include <iostream>
#include "yssimplesound.h"
#include "ysglfontdata.h"

const int windowWidth = 800;
const int windowHeight = 600;

const double PI = 3.1415927;



class BatAbility {
public:
    bool isActive;
    double alpha;

    BatAbility() : isActive(false), alpha(1.0) {}
    
    void abilitybox(int x, int y, int w, int h, double alpha)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0,0,0,alpha);
        glBegin(GL_QUADS);
        glVertex2i(x,y);
        glVertex2i(x+w,y);
        glVertex2i(x+w,y+h);
        glVertex2i(x,y+h);
        glEnd();
    }

    void Activate() {
        isActive = true;
    }

    void Update() {
        if (isActive) {
            abilitybox(515, 510, 70, 60, alpha);
            alpha -= 0.02;
            if (alpha <= 0) {
                isActive = false;
                alpha = 1;
            }
        }
    }
};

class BaguetteAbility {
public:
    bool isActive;
    double alpha;

    BaguetteAbility() : isActive(false), alpha(1.0) {}
    
    void abilitybox(int x, int y, int w, int h, double alpha)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0,0,0,alpha);
        glBegin(GL_QUADS);
        glVertex2i(x,y);
        glVertex2i(x+w,y);
        glVertex2i(x+w,y+h);
        glVertex2i(x,y+h);
        glEnd();
    }

    void Activate() {
        isActive = true;
    }

    void Update() {
        if (isActive) {
            abilitybox(590, 510, 70, 60, alpha);
            alpha -= 0.05;
            if (alpha <= 0) {
                isActive = false;
                alpha = 1;
            }
        }
    }
};

class EnergyBarAbility {
public:
    int energybar_increment;
    int ebar_wid;
    int state;

    EnergyBarAbility() : energybar_increment(0), ebar_wid(0), state(0) {}

    void energybar(int x, int y, int w, int h) // int enemyhit)
    {
        glColor3f(0,0,0);
        glBegin(GL_QUADS);
        glColor3ub(173,216,230);
        glVertex2i(x,y);
        glVertex2i(x+w,y);
        glVertex2i(x+w-13,y+h);
        glVertex2i(x-13,y+h);
        glEnd();
    }
    
    void Activate() {
        state = 1;
        ebar_wid += 20;
        ++energybar_increment;
    }

    void Update() {
        if (state == 1) {
            energybar(35, 60, ebar_wid, 20);
        }
    }
};


class ultimateAbility {
public:
    double alpha;
    bool readyState;
    bool activeState;
    int energyIncrement;
    
    ultimateAbility() : alpha(0.5), readyState(false), activeState(false), energyIncrement(0) {}

    void NotReady(int x, int y, int w, int h, double a) {
        // Draw the ultimate ability when not ready
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBegin(GL_QUADS);
        glColor4f(0, 0, 0, a);
        glVertex2i(x, y);
        glVertex2i(x + w, y);
        glVertex2i(x + w, y + h);
        glVertex2i(x, y + h);
        glEnd();
    }

    void IncrementEnergy() {
        energyIncrement++;
    }

    void UseUltimate() {
        if (energyIncrement >= 6) {
            readyState = true;
            alpha = 0;
            energyIncrement = 0; // Reset energy after using the ultimate ability
        }
    }

    void TriggerUltimate() {
        if (readyState) {
            activeState = true;
            alpha = 0.5;
            readyState = false; // Reset the ready state after using the ultimate
        }
    }

    void DrawUltimateNotReady() {
        NotReady(675, 475, 100, 100, alpha);
    }
};

class HealthBar {
public:
    int takedamage;

    HealthBar() : takedamage(0) {}
    
    void abilitybox(int x, int y, int w, int h, double alpha)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0,0,0,alpha);
        glBegin(GL_QUADS);
        glVertex2i(x,y);
        glVertex2i(x+w,y);
        glVertex2i(x+w,y+h);
        glVertex2i(x,y+h);
        glEnd();
    }

    void TakeDamage(int damage) {
        if (takedamage + damage <= 3) {
            takedamage += damage;
        }
    }

    void Draw() {
        if (takedamage >= 1) {
            abilitybox(80, 10, 29, 29, 0.9);
        }
        if (takedamage >= 2) {
            abilitybox(45, 10, 29, 29, 0.9);
        }
        if (takedamage >= 3) {
            abilitybox(11, 10, 29, 29, 0.9);
        }
    }
};


class MainData {
public:
    bool firstRenderingPass;
    YsRawPngDecoder png[5];
    GLuint texId[5];
    double angle;
    BatAbility bat; // Include bat ability in MainData
    BaguetteAbility baguette;
    EnergyBarAbility EnergyBar;
    ultimateAbility ultAbility; // Instantiate the UltimateAbility class
    HealthBar healthBar;

};

void Render(void *incoming)
{
    MainData *datPtr=(MainData *)incoming;
    if(true==datPtr->firstRenderingPass)  // Do it only once.
    {
        datPtr->firstRenderingPass=false; // And, don't do it again.
        
        glGenTextures(1,&datPtr->texId[0]);  // Reserve one texture identifier
        glBindTexture(GL_TEXTURE_2D,datPtr->texId[0]);  // Making the texture identifier current (or bring it to the deck)
        
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,datPtr->png[0].wid,datPtr->png[0].hei,0,GL_RGBA,GL_UNSIGNED_BYTE,datPtr->png[0].rgba);
        
        glGenTextures(1,&datPtr->texId[1]);  // Reserve one texture identifier
        glBindTexture(GL_TEXTURE_2D,datPtr->texId[1]);  // Making the texture identifier current (or bring it to the deck)
        
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,datPtr->png[1].wid,datPtr->png[1].hei,0,GL_RGBA,GL_UNSIGNED_BYTE,datPtr->png[1].rgba);
        
        glGenTextures(1,&datPtr->texId[2]);  // Reserve one texture identifier
        glBindTexture(GL_TEXTURE_2D,datPtr->texId[2]);  // Making the texture identifier current (or bring it to the deck)
        
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,datPtr->png[2].wid,datPtr->png[2].hei,0,GL_RGBA,GL_UNSIGNED_BYTE,datPtr->png[2].rgba);
        
        glGenTextures(1,&datPtr->texId[3]);  // Reserve one texture identifier
        glBindTexture(GL_TEXTURE_2D,datPtr->texId[3]);  // Making the texture identifier current (or bring it to the deck)
        
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,datPtr->png[3].wid,datPtr->png[3].hei,0,GL_RGBA,GL_UNSIGNED_BYTE,datPtr->png[3].rgba);
        
        glGenTextures(1,&datPtr->texId[4]);  // Reserve one texture identifier
        glBindTexture(GL_TEXTURE_2D,datPtr->texId[4]);  // Making the texture identifier current (or bring it to the deck)
        
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,datPtr->png[4].wid,datPtr->png[4].hei,0,GL_RGBA,GL_UNSIGNED_BYTE,datPtr->png[4].rgba);
    }
    glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    
    //
    // OPENGL
    //
    
    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
    glVertex2i(0,0);
    glVertex2i(800,0);
    glVertex2i(800,600);
    glVertex2i(0,600);
    glEnd();
    
    //
    // TEXTURES
    //
    
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glColor4d(1.0,1.0,1.0,1.0);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);  // Begin using texture mapping
    
    // Hearts
    glBindTexture(GL_TEXTURE_2D,datPtr->texId[0]);
    glBegin(GL_QUADS);
    glTexCoord2d(0.0,0.0);
    glVertex2i(10,-25);
    glTexCoord2d(1.0,0.0);
    glVertex2i(110,-25);
    glTexCoord2d(1.0,1.0);
    glVertex2i(110,75);
    glTexCoord2d(0.0,1.0);
    glVertex2i(10,75);
    glEnd();
    
    // Lightning Bolt
    glBindTexture(GL_TEXTURE_2D,datPtr->texId[1]);
    glBegin(GL_QUADS);
    glTexCoord2d(0.0,0.0);
    glVertex2d(0,50);
    glTexCoord2d(1.0,0.0);
    glVertex2d(40,50);
    glTexCoord2d(1.0,1.0);
    glVertex2d(40,90);
    glTexCoord2d(0.0,1.0);
    glVertex2d(0,90);
    glEnd();
    
    // Bat
    glBindTexture(GL_TEXTURE_2D,datPtr->texId[2]);
    glBegin(GL_QUADS);
    glTexCoord2d(0.0,0.0);
    glVertex2d(525,515);
    glTexCoord2d(1.0,0.0);
    glVertex2d(575,515);
    glTexCoord2d(1.0,1.0);
    glVertex2d(575,565);
    glTexCoord2d(0.0,1.0);
    glVertex2d(525,565);
    glEnd();
    
    // Baguette
    glBindTexture(GL_TEXTURE_2D,datPtr->texId[3]);
    glBegin(GL_QUADS);
    glTexCoord2d(0.0,0.0);
    glVertex2d(600,515);
    glTexCoord2d(1.0,0.0);
    glVertex2d(650,515);
    glTexCoord2d(1.0,1.0);
    glVertex2d(650,565);
    glTexCoord2d(0.0,1.0);
    glVertex2d(600,565);
    glEnd();
    
    // Utimate (B)
    glBindTexture(GL_TEXTURE_2D,datPtr->texId[4]);
    glBegin(GL_QUADS);
    glTexCoord2d(0.0,0.0);
    glVertex2d(675,475);
    glTexCoord2d(1.0,0.0);
    glVertex2d(775,475);
    glTexCoord2d(1.0,1.0);
    glVertex2d(775,575);
    glTexCoord2d(0.0,1.0);
    glVertex2d(675,575);
    glEnd();
    
    glDisable(GL_TEXTURE);
    glDisable(GL_TEXTURE_2D);
    
    glFlush();
}

class PlayerActions {
public:
    void ProcessInput(int key, MainData &dat,bool Damage_state) {
        
        if (Damage_state == true) {
            dat.healthBar.TakeDamage(1);
        }
        dat.healthBar.Draw();

        if (FSKEY_SPACE == key) {
            dat.bat.Activate();
        }
        dat.bat.Update();

        if (FSKEY_K == key) {
            dat.baguette.Activate();
        }
        dat.baguette.Update();

        if (FSKEY_L == key && dat.EnergyBar.energybar_increment <= 5) {
            dat.EnergyBar.Activate();
        }
        dat.EnergyBar.Update();

        if (dat.EnergyBar.energybar_increment <= 5) {
            dat.ultAbility.DrawUltimateNotReady();
        }

        if (FSKEY_H == key && dat.EnergyBar.energybar_increment == 6) {
            dat.ultAbility.UseUltimate();
            dat.ultAbility.TriggerUltimate();

            dat.EnergyBar.energybar_increment = 0;
            dat.EnergyBar.ebar_wid = 0;
            dat.ultAbility.readyState = false;
        }
    }
};


//
//int main(void) {
//        
//    PlayerActions playerActions;
//
//    FsChangeToProgramDir();
//
//    MainData dat;
//    dat.firstRenderingPass=true; // Make texture during the first rendering pass
//    dat.png[0].Decode("hearts.png");
//    dat.png[1].Decode("flash.png");
//    dat.png[2].Decode("swing.png");
//    dat.png[3].Decode("baguette.png");
//    dat.png[4].Decode("B.png");
//    
//    
//    FsOpenWindow(16,16,800,600,1);
//    
//    FsRegisterOnPaintCallBack(Render,&dat);
////    FsRegisterOnPaintCallBack(Render, &dat);
//
//    int key = FSKEY_NULL;
//    while (FSKEY_ESC != key) {
//        FsPollDevice();
//        key = FsInkey();
//
//        FsPushOnPaintEvent();
//        
//        playerActions.ProcessInput(key, dat); // Call the ProcessInput method to handle player actions
//
//        // Ultimate
//        if (dat.EnergyBar.energybar_increment <= 5) {
//            dat.ultAbility.DrawUltimateNotReady();
//        }
//
//        FsSwapBuffers();
//        FsSleep(25);
//    }
//
//    return 0;
//}




class Menu {
private:

public:
    
    YsSoundPlayer player;
    YsSoundPlayer::SoundData BackgroundSound, SelectionSound;
    YsRawPngDecoder png[2];
    //YsRawPngDecoder png1;
    
    int start = 0;
    int quit = 0;
    int about = 0;
    int img;
    Menu() {
        img=0;
        // Initialize game resources here
        BackgroundSound.LoadWav("background.wav");
        SelectionSound.LoadWav("selection.wav");
        player.Start();

        const char* filenames[2] = { "91657 (1).png", "aboout (1).png" };
        
        for (int i=0; i<2; i++) {
            if (YSOK != png[i].Decode(filenames[i])) {
                printf("PNG load error.");
                exit(1);
            }
            png[i].Flip();
        }
        
        
//        if (YSOK != png1.Decode("aboout.png")) {
//            printf("PNG load error.");
//            exit(1);
//        }
//        png1.Flip();
        //FsOpenWindow(50, 50, 800, 600, 1);
    }

    ~Menu() {
        // Clean up resources here
        player.End();
    }

    void PlayFunction() {
        player.Stop(BackgroundSound);
        start = 1;
        // Play function code here
    }

    void QuitFunction() {
        quit=1;
        
        
        // Quit function code here
        //FsCloseWindow();
    }

    void AboutFunction() {
        about=1;
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        img=1;
        //Run();
        //FsSwapBuffers();
        // About function code here
    }

    void Run() {
        
        while (true) {
            int lb, mb, rb, mx, my;
            FsGetMouseEvent(lb, mb, rb, mx, my);
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            

            glRasterPos2i(0, 599);
            glDrawPixels(png[img].wid, png[img].hei, GL_RGBA, GL_UNSIGNED_BYTE, png[img].rgba);

            YsGlUseFontBitmap20x32(0);
            glColor3ub(255, 105, 180);

            if (img==0) {
                
                glRasterPos2i(100, 190);
                YsGlDrawFontBitmap32x48("Bragon Dall T Duper");
                
                
                
                CheckButton(350, 500, 250, 300, "Play", lb, &Menu::PlayFunction);
                CheckButton(350, 500, 300, 350, "Quit", lb, &Menu::QuitFunction);
                CheckButton(350, 500, 350, 400, "About", lb, &Menu::AboutFunction);
            }

            if (start==1) {
                break;
            }
            if (quit==1) {
                break;
            }
            
            player.PlayOneShot(BackgroundSound);
            FsSwapBuffers();
            FsPollDevice();
            //FsSleep(10);
        }
    }

    void CheckButton(int xMin, int xMax, int yMin, int yMax, const char* label, int mouseButton, void (Menu::*func)()) {
        int mx, my, lb, mb, rb;
        FsGetMouseState(lb, mb, rb, mx, my);
        if (mx > xMin && mx < xMax && my > yMin && my < yMax) {
            glColor3ub(255, 0, 0);
            if (mouseButton) {
                (this->*func)();
                player.PlayOneShot(SelectionSound);
            }
        }
        else {
            glColor3ub(255, 105, 180);
        }
        glRasterPos2i(xMin, yMax);
        YsGlDrawFontBitmap20x32(label);
    }
};

//int main(void) {
//    GameApp game;
//    game.Run();
//
//    return 0;
//}



class PlayerCharacter {
public:
    int x,dlx=0,drx=0;
    int y,dly=0,dry=0;
    int width = 40;
    int height = 80;
    YsRawPngDecoder frames[26];
    GLuint texIds[2];
    GLuint playID[26];
    //GLuint playID[2];
    int currentFrame;
    
    //class Game;
    
    PlayerCharacter(int initialX, int initialY) : x(initialX), y(initialY), currentFrame(0) {
        // Additional initialization if needed
        const char* filenames[26] = { "sprite_00.png", "sprite_01.png","sprite_02.png", "sprite_03.png","sprite_04.png", "sprite_05.png","sprite_06.png", "sprite_07.png","sprite_08.png", "sprite_09.png","sprite_10.png", "sprite_11.png","sprite_12.png", "sprite_13.png","sprite_14.png", "sprite_15.png","sprite_16.png", "sprite_17.png","sprite_18.png","sprite_19.png", "sprite_20.png","sprite_21.png", "sprite_22.png","sprite_23.png","sprite_24.png", "sprite_25.png"};
        //const char* filenames[2] = { "sprite_00.png", "sprite_01.png"};
        YsRawPngDecoder player;
        for (int i = 0; i < 26; ++i) {
            if (YSOK == frames[i].Decode(filenames[i])) {
                frames[i].Flip();
                
                glGenTextures(1,&playID[i]);  // Reserve one texture identifier
                glBindTexture(GL_TEXTURE_2D,playID[i]);  // Making the texture identifier current (or bring it to the deck)
                
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
                
                glTexImage2D
                (GL_TEXTURE_2D,
                 0,    // Level of detail
                 GL_RGBA,
                 frames[i].wid,
                 frames[i].hei,
                 0,    // Border width, but not supported and needs to be 0.
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 frames[i].rgba);
            }
            else
            {
                printf("Read Error!\n");
                return;
            }
        }
    }
    ~PlayerCharacter(){
        for (int i = 0; i < 26; ++i) {
            glDeleteTextures(1, &playID[i]);
        }
    }
    
    void draw() {
        
        if (playID[currentFrame] != 0) {
            glEnable(GL_TEXTURE);
            glEnable(GL_TEXTURE_2D);  // <- Easy to forget
            glBindTexture(GL_TEXTURE_2D,playID[currentFrame]);
            glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
            
            glColor4f(1,1,1,1);
            
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            
            glBegin(GL_TRIANGLE_FAN);
            
            
            glBegin(GL_QUADS);
            glTexCoord2d(0.0, 1.0); glVertex2i(x - frames[currentFrame].wid / 2, y - frames[currentFrame].hei / 2);
            glTexCoord2d(1.0, 1.0); glVertex2i(x + frames[currentFrame].wid / 2, y - frames[currentFrame].hei / 2);
            glTexCoord2d(1.0, 0.0); glVertex2i(x + frames[currentFrame].wid / 2, y + frames[currentFrame].hei / 2);
            glTexCoord2d(0.0, 0.0); glVertex2i(x - frames[currentFrame].wid / 2, y + frames[currentFrame].hei / 2);
            glEnd();
            
            glDisable(GL_BLEND);
            glDisable(GL_TEXTURE_2D);
        }
        //std::cout << "Player character drawn at position (" << x << ", " << y << ")\n";
    }
    void Update(int key) {
        if (FsGetKeyState(FSKEY_LEFT) ) {
            x  -= 4; // Move left only when the left arrow key is pressed
            dlx += 1;
            drx = 0;
            
            if (dlx >= 3) {
                ///*
                if (currentFrame!=7) {
                    currentFrame=7;
                }
                else if (currentFrame!=8) {
                    currentFrame=8;
                }
                // */
                /*
                 if (currentFrame!=24) {
                 currentFrame=24;
                 }
                 else if (currentFrame!=25) {
                 currentFrame=25;
                 }
                 */
                dlx=0;
            }
        }
        if (FsGetKeyState(FSKEY_RIGHT)) {
            x += 4; // Move left only when the left arrow key is pressed
            drx += 1;
            dlx = 0;
            
            
            
            if (drx >= 3) {
                ///*
                if (currentFrame!=0) {
                    currentFrame=0;
                }
                else if (currentFrame!=1) {
                    currentFrame=1;
                }
                // */
                /*
                 if (currentFrame!=22) {
                 currentFrame=22;
                 }
                 else if (currentFrame!=23) {
                 currentFrame=23;
                 }
                 */
                drx=0;
            }
        }
        if (FsGetKeyState(FSKEY_UP) ) {
            y -= 4; // Move left only when the left arrow key is pressed
            
            dly += 1;
            
            if (dly >= 3) {
                if (currentFrame!=16) {
                    currentFrame=16;
                }
                else if (currentFrame!=17) {
                    currentFrame=17;
                }
                dly = 0;
            }
        }
        if (FsGetKeyState(FSKEY_DOWN) ) {
            y += 4; // Move left only when the left arrow key is pressed
            
            dry += 1;
            
            if (dry >= 3) {
                if (currentFrame!=19) {
                    currentFrame=19;
                }
                else if (currentFrame!=18) {
                    currentFrame=18;
                }
                dry = 0;
            }
        }
    }
    void drawAttack() {
        auto begin = std::chrono::high_resolution_clock::now();
        if (currentFrame==0 || currentFrame==1) {
            for (;;) {
                //currentFrame=3;
                //currentFrame=4;
                currentFrame=5;
                //draw();
                break;
                
                
            }
        }
        else if (currentFrame==7 || currentFrame==8) {
            for (;;) {
                //currentFrame=10;
                //currentFrame=11;
                currentFrame=12;
                //draw();
                break;
                
                
            }
        }
        else if (currentFrame==16 || currentFrame==17) {
            for (;;) {
                
                //currentFrame=14;
                currentFrame=15;
                //draw();
                break;
                
                
            }
        }
        else if (currentFrame==18 || currentFrame==19) {
            for (;;) {
                //currentFrame=20;
                currentFrame=21;
                //draw();
                break;
                
            }
        }
    }
    
    
};


class EnergyWave {
public:
    bool active;
    float x, y;
    float width, height;
    float speed;
    float maxLength;
    float alpha; // Transparency for flashing effect
    float alphaChangeRate; // Rate at which alpha changes
    float circleRadius, maxCircleRadius, circleGrowthRate;
    bool isCharging;

    EnergyWave() : active(false), x(0), y(0), width(0), height(10), speed(5), maxLength(200),
        alpha(1.0f), alphaChangeRate(0.05f), circleRadius(0), maxCircleRadius(15), circleGrowthRate(1), isCharging(true) {}

    void Activate(float startX, float startY) {
        active = true;
        isCharging = true;
        circleRadius = 0;
        x = startX;
        y = startY;
        width = 0;
        alpha = 1.0f; // Reset alpha
    }

    void Update() {
        if (active) {
            if (isCharging) {
                circleRadius += circleGrowthRate;
                if (circleRadius >= maxCircleRadius) {
                    isCharging = false; // End charging phase
                }
            }
            else {
                // Increase width continuously until it reaches the left edge of the screen
                if (x - width > 0) {
                    width += speed;
                }
                else {
                    active = false; // Deactivate if the end of the beam has reached the left edge
                }
            }

            // Update alpha for flashing effect
            alpha += alphaChangeRate;
            if (alpha > 1.0f || alpha < 0.5f) {
                alphaChangeRate = -alphaChangeRate; // Reverse direction when limits are reached
            }
        }
    }


    void Draw() {
        if (active) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // Draw the charging circle
            DrawGradientCircle(x - 30, y, circleRadius, 0.0, 0.0, 1.0, alpha); // Blue color

            // Draw the rectangle wave
            if (!isCharging) {
                glBegin(GL_QUADS);
                glColor4f(0.0f, 1.0f, 0.0f, alpha); // Green color at the left side of the beam
                glVertex2f(x - width - 30, y - height / 2);
                glVertex2f(x - width - 30, y + height / 2);

                glColor4f(1.0f, 1.0f, 0.0f, alpha); // Yellow color at the right side of the beam
                glVertex2f(x - 30, y + height / 2);
                glVertex2f(x - 30, y - height / 2);
                glEnd();
            }


            glDisable(GL_BLEND);
        }
    }

    void DrawGradientCircle(float cx, float cy, float radius, float r, float g, float b, float alpha) {
        glBegin(GL_TRIANGLE_FAN);
        for (int i = 0; i <= 360; i++) {
            float theta = 2.0f * 3.1415926f * float(i) / float(360);
            float x = radius * cosf(theta);
            float y = radius * sinf(theta);
            float gradient = (theta + 90) / 360.0f;
            glColor4f(r * gradient, g * gradient, b * gradient, alpha);
            glVertex2f(x + cx, y + cy);
        }
        glEnd();
    }
};




class EnemyCharacter {
public:
    YsRawPngDecoder frames[2];
    GLuint texIds[2];
    int currentFrame;
    EnergyWave wave;
    int x, y;
    bool State;
    int height()
    {
        return wave.height;
    }
    
    int width(){return wave.width;}
    int xret(){return wave.x;}
    int yret(){return wave.y;}

    EnemyCharacter(int startX, int startY) : currentFrame(0), x(startX), y(startY), State(false){
        const char* filenames[2] = { "enemy_frame1.png", "enemy_frame2.png" };
        for (int i = 0; i < 2; ++i) {
            if (YSOK == frames[i].Decode(filenames[i])) {
                frames[i].Flip();

                glGenTextures(1, &texIds[i]);
                glBindTexture(GL_TEXTURE_2D, texIds[i]);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frames[i].wid, frames[i].hei, 0, GL_RGBA, GL_UNSIGNED_BYTE, frames[i].rgba);
            }
            else {
                printf("Failed to load %s\n", filenames[i]);
            }
        }
    }

    ~EnemyCharacter() {
        for (int i = 0; i < 2; ++i) {
            glDeleteTextures(1, &texIds[i]);
        }
    }

    void Draw() {
        if (State) {
            if (texIds[currentFrame] != 0) {
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, texIds[currentFrame]);
                
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                glColor4d(1.0, 1.0, 1.0, 1.0);
                
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                
                glBegin(GL_QUADS);
                glTexCoord2d(0.0, 1.0); glVertex2i(x - frames[currentFrame].wid / 2, y - frames[currentFrame].hei / 2);
                glTexCoord2d(1.0, 1.0); glVertex2i(x + frames[currentFrame].wid / 2, y - frames[currentFrame].hei / 2);
                glTexCoord2d(1.0, 0.0); glVertex2i(x + frames[currentFrame].wid / 2, y + frames[currentFrame].hei / 2);
                glTexCoord2d(0.0, 0.0); glVertex2i(x - frames[currentFrame].wid / 2, y + frames[currentFrame].hei / 2);
                glEnd();
                
                glDisable(GL_BLEND);
                glDisable(GL_TEXTURE_2D);
            }
            wave.Update();
            wave.Draw();
        }
    }

    void Update(int key) {
        if (FsGetKeyState(FSKEY_A) && !wave.active) {
            x -= 4; // Move left
            currentFrame = (currentFrame + 1) % 2;
        }
        else if (FsGetKeyState(FSKEY_D) && !wave.active) {
            x += 4; // Move right
            currentFrame = (currentFrame + 1) % 2;
        }
        else if (FsGetKeyState(FSKEY_W) && !wave.active) {
            y -= 4; // Move up
            currentFrame = (currentFrame + 1) % 2;
        }
        else if (FsGetKeyState(FSKEY_S) && !wave.active) {
            y += 4; // Move down
            currentFrame = (currentFrame + 1) % 2;
        }

        if (FsGetKeyState(FSKEY_TAB) && !wave.active) {
            wave.Activate(x, y); // Activate wave on SPACE key
        }
    }
};
//Yashraj Shanker'  s Code

class Background
{
private:
    int playerX, playerY;   // Player position
    int playerSize;         // Player size
    int playerSpeed;        // Movement speed in pixels
    
    YsRawPngDecoder png;
    YsRawPngDecoder A_png[5];
    std::vector<int> mapHistory; // History of maps visited
    GLuint textureIDs[3];        // Array to store texture IDs
    int numberOfTextures;        // Number of textures loaded
    
    GLuint abilitytextureIDs[4];        // Array to store texture IDs
    int numberOfabilities;
    
    void LoadMap(int mapIndex);
    void Loadability(int ability);
    
    
//    void DrawPlayer() const
//    {
//        glColor3ub(255, 0, 0); // Red color for the player
//        glBegin(GL_QUADS);
//        glVertex2i(playerX - playerSize, playerY - playerSize);
//        glVertex2i(playerX + playerSize, playerY - playerSize);
//        glVertex2i(playerX + playerSize, playerY + playerSize);
//        glVertex2i(playerX - playerSize, playerY + playerSize);
//        glEnd();
//    }
    
    
    
public:
    void MovePlayer(int dx, int dy);
    class PlayerCharacter;
    //class PlayerCharacter;
    //void Render();
    Background() : playerX(400), playerY(300), playerSize(20), playerSpeed(10)
    {
        //FsOpenWindow(16,16,800,600,1);
        FsChangeToProgramDir();
        mapHistory.push_back(0); // Start with the first map
        Loadability(0);
        LoadMap(0);
        
    }
    ~Background();//{
//        for (int i = 0; i < numberOfTextures; ++i) {
//                glDeleteTextures(1, &fileNames[i]);
//            }
//
//    }
    void Render();
    void RenderAbility();
    void Run();
    
    GLuint abilityID[5];
};


void Background::LoadMap(int mapIndex)
{
    const char *fileNames[] = {"Power_Acquiring.png", "Post_Power.png", "Map4.png"};
    if (mapIndex >= 0 && mapIndex < 3)
    {
        GLuint texID;
        if (YSOK == png.Decode(fileNames[mapIndex]))
        {
            png.Flip();
            glGenTextures(1, &texID);
            glBindTexture(GL_TEXTURE_2D, texID);
            textureIDs[numberOfTextures++] = texID;
        }
        else
        {
            printf("Read Error!\n");
        }
    }
}
void Background::Loadability(int ability)
{
    const char *fileNames[5] = {"B.png", "flash.png", "hearts.png", "swing.png"};
    for (int ability = 0;  ability < 4; ability++ )
    {
        //GLuint abilityID[5];
        if (YSOK == A_png[ability].Decode(fileNames[ability]))
        {
            //A_png[ability].Flip();
            glGenTextures(1, &abilityID[ability]);
            glBindTexture(GL_TEXTURE_2D, abilityID[ability]);
            //abilitytextureIDs[ability++] = abilityID;
            
            glGenTextures(1,&abilityID[ability]);  // Reserve one texture identifier
            glBindTexture(GL_TEXTURE_2D,abilityID[ability]);  // Making the texture identifier current (or bring it to the deck)
            
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
            
            glTexImage2D
            (GL_TEXTURE_2D,
             0,    // Level of detail
             GL_RGBA,
             A_png[ability].wid,
             A_png[ability].hei,
             0,    // Border width, but not supported and needs to be 0.
             GL_RGBA,
             GL_UNSIGNED_BYTE,
             A_png[ability].rgba);
        }
        else
        {
            printf("Read Error!\n");
        }
    }
}
Background::~Background()
{
    for (int i = 0; i < numberOfTextures; ++i)
    {
        glDeleteTextures(1, &textureIDs[i]);
    }
//    for (int i = 0; i < 5; ++i)
//    {
//        glDeleteTextures(1, &abilityID[i]);
//    }
}

//PlayerCharacter player(400,300);


PlayerCharacter player(400,300);

void Background::MovePlayer(int dx, int dy)
{
//    playerX += dx * playerSpeed;
//    playerY += dy * playerSpeed;
    
    playerX = dx;//player.x;
    playerY = dy;//player.y;
    
    //std::cout << playerX;
    // Check for left boundary
    if (playerX < 0 && mapHistory.size() > 1)
    {
        
        mapHistory.pop_back(); // Remove the current map
        int previousMap = mapHistory.back(); // Get the previous map
        LoadMap(previousMap);
        //playerX = 800 - playerSize; // Place player on the right side of the screen
        player.x=800;
    }
    
    // Check for right boundary
    if (playerX > 800)
    {
        int nextMap = (mapHistory.back() + 1) % 3;
        mapHistory.push_back(nextMap);
        LoadMap(nextMap);
        //playerX = 0; // Place player on the left side of the screen
        player.x=0;
    }
}

//    void DrawPlayer() const
//    {
//        glColor3ub(255, 0, 0); // Red color for the player
//        glBegin(GL_QUADS);
//        glVertex2i(playerX - playerSize, playerY - playerSize);
//        glVertex2i(playerX + playerSize, playerY - playerSize);
//        glVertex2i(playerX + playerSize, playerY + playerSize);
//        glVertex2i(playerX - playerSize, playerY + playerSize);
//        glEnd();
//    }

void Background::Render()
{
    //glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    
    int winWid,winHei;
    FsGetWindowSize(winWid,winHei);
    glRasterPos2d(0.0,(double)(winHei-1));
    glDrawPixels(png.wid,png.hei,GL_RGBA,GL_UNSIGNED_BYTE,png.rgba);
    //DrawPlayer();
    //FsSwapBuffers();
}

void Background::RenderAbility()
{
    //glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
    
//    int winWid,winHei;
//    FsGetWindowSize(winWid,winHei);
//    glRasterPos2d(0.0,(double)(winHei-1));
//    glDrawPixels(png.wid,png.hei,GL_RGBA,GL_UNSIGNED_BYTE,png.rgba);
//    //DrawPlayer();
//    //FsSwapBuffers();
    
    glEnable(GL_TEXTURE);
    glEnable(GL_TEXTURE_2D);  // <- Easy to forget
    glBindTexture(GL_TEXTURE_2D,abilityID[3]);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    
    glColor4f(1,1,1,1);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glBegin(GL_TRIANGLE_FAN);
    glBegin(GL_QUADS);
    glTexCoord2d(0.0,0.0);
    glVertex2d(525,515);
    glTexCoord2d(1.0,0.0);
    glVertex2d(575,515);
    glTexCoord2d(1.0,1.0);
    glVertex2d(575,565);
    glTexCoord2d(0.0,1.0);
    glVertex2d(525,565);
    glEnd();
    
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    
    glEnable(GL_TEXTURE);
    glEnable(GL_TEXTURE_2D);  // <- Easy to forget
    glBindTexture(GL_TEXTURE_2D,abilityID[2]);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    
    glColor4f(1,1,1,1);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glBegin(GL_TRIANGLE_FAN);
    glBegin(GL_QUADS);
    glTexCoord2d(0.0,0.0);
    glVertex2i(10,-25);
    glTexCoord2d(1.0,0.0);
    glVertex2i(110,-25);
    glTexCoord2d(1.0,1.0);
    glVertex2i(110,75);
    glTexCoord2d(0.0,1.0);
    glVertex2i(10,75);
    glEnd();
    
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    

    glEnable(GL_TEXTURE);
    glEnable(GL_TEXTURE_2D);  // <- Easy to forget
    glBindTexture(GL_TEXTURE_2D,abilityID[0]);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

    glColor4f(1,1,1,1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_TRIANGLE_FAN);
    glBegin(GL_QUADS);
    glTexCoord2d(0.0,0.0);
    glVertex2d(675,475);
    glTexCoord2d(1.0,0.0);
    glVertex2d(775,475);
    glTexCoord2d(1.0,1.0);
    glVertex2d(775,575);
    glTexCoord2d(0.0,1.0);
    glVertex2d(675,575);
    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

//PlayerCharacter player(400,300);
void Background::Run()
{
    int key = FSKEY_NULL;
    //if (FSKEY_ESC != key)
    //{
        //FsPollDevice();
        //key = FsInkey();
        
//        if (FSKEY_LEFT == key) { MovePlayer(-1, 0);}
//        if (FSKEY_RIGHT == key) { MovePlayer(1, 0);}
//        if (FSKEY_UP == key) { MovePlayer(0, -1);}
//        if (FSKEY_DOWN == key) { MovePlayer(0, 1); player.Update(key);}
        
//        if (FSKEY_LEFT == key) { player.Update(key);}
//        if (FSKEY_RIGHT == key) {player.Update(key);}
//        if (FSKEY_UP == key) {player.Update(key);}
//        if (FSKEY_DOWN == key) {player.Update(key);}
        
        //Render();
        FsSleep(50);
    //}
}

//int main(void)
//{
//    Background game;
//    game.Run();
//    return 0;
//}
// End of Yashraj Shanker's Code


//Rayhan



//int main() {
//    FsOpenWindow(16, 16, windowWidth, windowHeight, 1);
//    EnemyCharacter enemy1(windowWidth * 3 / 4, windowHeight / 4);
//    EnemyCharacter enemy2(windowWidth * 3 / 4, windowHeight / 2);
//    EnemyCharacter enemy3(windowWidth * 3 / 4, windowHeight * 3 / 4);
//
//    while (true) {
//        FsPollDevice();
//        int key = FsInkey();
//        if (key == FSKEY_ESC) {
//            break;
//        }
//
//        if (key == FSKEY_LEFT || key == FSKEY_RIGHT || key == FSKEY_UP || key == FSKEY_DOWN || key == FSKEY_SPACE) {
//            enemy1.Update(key);
//            enemy2.Update(key);
//            enemy3.Update(key);
//        }
//
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        enemy1.Draw();
//        enemy2.Draw();
//        enemy3.Draw();
//        FsSwapBuffers();
//        FsSleep(25);
//    }
//
//    return 0;
//}


class Player1 {
public:
    int x, y;
    int width, height;
    int speed;

    Player1(int startX, int startY, int w, int h, int spd) : x(startX), y(startY), width(w), height(h), speed(spd) {}

    void UpdatePosition(int newX, int newY) {
        x = newX;
        y = newY;
    }

    void Draw(int r, int g, int b) {
        glColor3ub(r, g, b);
        glBegin(GL_QUADS);
        glVertex2i(x, y);
        glVertex2i(x + width, y);
        glVertex2i(x + width, y + height);
        glVertex2i(x, y + height);
        glEnd();
    }
};

class Missile {
public:
    int x, y;
    int width, height;
    int speed;
    bool isActive;
    int dirX, dirY;

    Missile() : x(0), y(0), width(10), height(40), speed(8), isActive(false), dirX(0), dirY(0) {}

    void Move() {
        x += dirX * speed;
        y += dirY * speed;
    }

    void Draw() {
        glColor3f(0.8, 0.5, 0);
        glBegin(GL_QUADS);
        glVertex2i(x, y);
        glVertex2i(x + width, y);
        glVertex2i(x + width, y + height);
        glVertex2i(x, y + height);
        glEnd();
        
        glColor3f(0, 0, 0);
        glBegin(GL_LINES);
        glVertex2i(x + 8, y);
        glVertex2i(x + 8, y+height);
        glEnd();
        
        glColor3f(0, 0, 0);
        glBegin(GL_LINES);
        glVertex2i(x + 18, y);
        glVertex2i(x + 18, y+height);
        glEnd();
        
        glColor3f(0, 0, 0);
        glBegin(GL_LINES);
        glVertex2i(x + 28, y);
        glVertex2i(x + 28, y+height);
        glEnd();
    }
};

class Villain {
public:
    int x, y;
    int width, height;

    Villain(int startX, int startY, int w, int h) : x(startX), y(startY), width(w), height(h) {}

    void Draw(int r, int g, int b) {
        glColor3ub(r, g, b);
        glBegin(GL_QUADS);
        glVertex2i(x, y);
        glVertex2i(x + width, y);
        glVertex2i(x + width, y + height);
        glVertex2i(x, y + height);
        glEnd();
    }
};

class Attack {
//private:
//    Player1 hero;
//    Missile heroMissile;
//    Villain villain;

public:
    //class PlayerCharacter
    //Player1 hero;
    
    PlayerCharacter hero;
    Missile heroMissile;
    Villain villain;
    int a,b;
    int xret=hero.x,yret=hero.y;
    bool active;
    int currentFrame=0;
    
    Attack() : hero(100, 100), heroMissile(), villain(400, 400, 30, 30) {}
    //Attack() :   heroMissile(), villain(400, 400, 30, 30) {}
    
    int a1=heroMissile.height,b1=heroMissile.width;
    int a2=heroMissile.width,b2=heroMissile.height;
    
    void Attack_Attack(int key,int x, int y,int Frame) {
        active=heroMissile.isActive;
        xret=heroMissile.x;
        yret=heroMissile.y;
        
        //while (FSKEY_ESC != FsInkey()) {
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Handle hero movement
            //FsPollDevice();
//            int key = FsInkey();
//            if (FsGetKeyState(FSKEY_LEFT) || key == FSKEY_LEFT) {
//                hero.UpdatePosition(hero.x - hero.speed, hero.y);
//            }
        //            if (FsGetKeyState(FSKEY_RIGHT) || key == FSKEY_RIGHT) {
        //                hero.UpdatePosition(hero.x + hero.speed, hero.y);
        //            }
        //            if (FsGetKeyState(FSKEY_UP) || key == FSKEY_UP) {
        //                hero.UpdatePosition(hero.x, hero.y - hero.speed);
        //            }
        //            if (FsGetKeyState(FSKEY_DOWN) || key == FSKEY_DOWN) {
        //                hero.UpdatePosition(hero.x, hero.y + hero.speed);
        //            }
        
        
        //Frame=hero.currentFrame;
        //std::cout << "||" << Frame << "\n";
        //Handle missile attack
        currentFrame=Frame;
        if ((currentFrame==16 || currentFrame==17) && !heroMissile.isActive && FsGetKeyState(FSKEY_SPACE)) {
            heroMissile.isActive = true;
            heroMissile.x = x+15; //+ hero.width / 2 - heroMissile.width / 2;
            heroMissile.y = y;// - heroMissile.height;
            heroMissile.dirX = 0;
            heroMissile.dirY = -1;
            
            heroMissile.width=b1;
            heroMissile.height=a1;
            
            //currentFrame=100;
        }
        else if ((currentFrame==0 || currentFrame==1) && !heroMissile.isActive && FsGetKeyState(FSKEY_SPACE)) {
            heroMissile.isActive = true;
            heroMissile.x = x+10;// + hero.width;
            heroMissile.y = y-20;// + hero.height / 2 - heroMissile.height / 2;
            heroMissile.dirX = 1;
            heroMissile.dirY = 0;
            
            heroMissile.width=b2;
            heroMissile.height=a2;
            
            //currentFrame=100;
        }
        else if ((currentFrame==7 || currentFrame==8) && !heroMissile.isActive && FsGetKeyState(FSKEY_SPACE)) {
            heroMissile.isActive = true;
            heroMissile.x = x-20;// - heroMissile.width;
            heroMissile.y = y-20;// + hero.height / 2 - heroMissile.height / 2;
            heroMissile.dirX = -1;
            heroMissile.dirY = 0;
            
            heroMissile.width=b2;
            heroMissile.height=a2;
            
            //currentFrame=100;
        }
        else if ((currentFrame==18 || currentFrame==19) && !heroMissile.isActive && FsGetKeyState(FSKEY_SPACE)) {
            heroMissile.isActive = true;
            heroMissile.x = x+20;// + hero.width / 2 - heroMissile.width / 2;
            heroMissile.y = y-15;// + hero.height;
            heroMissile.dirX = 0;
            heroMissile.dirY = 1;
            
            heroMissile.width=b1;
            heroMissile.height=a1;
            
            //currentFrame=100;
        }
            
//        if ((FsGetKeyState(FSKEY_2) || key == FSKEY_2) && !heroMissile.isActive) {
//            heroMissile.isActive = true;
//            heroMissile.x = x+15; //+ hero.width / 2 - heroMissile.width / 2;
//            heroMissile.y = y;// - heroMissile.height;
//            heroMissile.dirX = 0;
//            heroMissile.dirY = -1;
//
//            heroMissile.width=b1;
//            heroMissile.height=a1;
//        }
//        else if ((FsGetKeyState(FSKEY_3) || key == FSKEY_3) && !heroMissile.isActive) {
//            heroMissile.isActive = true;
//            heroMissile.x = x+10;// + hero.width;
//            heroMissile.y = y-20;// + hero.height / 2 - heroMissile.height / 2;
//            heroMissile.dirX = 1;
//            heroMissile.dirY = 0;
//
//            heroMissile.width=b2;
//            heroMissile.height=a2;
//        }
//        else if ((FsGetKeyState(FSKEY_4) || key == FSKEY_4) && !heroMissile.isActive) {
//            heroMissile.isActive = true;
//            heroMissile.x = x-20;// - heroMissile.width;
//            heroMissile.y = y-20;// + hero.height / 2 - heroMissile.height / 2;
//            heroMissile.dirX = -1;
//            heroMissile.dirY = 0;
//
//            heroMissile.width=b2;
//            heroMissile.height=a2;
//
//        }
//        else if ((FsGetKeyState(FSKEY_5) || key == FSKEY_5) && !heroMissile.isActive) {
//            heroMissile.isActive = true;
//            heroMissile.x = x+20;// + hero.width / 2 - heroMissile.width / 2;
//            heroMissile.y = y-15;// + hero.height;
//            heroMissile.dirX = 0;
//            heroMissile.dirY = 1;
//
//
//            heroMissile.width=b1;
//            heroMissile.height=a1;
//
//        }
        
        // Check for keys 5, 6, 7, 8 and hero proximity to villain
        if ((FsGetKeyState(FSKEY_6) || key == FSKEY_6) ||
            (FsGetKeyState(FSKEY_7) || key == FSKEY_7) ||
            (FsGetKeyState(FSKEY_8) || key == FSKEY_8) ||
            (FsGetKeyState(FSKEY_9) || key == FSKEY_9)) {
            
            // Check if the hero is within 10 pixels of the villain
            int dx = hero.x - villain.x;
            int dy = hero.y - villain.y;
            int distance = static_cast<int>(std::sqrt(dx * dx + dy * dy));
            
            //                if (distance <= 35) {
            //                    printf("Game Over! Hero too close to the villain.\n");
            //                    break;
            //                }
        }
        
        // Update and draw missile
        if (heroMissile.isActive) {
            heroMissile.Move();
            heroMissile.Draw();
            
            // Check collision with villain
            if ((heroMissile.x < villain.x + villain.width) &&
                (heroMissile.x + heroMissile.width > villain.x) &&
                (heroMissile.y < villain.y + villain.height) &&
                (heroMissile.y + heroMissile.height > villain.y)) {
                printf("Game Over! Missile hit the villain.\n");
                //break;
            }
            
            else if (heroMissile.y < 0 || heroMissile.x < 0 || heroMissile.x + heroMissile.width > 800 || heroMissile.y + heroMissile.height > 600) {
                heroMissile.isActive = false;
            }
        }
        
        // Draw characters
        //hero.Draw(0, 0, 255);
        //villain.Draw(255, 0, 0);
        
        //FsSwapBuffers();
        //FsSleep(25);
        //}
    }
};

//int main(void) {
//    Game2 game2;
//    game2.Run();
//
//    return 0;
//}



void RenderRocket(int rocketY)
{
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw the rocket at the updated Y position
    glColor3ub(255, 0, 0);  // Set color to red
    glBegin(GL_TRIANGLES);
    glVertex2i(400, 600 - rocketY);  // Top point (inverted Y position)
    glVertex2i(380, 650 - rocketY);  // Bottom left point
    glVertex2i(420, 650 - rocketY);  // Bottom right point
    glEnd();

    glColor3ub(255, 255, 0);  // Set color to yellow
    glBegin(GL_QUADS);
    glVertex2i(380, 650 - rocketY);  // Bottom left
    glVertex2i(420, 650 - rocketY);  // Bottom right
    glVertex2i(420, 700 - rocketY);  // Top right
    glVertex2i(380, 700 - rocketY);  // Top left
    glEnd();

    // Add more details or customize the design as needed

    FsSwapBuffers();
}

void Render(YsRawPngDecoder& png)
{
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int winWid, winHei;
    FsGetWindowSize(winWid, winHei);
    glRasterPos2d(0.0, (double)(winHei - 1));
    glDrawPixels(png.wid, png.hei, GL_RGBA, GL_UNSIGNED_BYTE, png.rgba);

    FsSwapBuffers();
}

int main() {
    FsChangeToProgramDir();
    FsOpenWindow(0,0,800,600,1);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //MovementInfoProvider movementProvider;
    PlayerCharacter player(200,200);
    Background game;
    Attack attack;
    Missile heroMissile;
    PlayerActions playerActions;

    //FsChangeToProgramDir();
    Menu menu;
    menu.Run();
    
    Attack game2;
    //game2.Run();
    
    int start = menu.start;
    int quit = menu.quit;
    int level=1;
    int health=3;
    int level_enemy = 1;
    
    MainData dat;
    
    EnemyCharacter enemy1(windowWidth * 3 / 4, windowHeight / 4);
    EnemyCharacter enemy2(windowWidth * 3 / 4, windowHeight / 2);
    EnemyCharacter enemy3(windowWidth * 3 / 4, windowHeight * 3 / 4);
    
    FsRegisterOnPaintCallBack(Render,&dat);
    
    
    YsRawPngDecoder png1;
    if (YSOK != png1.Decode("Single Planet.png"))
    {
        printf("Read Error for PNG 1!\n");
        return 0;
    }

    YsRawPngDecoder png2;
    if (YSOK != png2.Decode("Farmer Happy.png"))
    {
        printf("Read Error for PNG 2!\n");
        return 0;
    }

    YsRawPngDecoder png3;
    if (YSOK != png3.Decode("Happy and Prosperous.png"))
    {
        printf("Read Error for PNG 3!\n");
        return 0;
    }

    YsRawPngDecoder png4;
    if (YSOK != png4.Decode("Jealous.png"))
    {
        printf("Read Error for PNG 4!\n");
        return 0;
    }

    YsRawPngDecoder png5;
    if (YSOK != png5.Decode("Attack.png"))
    {
        printf("Read Error for PNG 5!\n");
        return 0;
    }

    YsRawPngDecoder png6;
    if (YSOK != png6.Decode("Attack 2.png"))
    {
        printf("Read Error for PNG 6!\n");
        return 0;
    }

    YsRawPngDecoder png7;
    if (YSOK != png7.Decode("Rocket To Earth.png"))
    {
        printf("Read Error for PNG 7!\n");
        return 0;
    }

    YsRawPngDecoder png8;
    if (YSOK != png8.Decode("Family.png"))
    {
        printf("Read Error for PNG 8!\n");
        return 0;
    }

    YsRawPngDecoder png9;
    if (YSOK != png9.Decode("Hero and Best Friend.png"))
    {
        printf("Read Error for PNG 9!\n");
        return 0;
    }

    YsRawPngDecoder png10;
    if (YSOK != png10.Decode("Friend Attack.png"))
    {
        printf("Read Error for PNG 10!\n");
        return 0;
    }

    YsRawPngDecoder png11;
    if (YSOK != png11.Decode("Goku Angry 1.png"))
    {
        printf("Read Error for PNG 11!\n");
        return 0;
    }

    YsRawPngDecoder png12;
    if (YSOK != png12.Decode("Goku Angry 2.png"))
    {
        printf("Read Error for PNG 12!\n");
        return 0;
    }

    const char* audioFileName = "Story1 (mp3cut.net).wav";
    YsSoundPlayer player1;
    player1.Start();

    YsSoundPlayer::SoundData wav;
    if (YSOK != wav.LoadWav(audioFileName))
    {
        printf("Failed to load audio file!\n");
        player1.End();
        return 0;
    }
    
    ////////
    YsRawPngDecoder png13;
    if (YSOK != png13.Decode("Boku Angry.png"))
    {
        printf("Read Error for PNG 1!\n");
        return 0;
    }

    YsRawPngDecoder png14;
    if (YSOK != png14.Decode("Boku Flying.png"))
    {
        printf("Read Error for PNG 2!\n");
        return 0;
    }

    YsRawPngDecoder png15;
    if (YSOK != png15.Decode("City Destroy.png"))
    {
        printf("Read Error for PNG 3!\n");
        return 0;
    }

    YsRawPngDecoder png16;
    if (YSOK != png16.Decode("Attack 2 copy.png"))
    {
        printf("Read Error for PNG 4!\n");
        return 0;
    }

    const char* audioFileName2 = "Story2.wav";
    YsSoundPlayer player2;
    player2.Start();

    YsSoundPlayer::SoundData wav2;
    if (YSOK != wav2.LoadWav(audioFileName2))
    {
        printf("Failed to load audio file!\n");
        player2.End();
        return 0;
    }
    
    const char* audioFileName3 = "Background3.wav";
    YsSoundPlayer player3;
    player3.Start();
    
    YsSoundPlayer::SoundData wav3;
    if (YSOK != wav3.LoadWav(audioFileName3))
    {
        printf("Failed to load audio file!\n");
        player3.End();
        return 0;
    }
    
    const char* audioFileName4 = "sad.wav";
    YsSoundPlayer player4;
    player4.Start();
    
    YsSoundPlayer::SoundData wav4;
    if (YSOK != wav4.LoadWav(audioFileName4))
    {
        printf("Failed to load audio file!\n");
        player4.End();
        return 0;
    }
    
    const char* audioFileName5 = "whoosh.wav";
    YsSoundPlayer player5;
    player5.Start();
    
    YsSoundPlayer::SoundData wav5;
    if (YSOK != wav5.LoadWav(audioFileName5))
    {
        printf("Failed to load audio file!\n");
        player5.End();
        return 0;
    }
    
    const char* audioFileName6 = "beam.wav";
    YsSoundPlayer player6;
    player6.Start();
    
    YsSoundPlayer::SoundData wav6;
    if (YSOK != wav6.LoadWav(audioFileName6))
    {
        printf("Failed to load audio file!\n");
        player6.End();
        return 0;
    }
    
    const char* audioFileName7 = "hurt.wav";
    YsSoundPlayer player7;
    player7.Start();
    
    YsSoundPlayer::SoundData wav7;
    if (YSOK != wav7.LoadWav(audioFileName7))
    {
        printf("Failed to load audio file!\n");
        player7.End();
        return 0;
    }
    
    int rocketY = 50;  // Initial Y position of the rocket below the screen

    long long int startTime = FsSubSecondTimer();
    long long int switchTime1 = 8000;  // Switch to png2 after 3 seconds
    long long int switchTime2 = 11500;  // Switch to png3 after 6 seconds
    long long int switchTime3 = 24000;  // Switch to png3 after 6 seconds
    long long int switchTime4 = 45000;  // Switch to png3 after 6 seconds
    long long int switchTime5 = 50000;  // Switch to rocket after 10 seconds
    long long int switchTime6 = 54000;  // Switch to rocket after 10 seconds
    //long long int switchTime7 = 58000;  // Switch to rocket after 10 seconds
    long long int switchTime8 = 63000;  // Switch to rocket after 10 seconds
    long long int switchTime9 = 68000;  // Switch to rocket after 10 seconds
    long long int switchTime10 = 80000;  // Switch to rocket after 10 seconds
    long long int switchTime11 = 90000;  // Switch to rocket after 10 seconds
    long long int switchTime12 = 100000;  // Switch to rocket after 10 seconds
    long long int switchTime13 = 103000;  // Switch to rocket after 10 seconds
    
    
    long long int startTime2;// = FsSubSecondTimer();
    long long int switchTime14 = 16000;  // Switch to png2 after 3 seconds
    long long int switchTime15 = 27000;  // Switch to png3 after 6 seconds
    long long int switchTime16 = 37000;  // Switch to png3 after 6 seconds
    long long int switchTime17 = 45000;  // Switch to png3 after 6 seconds


    auto key = FSKEY_NULL;
    enemy2.State=true;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for(;;)
    {
        if (start==0) {
            menu.Run();
            if (quit==1) {
                FsSleep(1000);
                FsCloseWindow();
                break;
            }
        }
        
        else if (start==1) {
            player1.PlayOneShot(wav);
            while (player1.IsPlaying(wav))
            {

                FsPollDevice();
                auto key=FsInkey();
                
                long long int currentTime = FsSubSecondTimer();
                if (key == FSKEY_ENTER) {
                    player1.Stop(wav);
                    break;
                }
                if (currentTime - startTime < switchTime1)
                {
                    Render(png1);
                }
                else if (currentTime - startTime < switchTime2)
                {
                    Render(png2);
                }
                else if (currentTime - startTime < switchTime3)
                {
                    Render(png3);
                }
                else if (currentTime - startTime < switchTime4)
                {
                    Render(png4);
                }
                else if (currentTime - startTime < switchTime5)
                {
                    Render(png5);
                }
                else if (currentTime - startTime < switchTime6)
                {
                    // After displaying the last PNG, update and render the rocket
                    rocketY += 2;  // Adjust the speed as needed
                    RenderRocket(rocketY);
                    
                    // If the rocket goes above the screen, exit the loop
                    //if (rocketY > 600)
                    //{
                    //break;
                    //}
                }
                
                //else if (currentTime - startTime < switchTime7)
                //{
                //Render(png6);
                //}
                
                else if (currentTime - startTime < switchTime8)
                {
                    Render(png7);
                }
                
                else if (currentTime - startTime < switchTime9)
                {
                    Render(png8);
                }
                
                else if (currentTime - startTime < switchTime10)
                {
                    Render(png9);
                }
                
                else if (currentTime - startTime < switchTime11)
                {
                    Render(png10);
                }
                
                else if (currentTime - startTime < switchTime12)
                {
                    Render(png11);
                }
                else if (currentTime - startTime < switchTime13){
                    Render(png12);
                }
                else  if (currentTime - startTime > switchTime13){
                    start=2;
                    break;
                }
                FsSleep(50);
                //FsSwapBuffers();
            }
            player1.End();
            start=2;
            //start+=1;
        }
            
        else if (start==2) {
            FsPollDevice();
            player3.PlayOneShot(wav3);
            auto key=FsInkey();
            if (key == FSKEY_ESC) {
                break;
            }
           
//            std::cout << enemy1.xret() << std::endl;
//            std::cout << enemy1.yret() << std::endl;
//            std::cout << enemy1.width() << std::endl;
//            std::cout <<enemy1.height() << std::endl;
//            std::cout <<enemy1.xret() -enemy1.width()<< std::endl;
//            std::cout << player.x << std::endl;
//            std::cout << player.y << std::endl;
            
            
            if (((enemy1.xret()-enemy1.width() > player.x-3) && (enemy1.xret()-enemy1.width() < player.x+3) && (enemy1.yret() < player.y+40) && (enemy1.yret() - enemy1.height() > player.y-40)) || ((enemy2.xret()-enemy2.width() > player.x-3) && (enemy2.xret()-enemy2.width() < player.x+3) && (enemy2.yret() < player.y+40) && (enemy2.yret() - enemy2.height() > player.y-40)) || ((enemy3.xret()-enemy3.width() > player.x-3) && (enemy3.xret()-enemy3.width() < player.x+3) && (enemy3.yret() < player.y+40) && (enemy3.yret() - enemy3.height() > player.y-40))) {
                printf("Game Over! Missile hit the villain.\n");
                //break;
                playerActions.ProcessInput(key, dat,true);
                //      player7.PlayOneShot(wav7);
                health-=1;
            }
            if (health<=0) {
                //start=0;
                //continue;
                break;
                //start=5;
            }
            
            
            if (((enemy1.x-10 > player.x-10) && (enemy1.x-10 < player.x+10) && (enemy1.y < player.y+40) && (enemy1.y - 40 > player.y-40) && enemy1.State==true) || ((enemy2.x-10 > player.x-10) && (enemy2.x-10 < player.x+10) && (enemy2.y < player.y+40) && (enemy2.y - 40 > player.y-40) && enemy2.State==true) || ((enemy3.x-40 > player.x-10) && (enemy3.x-10 < player.x+10) && (enemy3.y < player.y+40) && (enemy3.y - 40 > player.y-40) && enemy3.State==true)) {
                printf("Game Over! player hit the villain.\n");
                break;
            }
            
            if ((attack.xret > enemy1.x - 30 && attack.xret < enemy1.x + 30 && attack.yret > enemy1.y-50 && attack.yret < enemy1.y+30)) {
                //std::cout << attack.xret << "||" << enemy1.x << "\n";
                enemy1.State=false;
            }
            else if ((attack.xret > enemy2.x - 30 && attack.xret < enemy2.x + 30 && attack.yret > enemy2.y-50 && attack.yret < enemy2.y+30)) {
                //std::cout << attack.xret << "||" << enemy2.x << "\n";
                enemy2.State=false;
            }
            else if ((attack.xret > enemy3.x - 30 && attack.xret < enemy3.x + 30 && attack.yret > enemy3.y-50 && attack.yret < enemy3.y+30)) {
                //std::cout << attack.xret << "||" << enemy3.x << "\n";
                enemy3.State=false;
            }
            
           
            
            game.Render();
            game.RenderAbility();
            FsPushOnPaintEvent();
            
            playerActions.ProcessInput(key, dat,false); // Call the ProcessInput method to handle player actions
            
            // Ultimate
            if (dat.EnergyBar.energybar_increment <= 5) {
                dat.ultAbility.DrawUltimateNotReady();
            }
            
            
            
            //
            if (player.currentFrame==5) {
                player.currentFrame=1;
            }
            if (player.currentFrame==12) {
                player.currentFrame=8;
            }
            if (player.currentFrame==15) {
                player.currentFrame=16;
            }
            if (player.currentFrame==22) {
                player.currentFrame=18;
            }
            
            if (FsGetKeyState(FSKEY_A) || FsGetKeyState(FSKEY_D) || FsGetKeyState(FSKEY_W) || FsGetKeyState(FSKEY_S) || FsGetKeyState(FSKEY_TAB)) {
                enemy1.Update(key);
                enemy2.Update(key);
                enemy3.Update(key);
//                if (FsGetKeyState(FSKEY_TAB)) {
//                    player6.PlayOneShot(wav6);
//                }
            }
            if (FsGetKeyState(FSKEY_UP) || FsGetKeyState(FSKEY_DOWN)  || FsGetKeyState(FSKEY_LEFT)  || FsGetKeyState(FSKEY_RIGHT)  || FsGetKeyState(FSKEY_SPACE)) {
                if (player.x>795 && (enemy1.State==true || enemy2.State==true || enemy3.State==true)) {
                    player.x -= 5;
                    continue;
                }
                if (player.x<5 && (enemy1.State==true || enemy2.State==true || enemy3.State==true)) {
                    player.x += 5;
                    continue;
                }
                if (player.x<5 && level==1) {
                    player.x += 5;
                    continue;
                }
                if (player.y>595) {
                    player.y -= 5;
                    continue;
                }
                if (player.y<5) {
                    player.y += 5;
                    continue;
                }
                player.Update(key);
            }
            if (FsGetKeyState(FSKEY_SPACE) || attack.active) {
               
                int x=player.x;
                int y=player.y;
                
        
                int Frame=player.currentFrame;
                attack.Attack_Attack(key,x,y,Frame);
                
                if (FsGetKeyState(FSKEY_SPACE)){
                    player.Update(key);
                }
                player.drawAttack();
                //player5.PlayOneShot(wav5);
                
               
            }
            
            game.MovePlayer(player.x, player.y);
            
            if (player.x>800) {
                player.x=0;
                level+=1;
            }
            if (player.x<0) {
                player.x=800;
                level-=1;
            }
            if (level==2 && level_enemy==1) {
                enemy1.State=true;
                enemy3.State=true;
                level_enemy=2;
            }
            else if (level==3 && level_enemy==2) {
                enemy1.State=true;
                enemy2.State=true;
                enemy3.State=true;
                level_enemy=3;
            }
//            if(0!=FsGetKeyState(FSKEY_SPACE))
//            {
//                //dy=1;
//                //player.currentFrame=18;
//                //player.Update(key);
//                //player.drawAttack();
//            }
            
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            enemy1.Draw();
            enemy2.Draw();
            enemy3.Draw();
            
            
            // Render the player character
            player.draw();
//            Game2 game2;
//            game2.Run();
            
           
            if (enemy1.State==false && enemy2.State==false && enemy3.State == false && level == 3) {
                startTime2 = FsSubSecondTimer();
                player3.Stop(wav3);
                start=3;
            }
            
            FsSwapBuffers();
            FsSleep(20);
        }
        else if (start==3) {
            player2.PlayOneShot(wav2);
            
            while (player2.IsPlaying(wav2))
            {
                FsPollDevice();
                auto key=FsInkey();
                if (key == FSKEY_ESC) {
                    break;
                }
                
                long long int currentTime2 = FsSubSecondTimer();
                
                if (currentTime2 - startTime2 < switchTime14)
                {
                    Render(png13);
                }
                else if (currentTime2 - startTime2 < switchTime15)
                {
                    Render(png14);
                }
                else if (currentTime2 - startTime2 < switchTime16)
                {
                    Render(png15);
                }
                else if (currentTime2 - startTime2 < switchTime17)
                {
                    Render(png15);
                }
                //FsSwapBuffers();
                FsSleep(20);
            }
            break;
        }
        
    }
    
    return 0;
}
