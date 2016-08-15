/*

  u8x8_d_sed1330.c
  
  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright (c) 2016, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  


  The device might also work with the RA8835, SED1335 and SED1336 controller.
  
*/
#include "u8x8.h"



static const uint8_t u8x8_d_sed1330_powersave0_seq[] = {
  U8X8_START_TRANSFER(),             	/* enable chip, delay is part of the transfer start */
  U8X8_CA(0x040, 0x030),		/* sys init (0x040) with one arg, where 0x030 is a wild guess */
  U8X8_CA(0x059, 0x054),               /* send display on command (hex 0x059, see p37 ) */
							/* display cmd has one arg: 01010100 should enable all three blocks, but disable the cursor*/
  
  U8X8_END_TRANSFER(),             	/* disable chip */
  U8X8_END()             			/* end of sequence */
};

static const uint8_t u8x8_d_sed1330_powersave1_seq[] = {
  U8X8_START_TRANSFER(),             	/* enable chip, delay is part of the transfer start */
  U8X8_CA(0x058, 0x000),               /* send display off command (hex 0x059, see p37) and turn of all banks */
  /* maybe send a sleep in cmd */
  //U8X8_C(0x053)				/* sleep in: 0x053 */
  U8X8_END_TRANSFER(),             	/* disable chip */
  U8X8_END()             			/* end of sequence */
};


uint8_t u8x8_d_sed1330_common(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  uint8_t c, i;
  uint16_t y;
  uint8_t *ptr;
  switch(msg)
  {
    /* U8X8_MSG_DISPLAY_SETUP_MEMORY is handled by the calling function */
    /*
    case U8X8_MSG_DISPLAY_SETUP_MEMORY:
      break;
    case U8X8_MSG_DISPLAY_INIT:
      break;
    */
    case U8X8_MSG_DISPLAY_SET_POWER_SAVE:
      if ( arg_int == 0 )
	u8x8_cad_SendSequence(u8x8, u8x8_d_sed1330_powersave0_seq);
      else
	u8x8_cad_SendSequence(u8x8, u8x8_d_sed1330_powersave1_seq);
      break;
    case U8X8_MSG_DISPLAY_DRAW_TILE:
      y = (((u8x8_tile_t *)arg_ptr)->y_pos);
      y*=8;
      y*= u8x8->display_info->tile_width;
    
      u8x8_cad_StartTransfer(u8x8);
    
      c = ((u8x8_tile_t *)arg_ptr)->cnt;	/* number of tiles */
      ptr = ((u8x8_tile_t *)arg_ptr)->tile_ptr;	/* data ptr to the tiles */
      for( i = 0; i < 8; i++ )
      {
	u8x8_cad_SendCmd(u8x8, 0x046 );	/* CSRW command*/
	u8x8_cad_SendArg(u8x8, y>>8);		/* CSRW low adr byte */
	u8x8_cad_SendArg(u8x8, y&255);		/* CSRW high adr byte */
	u8x8_cad_SendCmd(u8x8, 0x042 );	/* MWRITE */
	
	u8x8_cad_SendData(u8x8, c, ptr);	/* note: SendData can not handle more than 255 bytes, send one line of data */
	
	ptr += u8x8->display_info->tile_width;
	y += u8x8->display_info->tile_width;
      }

      u8x8_cad_EndTransfer(u8x8);
      //u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO, 200, NULL);	/* extra dely required */

      break;
    default:
      return 0;
  }
  return 1;
}

/*=============================================*/


static const u8x8_display_info_t u8x8_sed1330_240x128_display_info =
{
  /* chip_enable_level = */ 0,
  /* chip_disable_level = */ 1,
  
  /* post_chip_enable_wait_ns = */ 30,	/* G242CX Datasheet p5 */
  /* pre_chip_disable_wait_ns = */ 10,	/* G242CX Datasheet p5 */
  /* reset_pulse_width_ms = */ 1, 
  /* post_reset_wait_ms = */ 6, 
  /* sda_setup_time_ns = */ 20,		
  /* sck_pulse_width_ns = */  140,	
  /* spi_mode = */ 1,		
  /* i2c_bus_clock_100kHz = */ 4,
  /* data_setup_time_ns = */ 120,		/* G242CX Datasheet p5 */
  /* write_pulse_width_ns = */ 220,		/* G242CX Datasheet p5 */
  /* tile_width = */ 30,
  /* tile_hight = */ 16,
  /* default_x_offset = */ 0,
  /* flipmode_x_offset = */ 0,
  /* pixel_width = */ 240,
  /* pixel_height = */ 128
};

/* 240x128 Seiko G242C */
static const uint8_t u8x8_d_sed1330_240x128_init_seq[] = {
  U8X8_DLY(100),
  U8X8_START_TRANSFER(),             	/* enable chip, delay is part of the transfer start */
  U8X8_DLY(100),

  /* system init command, see also u8x8_d_sed1330_powersave0_seq */
  U8X8_CA(0x040, 0x030),		/* sys init (0x040) with one arg, where 0x030 is a wild guess */
  /* system init has total 8 parameters, so 7 more are here */
  U8X8_A(0x087),				/* no idea here... WF (topmost bit) is set to one because it is suggested in the datasheet, lowest 3 bits refer to text mode only */
  U8X8_A(0x007),				/* FY: height of a char+1, does not matter here (hopefully), because we use graphics mode only */
  U8X8_A(240/8-1),				/* C/R: this could be the number of horizontal bytes - 1 (Value confirmed with app notes p41) */
  U8X8_A(74),					/* TC/R: According to app notes fOSC=6Mhz fFF=70Hz --> TC/R = 74d*/
  U8X8_A(127),				/* L/F: Lines per frame - 1, probably this is the height of the display - 1 (value confirmed with app notes p41)*/
  U8X8_A(240/8),				/* Low byte of the virtual screen size. (Value confirmed with app notes p41)   */
  U8X8_A(0),					/* High byte of the virtual screen size, see also section 9.1.2 */
	
  U8X8_C(0x044),				/* SCROLL: Args taken fom app notes p41 */
  U8X8_A(0),					
  U8X8_A(0),					
  U8X8_A(128),					
  U8X8_A(0),					
  U8X8_A(5),					
  U8X8_A(128),
	
  U8X8_CA(0x05a, 0),			/* HDOT SCR: Horizontal dotwise scroll... set to 0 */
	
  U8X8_CA(0x05b, 0x0c),			/* OVLAY: 2-layer, all graphics, OR between layer 1 and 2 */
    
  U8X8_DLY(100),
  U8X8_END_TRANSFER(),             	/* disable chip */
  U8X8_DLY(100),
};

uint8_t u8x8_d_sed1330_240x128(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  switch(msg)
  {
    case U8X8_MSG_DISPLAY_SETUP_MEMORY:
      u8x8_d_helper_display_setup_memory(u8x8, &u8x8_sed1330_240x128_display_info);
      break;
    case U8X8_MSG_DISPLAY_INIT:
      u8x8_d_helper_display_init(u8x8);
      u8x8_cad_SendSequence(u8x8, u8x8_d_sed1330_240x128_init_seq);
      break;
    default:
      return u8x8_d_sed1330_common(u8x8, msg, arg_int, arg_ptr);
  }
  return 1;
}

