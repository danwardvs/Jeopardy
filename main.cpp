#include <allegro.h>
#include <string>
#include"fmod/fmod.h"
#include"fmod/fmod_errors.h"

using namespace std;

#define SPLASH 0
#define MENU 1
#define GAME 2
#define QUESTION 3

#define A 0
#define B 1
#define C 2
#define D 3

#define buttonWidth 105;
#define buttonHeight 89;

//Images
BITMAP* buffer;
BITMAP* title;
BITMAP* cursor;
BITMAP* play;
BITMAP* playHover;
BITMAP* inGame;
BITMAP* intro;
BITMAP* buttonDown;
BITMAP* buttonAnswered;
BITMAP* buttons[4];

//Audio
FSOUND_STREAM* titleTheme;

SAMPLE* wrong;
SAMPLE* correct;

//Fonts
FONT *f1, *f2, *f3, *f4, *f5; 

int gameScreen;
int buttonID[2];

bool hovering;

//Tile
struct question{
  BITMAP* image[2];
  int x;
  int y;
  int answer;
  string questionText[6];
  bool answered;
}questionButtons[6][5];

//Buttons
struct button{
  BITMAP* image;
  int x;
  int y;
}multipleButtons[4];

//Collision
bool collision(float xMin1, float xMax1, float xMin2, float xMax2, float yMin1, float yMax1, float yMin2, float yMax2){
  if (xMin1 < xMax2 && yMin1 < yMax2 && xMin2 < xMax1 && yMin2 < yMax1){
    return true;
  }
  return false;
}

//Fade in
void highcolor_fade_in(BITMAP* bmp_orig, int speed){
  BITMAP* bmp_buff;
     
  if((bmp_buff=create_bitmap(SCREEN_W,SCREEN_H))){
    int a;
    if (speed<=0)speed=16;
      
    for(a=0;a<256;a+=speed){
      clear(bmp_buff);
      set_trans_blender(0,0,0,a);
      draw_trans_sprite(bmp_buff,bmp_orig,0,0);
      vsync();
      blit(bmp_buff,screen,0,0,0,0,SCREEN_W,SCREEN_H);
    }
    destroy_bitmap(bmp_buff);
  }
  blit(bmp_orig,screen,0,0,0,0,SCREEN_W,SCREEN_H);
}

//Fade out
void highcolor_fade_out(int speed){
  BITMAP* bmp_orig, *bmp_buff;
     
  if((bmp_orig=create_bitmap(SCREEN_W,SCREEN_H))){
    if((bmp_buff=create_bitmap(SCREEN_W,SCREEN_H))){
      int a;
      blit(screen,bmp_orig,0,0,0,0,SCREEN_W,SCREEN_H);
      if (speed<=0)speed=16;
      
      for(a=255-speed;a>0;a-=speed){
         clear(bmp_buff);
         set_trans_blender(0,0,0,a);
         draw_trans_sprite(bmp_buff,bmp_orig,0,0);
         vsync();
         blit(bmp_buff,screen,0,0,0,0,SCREEN_W,SCREEN_H);
      }
    destroy_bitmap(bmp_buff);
    }
    destroy_bitmap(bmp_orig);
  }
  rectfill(screen,0,0,SCREEN_W,SCREEN_H,makecol(0,0,0));
}

//Collision
bool collisionAny(int xMin1, int xMax1, int xMin2, int xMax2, int yMin1, int yMax1, int yMin2, int yMax2){
  if (xMin1 < xMax2 && yMin1 < yMax2 && xMin2 < xMax1 && yMin2 < yMax1){
    return true;
  }
  return false;
}

