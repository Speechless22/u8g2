
#include "u8g2.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mui.h"
#include "mui_u8g2.h"



uint8_t mui_style_helv_r_08(mui_t *ui, uint8_t msg)
{
  
  u8g2_t *u8g2 = mui_get_U8g2(ui);
  switch(msg)
  {
    case MUIF_MSG_DRAW:
      u8g2_SetFont(u8g2, u8g2_font_helvR08_tr);
    
      break;
  }
  return 0;
}

uint8_t mui_style_helv_b_08(mui_t *ui, uint8_t msg)
{
  u8g2_t *u8g2 = mui_get_U8g2(ui);
  switch(msg)
  {
    case MUIF_MSG_DRAW:
      u8g2_SetFont(u8g2, u8g2_font_helvB08_tr);
      break;
  }
  return 0;
}

uint8_t mui_style_monospaced(mui_t *ui, uint8_t msg)
{
  u8g2_t *u8g2 = mui_get_U8g2(ui);
  switch(msg)
  {
    case MUIF_MSG_DRAW:
      u8g2_SetFont(u8g2, u8g2_font_profont12_tr);
      //u8g2_SetFont(u8g2, u8g2_font_6x10_tr);
      break;
  }
  return 0;
}

uint8_t mui_hrule(mui_t *ui, uint8_t msg)
{
  u8g2_t *u8g2 = mui_get_U8g2(ui);
  switch(msg)
  {
    case MUIF_MSG_DRAW:
      u8g2_DrawHLine(u8g2, 0, mui_get_y(ui), u8g2_GetDisplayWidth(u8g2));
      break;
  }
  return 0;
}

u8g2_t u8g2;
mui_t ui;

/*
  global variables which form the communication gateway between the user interface and the rest of the code
*/
uint8_t number_input = 2;       // variable where the user can input a number between 0 and 9
uint8_t number_input2 = 100;       // variable where the user can input a number between 0 and 9
uint8_t fruit_input = 2;
uint8_t fruit_input2 = 2;
uint8_t my_value3 = 0;
uint8_t color_input = 0;
uint8_t checkbox_input = 0;
uint8_t direction_input = 0;
uint8_t text_input[4] = { ' ',' ',' ',' '} ;
uint8_t exit_code = 0;


/* 

  User interface fields list. Each entry is defined with the MUIF macro MUIF(id,cflags,data,cb)
  Arguments are:
    id: 
      A string with exactly two characters. This is the unique "id" of the field, which is later used in the form definition string (fds)
      There are some special id's: ".L" for text labels and ".G" for a goto form command. 
    cflags: 
      Flags, which define static (constant) properties of the field. Currently this is either 0 or MUIF_CFLAG_IS_CURSOR_SELECTABLE which marks the field as editable.
    data: 
      A pointer to a local variable, where the result of an editiable field is stored. Currently this is a pointer to uint8_t in most cases.
      It depends in the callback function (cb) whether this is used or what kind of data is stored
    cb:
      A callback function.
      The callback function will receive messages and have to react accordingly to the message. Some predefined callback functions are avilable in mui_u8g2.c    
  
*/
#define COMMA ,
muif_t muif_list[] MUI_PROGMEM = {
  /* normal text style */
  MUIF_STYLE(0, mui_style_helv_r_08),
  
  /* bold text style */
  MUIF_STYLE(1, mui_style_helv_b_08),

  /* monospaced font */
  MUIF_STYLE(2, mui_style_monospaced),
  
  /* horizontal line (hrule) */
  MUIF("HR",0,0,mui_hrule),

  /* Goto Form Button where the width is equal to the size of the text, spaces can be used to extend the size */
  MUIF("G1",MUIF_CFLAG_IS_CURSOR_SELECTABLE,0,mui_u8g2_btn_jmp_wm_fi),
  
  /* input for a number between 0 to 9 */
  //MUIF("IN",MUIF_CFLAG_IS_CURSOR_SELECTABLE,&number_input,mui_u8g2_u8_value_0_9_wm_mse_pi),
  //MUIF("IN",MUIF_CFLAG_IS_CURSOR_SELECTABLE, (void *)((mui_u8g2_u8_min_max_t   []  ) {{ &number_input COMMA 1 COMMA 8 }  }  ) , mui_u8g2_u8_min_max_wm_mse_pi),
  MUIF_U8G2_U8_MIN_MAX_WM_MSE_PI("IN", &number_input, 2, 7),

  /* input for a number between 0 to 100 */
  //MUIF("IH",MUIF_CFLAG_IS_CURSOR_SELECTABLE,&number_input2,mui_u8g2_u8_value_0_100_wm_mud_pi),
  MUIF_U8G2_U8_MIN_MAX_WM_MUD_PI("IH", &number_input2, 1, 101),
  
  /* input for text with four chars  */
  MUIF("T0",MUIF_CFLAG_IS_CURSOR_SELECTABLE,text_input+0,mui_u8g2_u8_char_wm_mud_pi),
  MUIF("T1",MUIF_CFLAG_IS_CURSOR_SELECTABLE,text_input+1,mui_u8g2_u8_char_wm_mud_pi),
  MUIF("T2",MUIF_CFLAG_IS_CURSOR_SELECTABLE,text_input+2,mui_u8g2_u8_char_wm_mud_pi),
  MUIF("T3",MUIF_CFLAG_IS_CURSOR_SELECTABLE,text_input+3,mui_u8g2_u8_char_wm_mud_pi),
  
  /* input for a fruit (0..3), implements a selection, where the user can cycle through the options  */
  MUIF("IF",MUIF_CFLAG_IS_CURSOR_SELECTABLE,&fruit_input,mui_u8g2_u8_opt_line_wa_mse_pi),
  MUIF("IG",MUIF_CFLAG_IS_CURSOR_SELECTABLE,&fruit_input2,mui_u8g2_u8_opt_line_wa_mud_pi),
  
  /* radio style selection */
  MUIF("CB",MUIF_CFLAG_IS_CURSOR_SELECTABLE,&checkbox_input,mui_u8g2_u8_chkbox_wm_pi),
  
  /* the following two fields belong together and implement a single selection combo box to select a color */
  MUIF("IC",MUIF_CFLAG_IS_CURSOR_SELECTABLE,&color_input,mui_u8g2_u8_opt_parent_wa_mse_pi),
  MUIF("OC",MUIF_CFLAG_IS_CURSOR_SELECTABLE,&color_input,mui_u8g2_u8_opt_child_w1_mse_pi),
    
  /* radio button style */
  MUIF("RS",MUIF_CFLAG_IS_CURSOR_SELECTABLE,&direction_input,mui_u8g2_u8_radio_wm_pi),

  /* MUI_GOTO uses the fixed ".G" id and is intended for goto buttons. This is a full display width style button */
  MUIF(".G",MUIF_CFLAG_IS_CURSOR_SELECTABLE,0,mui_u8g2_btn_jmp_w1_pi),
  
  /* MUI_LABEL uses the fixed ".L" id and is used to place read only text on a form */
  MUIF(".L",0,0,mui_u8g2_draw_text),

  /* button for the minimal example */
  MUIF("BN", MUIF_CFLAG_IS_CURSOR_SELECTABLE, 0, mui_u8g2_btn_exit_wm_fi),

  /* Leave the menu system */
  MUIF("LV",MUIF_CFLAG_IS_CURSOR_SELECTABLE,&exit_code,mui_u8g2_btn_exit_wm_fi)

};

/*
  The form definition string (fds) defines all forms and the fields on those forms.
  A new form always starts with MUI_FORM(u). The form ends with the next MUI_FORM() or the end of the fds.
  Inside the form use fields or the style command (MUI_STYLE)
  The fields are define with
    MUI_XY(id, x, y)                     Field 'id' without extra argument or text placed at the specified xy position
    MUI_XYT(id, x, y, text)            Field 'id' with the specified test at position xy
    MUI_XYA(id, x, y, a)                 Field 'id' with argument 'a' at position xy
    MUI_XYAT(id, x, y, a, text)         Field 'id' with argument and text placed at position xy
    MUI_LABEL(x,y,text)                 Field '.L' (usually some readonly text) placed at position xy
    MUI_GOTO(x,y,n,text)                Field '.G', usually a button placed at xy, which activates form n 

  Note:
    MUI_LABEL(x,y,text) is the same as MUI_XYT(".L", x, y, text)
    MUI_GOTO(x,y,text) is the same as MUI_XYAT(".G", x, y, n, text)
    
*/