//Setup game
void setup(bool first){
  //Create buffer
  buffer = create_bitmap( 1024,768 );
  
  //Load images
  if (!(title = load_bitmap("images/title.bmp", NULL))){
    allegro_message("Cannot find image title.bmp\nPlease check your files and try again");
    exit(-1); 
  }
  if (!(cursor = load_bitmap("images/cursor.bmp", NULL))){
    allegro_message("Cannot find image cursor.bmp\nPlease check your files and try again");
    exit(-1); 
  }
  if (!(play = load_bitmap("images/play.bmp", NULL))){
    allegro_message("Cannot find image play.bmp\nPlease check your files and try again");
    exit(-1); 
  }
  if (!(playHover = load_bitmap("images/playHover.bmp", NULL))){
    allegro_message("Cannot find image playHover.bmp\nPlease check your files and try again");
    exit(-1); 
  }
  if (!(inGame = load_bitmap("images/inGame.bmp", NULL))){
    allegro_message("Cannot find image inGame.bmp\nPlease check your files and try again");
    exit(-1); 
  }
  if (!(intro = load_bitmap("images/intro.bmp", NULL))){
    allegro_message("Cannot find image intro.bmp\nPlease check your files and try again");
    exit(-1); 
  }
  if (!(buttonDown = load_bitmap("images/buttonDown.bmp", NULL))){
    allegro_message("Cannot find image buttonDown.bmp\nPlease check your files and try again");
    exit(-1); 
  }
  if (!(buttonAnswered = load_bitmap("images/buttonAnswered.bmp", NULL))){
    allegro_message("Cannot find image buttonAnswered.bmp\nPlease check your files and try again");
    exit(-1); 
  }
  if (!(buttons[0] = load_bitmap("images/a.bmp", NULL))){
    allegro_message("Cannot find image a.bmp\nPlease check your files and try again");
    exit(-1); 
  }
  if (!(buttons[1] = load_bitmap("images/b.bmp", NULL))){
    allegro_message("Cannot find image b.bmp\nPlease check your files and try again");
    exit(-1); 
  }
  if (!(buttons[2] = load_bitmap("images/c.bmp", NULL))){
    allegro_message("Cannot find image c.bmp\nPlease check your files and try again");
    exit(-1); 
  }
  if (!(buttons[3] = load_bitmap("images/d.bmp", NULL))){
    allegro_message("Cannot find image d.bmp\nPlease check your files and try again");
    exit(-1); 
  }
  
  //Load sounds
  if (!(titleTheme = FSOUND_Stream_Open( "audio/titleTheme.mp3",2,0,0))){
    allegro_message("Cannot find soundtrack titleTheme.mp3\nPlease check your files and try again");
    exit(-1);
  }
  if (!(wrong = load_sample("audio/wrong.wav"))){
    allegro_message("Cannot find sample wrong.wav\nPlease check your files and try again");
    exit(-1); 
  }
  if (!(correct = load_sample("audio/correct.wav"))){
    allegro_message("Cannot find sample correct.wav\nPlease check your files and try again");
    exit(-1); 
  }
  
  //Sets variables
  buttonID[0] = 0;
  buttonID[1] = 1;
  
  //Sets Font
  f1 = load_font("fonts/ScaKorinna.pcx", NULL, NULL);
  f2 = extract_font_range(f1, ' ', 'A'-1);
  f3 = extract_font_range(f1, 'A', 'Z');
  f4 = extract_font_range(f1, 'Z'+1, 'z');
 
  //Merge fonts 
  font = merge_fonts(f4, f5 = merge_fonts(f2, f3));
  
  //Setup answer buttons
  multipleButtons[0].image = buttons[0];
  multipleButtons[1].image = buttons[1];
  multipleButtons[2].image = buttons[2];
  multipleButtons[3].image = buttons[3];
  
  multipleButtons[0].x = 120;
  multipleButtons[0].y = 620;
  
  multipleButtons[1].x = 330;
  multipleButtons[1].y = 620;
  
  multipleButtons[2].x = 560;
  multipleButtons[2].y = 620;
  
  multipleButtons[3].x = 800;
  multipleButtons[3].y = 620;
  
  //Setup question buttons
  for(int i = 0; i < 6; i++){
    for(int t = 0; t < 5; t++){
      questionButtons[i][t].x = 200 + (i * 105);
      questionButtons[i][t].y = 180 + (t * 89);
      questionButtons[i][t].image[0] = buttonDown;
      questionButtons[i][t].image[1] = buttonAnswered;
      questionButtons[i][t].questionText[0] = "";
      questionButtons[i][t].questionText[1] = "";
      questionButtons[i][t].questionText[2] = "";
      questionButtons[i][t].questionText[3] = "";
      questionButtons[i][t].questionText[4] = "";
      questionButtons[i][t].questionText[5] = "";
      questionButtons[i][t].answer = A;
      questionButtons[i][t].answered = false;
    }
  }
  
  /*----------
  Questions
  ----------*/
  //Column One (Missing Voices)
  //200
  questionButtons[0][0].questionText[0] = "What was John A.";
  questionButtons[0][0].questionText[1] = "McDonalds Job?";
  questionButtons[0][0].questionText[2] = "";
  questionButtons[0][0].questionText[3] = "";
  questionButtons[0][0].questionText[4] = "(A) Painter (B) Contracter";
  questionButtons[0][0].questionText[5] = "(C) Prime Minister (D) Sanitary Engineer";
  questionButtons[0][0].answer = C;
  
  //400
  questionButtons[0][1].questionText[0] = "";
  questionButtons[0][1].questionText[1] = "";
  questionButtons[0][1].questionText[2] = "";
  questionButtons[0][1].questionText[3] = "";
  questionButtons[0][1].questionText[4] = "";
  questionButtons[0][1].questionText[5] = "(A) (B) (C) (D)";
  questionButtons[0][1].answer = B;
  
  //600
  questionButtons[0][2].questionText[0] = "";
  questionButtons[0][2].questionText[1] = "";
  questionButtons[0][2].questionText[2] = "";
  questionButtons[0][2].questionText[3] = "";
  questionButtons[0][2].questionText[4] = "";
  questionButtons[0][2].questionText[5] = "(A) (B) (C) (D)";
  questionButtons[0][2].answer = C;
  
  //800
  questionButtons[0][3].questionText[0] = "";
  questionButtons[0][3].questionText[1] = "";
  questionButtons[0][3].questionText[2] = "";
  questionButtons[0][3].questionText[3] = "";
  questionButtons[0][3].questionText[4] = "";
  questionButtons[0][3].questionText[5] = "(A) (B) (C) (D)";
  questionButtons[0][3].answer = D;
  
  //1000
  questionButtons[0][4].questionText[0] = "";
  questionButtons[0][4].questionText[1] = "";
  questionButtons[0][4].questionText[2] = "";
  questionButtons[0][4].questionText[3] = "";
  questionButtons[0][4].questionText[4] = "";
  questionButtons[0][4].questionText[5] = "(A) (B) (C) (D)";
  questionButtons[0][4].answer = B;
  
  
  
  
  //Column Two (John A. Mcdonald)
  //200
  questionButtons[1][0].questionText[0] = "";
  questionButtons[1][0].questionText[1] = "";
  questionButtons[1][0].questionText[2] = "";
  questionButtons[1][0].questionText[3] = "";
  questionButtons[1][0].questionText[4] = "";
  questionButtons[1][0].questionText[5] = "(A) (B) (C) (D)";
  questionButtons[1][0].answer = A;
  
  //400
  questionButtons[1][1].questionText[0] = "";
  questionButtons[1][1].questionText[1] = "";
  questionButtons[1][1].questionText[2] = "";
  questionButtons[1][1].questionText[3] = "";
  questionButtons[1][1].questionText[4] = "";
  questionButtons[1][1].questionText[5] = "(A) (B) (C) (D)";
  questionButtons[1][1].answer = A;
  
  //600
  questionButtons[1][2].questionText[0] = "";
  questionButtons[1][2].questionText[1] = "";
  questionButtons[1][2].questionText[2] = "";
  questionButtons[1][2].questionText[3] = "";
  questionButtons[1][2].questionText[4] = "";
  questionButtons[1][2].questionText[5] = "(A) (B) (C) (D)";
  questionButtons[1][2].answer = A;
  
  //800
  questionButtons[1][3].questionText[0] = "";
  questionButtons[1][3].questionText[1] = "";
  questionButtons[1][3].questionText[2] = "";
  questionButtons[1][3].questionText[3] = "";
  questionButtons[1][3].questionText[4] = "";
  questionButtons[1][3].questionText[5] = "(A) (B) (C) (D)";
  questionButtons[1][3].answer = A;
  
  //1000
  questionButtons[1][4].questionText[0] = "";
  questionButtons[1][4].questionText[1] = "";
  questionButtons[1][4].questionText[2] = "";
  questionButtons[1][4].questionText[3] = "";
  questionButtons[1][4].questionText[4] = "";
  questionButtons[1][4].questionText[5] = "(A) (B) (C) (D)";
  questionButtons[1][4].answer = A;
  
  
  
  //Column Three (London Conference)
  //200
  questionButtons[2][0].questionText[0] = "";
  questionButtons[2][0].questionText[1] = "";
  questionButtons[2][0].questionText[2] = "";
  questionButtons[2][0].questionText[3] = "";
  questionButtons[2][0].questionText[4] = "";
  questionButtons[2][0].questionText[5] = "(A) (B) (C) (D)";
  questionButtons[2][0].answer = A;
  
  //400
  questionButtons[2][1].questionText[0] = "";
  questionButtons[2][1].questionText[1] = "";
  questionButtons[2][1].questionText[2] = "";
  questionButtons[2][1].questionText[3] = "";
  questionButtons[2][1].questionText[4] = "";
  questionButtons[2][1].questionText[5] = "(A) (B) (C) (D)";
  questionButtons[2][1].answer = A;
  
  //600
  questionButtons[2][2].questionText[0] = "";
  questionButtons[2][2].questionText[1] = "";
  questionButtons[2][2].questionText[2] = "";
  questionButtons[2][2].questionText[3] = "";
  questionButtons[2][2].questionText[4] = "";
  questionButtons[2][2].questionText[5] = "(A) (B) (C) (D)";
  questionButtons[2][2].answer = A;
  
  //800
  questionButtons[2][3].questionText[0] = "";
  questionButtons[2][3].questionText[1] = "";
  questionButtons[2][3].questionText[2] = "";
  questionButtons[2][3].questionText[3] = "";
  questionButtons[2][3].questionText[4] = "";
  questionButtons[2][3].questionText[5] = "(A) (B) (C) (D)";
  questionButtons[2][3].answer = A;
  
  //1000
  questionButtons[2][4].questionText[0] = "";
  questionButtons[2][4].questionText[1] = "";
  questionButtons[2][4].questionText[2] = "";
  questionButtons[2][4].questionText[3] = "";
  questionButtons[2][4].questionText[4] = "";
  questionButtons[2][4].questionText[5] = "(A) (B) (C) (D)";
  questionButtons[2][4].answer = A;
  
  
  
  //Column Four
  //200
  questionButtons[3][0].questionText[0] = "";
  questionButtons[3][0].questionText[1] = "";
  questionButtons[3][0].questionText[2] = "";
  questionButtons[3][0].questionText[3] = "";
  questionButtons[3][0].questionText[4] = "";
  questionButtons[3][0].questionText[5] = "(A) (B) (C) (D)";
  questionButtons[3][0].answer = A;
  
  //400
  questionButtons[3][1].questionText[0] = "";
  questionButtons[3][1].questionText[1] = "";
  questionButtons[3][1].questionText[2] = "";
  questionButtons[3][1].questionText[3] = "";
  questionButtons[3][1].questionText[4] = "";
  questionButtons[3][1].questionText[5] = "(A) (B) (C) (D)";
  questionButtons[3][1].answer = A;
  
  //600
  questionButtons[3][2].questionText[0] = "";
  questionButtons[3][2].questionText[1] = "";
  questionButtons[3][2].questionText[2] = "";
  questionButtons[3][2].questionText[3] = "";
  questionButtons[3][2].questionText[4] = "";
  questionButtons[3][2].questionText[5] = "(A) (B) (C) (D)";
  questionButtons[3][2].answer = A;
  
  //800
  questionButtons[3][3].questionText[0] = "";
  questionButtons[3][3].questionText[1] = "";
  questionButtons[3][3].questionText[2] = "";
  questionButtons[3][3].questionText[3] = "";
  questionButtons[3][3].questionText[4] = "";
  questionButtons[3][3].questionText[5] = "(A) (B) (C) (D)";
  questionButtons[3][3].answer = A;
  
  //1000
  questionButtons[3][4].questionText[0] = "";
  questionButtons[3][4].questionText[1] = "";
  questionButtons[3][4].questionText[2] = "";
  questionButtons[3][4].questionText[3] = "";
  questionButtons[3][4].questionText[4] = "";
  questionButtons[3][4].questionText[5] = "(A) (B) (C) (D)";
  questionButtons[3][4].answer = A;
  
  
  
  //Column Five
  //200
  questionButtons[4][0].questionText[0] = "";
  questionButtons[4][0].questionText[1] = "";
  questionButtons[4][0].questionText[2] = "";
  questionButtons[4][0].questionText[3] = "";
  questionButtons[4][0].questionText[4] = "";
  questionButtons[4][0].questionText[5] = "(A) (B) (C) (D)";
  questionButtons[4][0].answer = A;
  
  //400
  questionButtons[4][1].questionText[0] = "";
  questionButtons[4][1].questionText[1] = "";
  questionButtons[4][1].questionText[2] = "";
  questionButtons[4][1].questionText[3] = "";
  questionButtons[4][1].questionText[4] = "";
  questionButtons[4][1].questionText[5] = "(A) (B) (C) (D)";
  questionButtons[4][1].answer = A;
  
  //600
  questionButtons[4][2].questionText[0] = "";
  questionButtons[4][2].questionText[1] = "";
  questionButtons[4][2].questionText[2] = "";
  questionButtons[4][2].questionText[3] = "";
  questionButtons[4][2].questionText[4] = "";
  questionButtons[4][2].questionText[5] = "(A) (B) (C) (D)";
  questionButtons[4][2].answer = A;
  
  //800
  questionButtons[4][3].questionText[0] = "";
  questionButtons[4][3].questionText[1] = "";
  questionButtons[4][3].questionText[2] = "";
  questionButtons[4][3].questionText[3] = "";
  questionButtons[4][3].questionText[4] = "";
  questionButtons[4][3].questionText[5] = "(A) (B) (C) (D)";
  questionButtons[4][3].answer = A;
  
  //1000
  questionButtons[4][4].questionText[0] = "";
  questionButtons[4][4].questionText[1] = "";
  questionButtons[4][4].questionText[2] = "";
  questionButtons[4][4].questionText[3] = "";
  questionButtons[4][4].questionText[4] = "";
  questionButtons[4][4].questionText[5] = "(A) (B) (C) (D)";
  questionButtons[4][4].answer = A;
  
  
  
  //Column Six
  //200
  questionButtons[5][0].questionText[0] = "";
  questionButtons[5][0].questionText[1] = "";
  questionButtons[5][0].questionText[2] = "";
  questionButtons[5][0].questionText[3] = "";
  questionButtons[5][0].questionText[4] = "";
  questionButtons[5][0].questionText[5] = "(A) (B) (C) (D)";
  questionButtons[5][0].answer = A;
  
  //400
  questionButtons[5][1].questionText[0] = "";
  questionButtons[5][1].questionText[1] = "";
  questionButtons[5][1].questionText[2] = "";
  questionButtons[5][1].questionText[3] = "";
  questionButtons[5][1].questionText[4] = "";
  questionButtons[5][1].questionText[5] = "(A) (B) (C) (D)";
  questionButtons[5][1].answer = A;
  
  //600
  questionButtons[5][2].questionText[0] = "";
  questionButtons[5][2].questionText[1] = "";
  questionButtons[5][2].questionText[2] = "";
  questionButtons[5][2].questionText[3] = "";
  questionButtons[5][2].questionText[4] = "";
  questionButtons[5][2].questionText[5] = "(A) (B) (C) (D)";
  questionButtons[5][2].answer = A;
  
  //800
  questionButtons[5][3].questionText[0] = "";
  questionButtons[5][3].questionText[1] = "";
  questionButtons[5][3].questionText[2] = "";
  questionButtons[5][3].questionText[3] = "";
  questionButtons[5][3].questionText[4] = "";
  questionButtons[5][3].questionText[5] = "(A) (B) (C) (D)";
  questionButtons[5][3].answer = A;
  
  //1000
  questionButtons[5][4].questionText[0] = "";
  questionButtons[5][4].questionText[1] = "";
  questionButtons[5][4].questionText[2] = "";
  questionButtons[5][4].questionText[3] = "";
  questionButtons[5][4].questionText[4] = "";
  questionButtons[5][4].questionText[5] = "(A) (B) (C) (D)";
  questionButtons[5][4].answer = A;
}

//Main game loop
void game(){
  setup(true);
  while( !key[KEY_ESC]){
    //Splash
    if(gameScreen == SPLASH){
      highcolor_fade_in(intro,16);
      rest(2000);
      highcolor_fade_out(16);
      FSOUND_Stream_Play(1,titleTheme);
      gameScreen = MENU;
      highcolor_fade_in(title,16);
    }
    //Menu
    if(gameScreen == MENU){
      draw_sprite(buffer,title,0,0);
      if(!hovering){
        draw_sprite(buffer,play,420,600);
      }
      if(hovering){
        draw_sprite(buffer,playHover,420,600);
      }
      if(collision(mouse_x,mouse_x,427,609,mouse_y,mouse_y,610,689)){
        hovering=true;
        if(mouse_b & 1){
          highcolor_fade_out(16);
          gameScreen = GAME;
          highcolor_fade_in(inGame,16);
        }
      }
      else{
        hovering=false;
      }
    }
    //Game
    if(gameScreen == GAME){
      draw_sprite(buffer,inGame,0,0);
      for(int i = 0; i < 6; i++){
        for(int t = 0; t < 5; t++){
          if(questionButtons[i][t].answered == false){
            if(collision(mouse_x,mouse_x,questionButtons[i][t].x,questionButtons[i][t].x + 105,mouse_y,mouse_y,questionButtons[i][t].y,questionButtons[i][t].y+89)){
              draw_sprite(buffer, questionButtons[i][t].image[0], questionButtons[i][t].x, questionButtons[i][t].y);
              if(mouse_b & 1){
                highcolor_fade_out(16);
                gameScreen = QUESTION;
                buttonID[0] = i;
                buttonID[1] = t;
                rectfill(buffer, 0, 0, 1024, 768, makecol(0,0,255));
                highcolor_fade_in(buffer,16);
              }
            }
          }
          else{
            draw_sprite(buffer, questionButtons[i][t].image[1], questionButtons[i][t].x, questionButtons[i][t].y);
            if(collision(mouse_x,mouse_x,questionButtons[i][t].x,questionButtons[i][t].x + 105,mouse_y,mouse_y,questionButtons[i][t].y,questionButtons[i][t].y+89)){
              if(mouse_b & 2){
                questionButtons[i][t].answered = false;
              }
            }
          }
        }
      }
    }
    //Question screen
    if(gameScreen == QUESTION){
      //Background
      rectfill(buffer, 0, 0, 1024, 768, makecol(0,0,255));
      int i = buttonID[0];
      int t = buttonID[1];
      bool win = false;
      
      //Draws text for question
      for (int num = 0, y = 100; num <6; num++, y += text_height(font)){
        if(questionButtons[i][t].questionText[num].c_str() != NULL){
          textout_centre_ex(buffer, font, questionButtons[i][t].questionText[num].c_str(), 512, y, makecol(255,255,255), -1);
        }
      }
      //Buttons
      for(int q = 0; q < 4; q++){
        draw_sprite(buffer, multipleButtons[q].image,  multipleButtons[q].x, multipleButtons[q].y);
      }
      //Choose A
      if(key[KEY_A] || mouse_b & 1 &&  collision(multipleButtons[0].x, multipleButtons[0].x + 100, mouse_x, mouse_x, multipleButtons[0].y, multipleButtons[0].y + 100, mouse_y, mouse_y)){
        if(questionButtons[i][t].answer == A){
          win = true;
        }
        else{
          play_sample(wrong,125,125,1000,0);
        }
      }
      //Choose B
      if(key[KEY_B] || mouse_b & 1 && collision(multipleButtons[1].x, multipleButtons[1].x + 100, mouse_x, mouse_x, multipleButtons[1].y, multipleButtons[1].y + 100, mouse_y, mouse_y)){
        if(questionButtons[i][t].answer == B){
          win = true;
        }
        else{
          play_sample(wrong,125,125,1000,0);
        }
      }
      //Choose C
      if(key[KEY_C] || mouse_b & 1 &&  collision(multipleButtons[2].x, multipleButtons[2].x + 100, mouse_x, mouse_x, multipleButtons[2].y, multipleButtons[2].y + 100, mouse_y, mouse_y)){
        if(questionButtons[i][t].answer == C){
          win = true;
        }
        else{
          play_sample(wrong,125,125,1000,0);
        }
      }
      //Choose D
      if(key[KEY_D] || mouse_b & 1 &&  collision(multipleButtons[3].x, multipleButtons[3].x + 100, mouse_x, mouse_x, multipleButtons[3].y, multipleButtons[3].y + 100, mouse_y, mouse_y)){
        if(questionButtons[i][t].answer == D){
          win = true;
        }
        else{
          play_sample(wrong,125,125,1000,0);
        }
      }
      //Back
      if(key[KEY_SPACE]){
        gameScreen = GAME; 
      }
      //win
      if(win){
        play_sample(correct,125,125,1000,0);
        highcolor_fade_out(16);
        draw_sprite(buffer,inGame,0,0);
        gameScreen = GAME;
        highcolor_fade_in(inGame,16);
        questionButtons[i][t].answered = true;
      }
    }
    draw_sprite(buffer,cursor,mouse_x,mouse_y);
    draw_sprite(screen,buffer,0,0);
    rest(10);
  }
}

//Main loop
int main(){
  FSOUND_Init(44100, 32, 0);
  allegro_init();
  install_mouse();
  install_keyboard();
  set_color_depth(32);
  install_sound(DIGI_AUTODETECT,MIDI_AUTODETECT,".");
  set_gfx_mode( GFX_AUTODETECT, 1024, 768, 0, 0);
  set_window_title("Jeopardy - History");
  
  game();
  
  return 0;
}
END_OF_MAIN();