fds_t fds[] = 

/* top level main menu */
MUI_FORM(0)
MUI_STYLE(1)
MUI_LABEL(5,10, "Main Menu 1/2")
MUI_XY("HR", 0,13)
MUI_STYLE(0)
MUI_GOTO(5,25,10, "Enter a number")
MUI_GOTO(5,37,11, "Parent/Child Selection")
MUI_GOTO(5,49,13, "Checkbox")
MUI_GOTO(5,61,1, "More...")

MUI_FORM(1)
MUI_STYLE(1)
MUI_LABEL(5,10, "Main Menu 2/2")
MUI_XY("HR", 0,13)
MUI_STYLE(0)
MUI_GOTO(5,25,14, "Radio Selection")
MUI_GOTO(5,37,15, "Text Input")
MUI_GOTO(5,49,16, "Single Line Selection")
MUI_GOTO(5,61,0, "Back...")

/* number entry demo */
MUI_FORM(10)
MUI_STYLE(1)
MUI_LABEL(5,10, "Number Menu")
MUI_XY("HR", 0,13)
MUI_STYLE(0)

MUI_LABEL(5,27, "Number [mse]:")
MUI_XY("IN",76, 27)
MUI_LABEL(5,41, "Number [mud]:")
MUI_XY("IH",76, 41)

MUI_XYAT("G1",64, 59, 0, " OK ")

/* parent / child selection */
MUI_FORM(11)
MUI_STYLE(1)
MUI_LABEL(5,10, "Parent/Child Selection")
MUI_XY("HR", 0,13)
MUI_STYLE(0)

//MUI_LABEL(5,29, "Fruit:")
//MUI_XYAT("IF",50, 29, 60, "Banana|Apple|Melon|Cranberry")

MUI_LABEL(5,29, "Color:")
//MUI_XYAT("IC",50, 29, 12, "red|green|blue")     /* jump to sub form 12 */
MUI_XYAT("IC",50, 29, 12, "red|orange|yellow|green|cyan|azure|blue|violet|magenta|rose")     /* jump to sub form 12 */
/* red|orange|yellow|green|cyan|azure|blue|violet|magenta|rose */

MUI_XYAT("G1",64, 59, 0, " OK ")

/* combo box color selection */
MUI_FORM(12)
MUI_STYLE(1)
MUI_LABEL(5,10, "Color Selection")
MUI_XY("HR", 0,13)
MUI_STYLE(0)
MUI_XYA("OC", 5, 30, 0) /* takeover the selection text from calling field ("red") */
MUI_XYA("OC", 5, 42, 1) /* takeover the selection text from calling field ("green") */
MUI_XYA("OC", 5, 54, 2)  /* just as a demo: provide a different text for this option */


/* Checkbox demo */
MUI_FORM(13)
MUI_STYLE(1)
MUI_LABEL(5,10, "Checkbox Menu")
MUI_XY("HR", 0,13)
MUI_STYLE(0)

MUI_LABEL(5,30, "Checkbox:")
MUI_XY("CB",60, 30)

MUI_XYAT("G1",64, 59, 0, " OK ")

/* Radio selection demo */
MUI_FORM(14)
MUI_STYLE(1)
MUI_LABEL(5,10, "Radio Selection Menu")
MUI_XY("HR", 0,13)
MUI_STYLE(0)

MUI_XYAT("RS",10, 28,0,"North")
MUI_XYAT("RS",10, 40,1,"South")

MUI_XYAT("RS",65, 28,2,"East")
MUI_XYAT("RS",65, 40,3,"West")

MUI_XYAT("G1",64, 59, 1, " OK ")

/* text demo */
MUI_FORM(15)
MUI_STYLE(1)
MUI_LABEL(5,10, "Enter Text Menu")
MUI_XY("HR", 0,13)
MUI_STYLE(0)

MUI_LABEL(5,30, "Text:")
MUI_STYLE(2)
MUI_XY("T0",40, 30)
MUI_XY("T1",48, 30)
MUI_XY("T2",56, 30)
MUI_XY("T3",64, 30)
MUI_STYLE(0)

MUI_XYAT("G1",64, 59, 1, " OK ")



/* single line selection */
MUI_FORM(16)
MUI_STYLE(1)
MUI_LABEL(5,10, "Single Line Selection")
MUI_XY("HR", 0,13)
MUI_STYLE(0)

MUI_LABEL(5,29, "Fruit [mse]:")
MUI_XYAT("IF",60, 29, 60, "Banana|Apple|Melon|Cranberry")

MUI_LABEL(5,43, "Fruit [mud]:")
MUI_XYAT("IG",60, 43, 60, "Banana|Apple|Melon|Cranberry")

MUI_XYAT("G1",64, 59, 1, " OK ")





/* minimal example */
MUI_FORM(200)
MUI_STYLE(0)
MUI_XYT("BN",64, 30, " Select Me ")

MUI_FORM(201)
MUI_STYLE(0)
MUI_LABEL(5,12, "Countdown Time")
MUI_LABEL(5,30, "Seconds:")
MUI_XY("IN",60, 30)
MUI_XYT("LV",64, 59, " OK ")

;

int screenshot_n = 0;

void do_screenshot(void)
{
  char s[4096];
  u8x8_SaveBitmapTGA(u8g2_GetU8x8(&u8g2),  "screenshot.tga");
  sprintf(  s, 
  "convert -border 4 -bordercolor 'rgb(255,190,40)'"
  " -fill 'rgb(255,170,0)' -opaque white"
  "  -filter point -resize 200%%" 
  " screenshot.tga pic%04d.png", screenshot_n);
  system(s);
  screenshot_n++;
  /*
  gif animation:
  convert -delay 40 -loop 0 pic*.png animation.gif
  */
}

int main(void)
{
  int x, y;
  int k;

  
  u8g2_SetupBuffer_SDL_128x64_4(&u8g2, &u8g2_cb_r0);
  u8x8_InitDisplay(u8g2_GetU8x8(&u8g2));
  u8x8_SetPowerSave(u8g2_GetU8x8(&u8g2), 0);  
  
  u8x8_ConnectBitmapToU8x8(u8g2_GetU8x8(&u8g2));		/* connect to bitmap */
  
  mui_Init(&ui, &u8g2, fds, muif_list, sizeof(muif_list)/sizeof(muif_t));
  //mui_GotoForm(&ui, 201, 0);
  mui_GotoForm(&ui, 0, 0);

  //puts(fds);
  
  x = 4;  // use as height for the box
  y = 0;
  
  for(;;)
  {
    u8g2_SetFontRefHeightExtendedText(&u8g2);
    u8g2_FirstPage(&u8g2);
    do
    {
      mui_Draw(&ui);
    } while( u8g2_NextPage(&u8g2) );
    do_screenshot();
    
    // printf("mui_GetCurrentCursorFocusPosition=%d\n", mui_GetCurrentCursorFocusPosition(&ui));
    
    do
    {
      k = u8g_sdl_get_key();
    } while( k < 0 );
    
    if ( k == 273 ) y -= 1;
    if ( k == 274 ) y += 1;
    if ( k == 276 ) x -= 1;
    if ( k == 275 ) x += 1;
    
    /*
    if ( k == 'e' ) y -= 1;
    if ( k == 'x' ) y += 1;
    if ( k == 's' ) x -= 1;
    if ( k == 'd' ) x += 1;
    */
    
    if ( k == 'q' ) break;

    if ( k == 'n' ) 
    {
      mui_NextField(&ui);
    }
    if ( k == 'p' ) 
    {
      mui_PrevField(&ui);
    }
    if ( k == 's' )
    {
      mui_SendSelect(&ui);
    }

    if ( k == 't' ) 
    {
      puts("screenshot");
      do_screenshot();
    }
    
    if ( x < 0 )
      x = 0;
    
  }
  return 0;
}
