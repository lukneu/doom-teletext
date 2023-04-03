//doomgeneric for cross-platform development library 'Simple DirectMedia Layer'

#include "doomkeys.h"
#include "m_argv.h"
#include "doomdef.h"
#include "doomgeneric.h"

#include "tt_socket.h"
#include "tt_pagegenerator.h"

#include <stdio.h>
#include <unistd.h>

#include <stdbool.h>
#include <SDL.h>

#define FPS_START 6
#define FPS_MAX 35
#define FPS_MIN 1
#define DISPLAY_SECONDS 4

enum GraphicMode {MOSAIC_SEPARATED,
                  MOSAIC_CONTIGUOUS,
                  NumberOfGraphicModes};

enum GraphicMode graphicMode = 0;

uint8_t tt_page[TT_ROWS][TT_COLUMNS]; //holds whole teletext page (incl mpag bytes)
uint8_t tt_statusbar[TT_STATUSBAR_ROWS][TT_STATUSBAR_COLUMNS]; //holds statusbar
uint8_t tt_rendering[TT_FRAMEBUFFER_ROWS][TT_FRAMEBUFFER_COLUMNS]; //holds part of tt page that displays the framebuffer

uint current_frame = 0;
uint8_t fps = FPS_START;
uint8_t frames_display_config = 0;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture;

#define KEYQUEUE_SIZE 16

static unsigned short s_KeyQueue[KEYQUEUE_SIZE];
static unsigned int s_KeyQueueWriteIndex = 0;
static unsigned int s_KeyQueueReadIndex = 0;

static unsigned char convertToDoomKey(unsigned int key){
  switch (key)
    {
    case SDLK_RETURN:
      key = KEY_ENTER;
      break;
    case SDLK_ESCAPE:
      key = KEY_ESCAPE;
      break;
    case SDLK_LEFT:
      key = KEY_LEFTARROW;
      break;
    case SDLK_RIGHT:
      key = KEY_RIGHTARROW;
      break;
    case SDLK_UP:
      key = KEY_UPARROW;
      break;
    case SDLK_DOWN:
      key = KEY_DOWNARROW;
      break;
    case SDLK_LCTRL:
    case SDLK_RCTRL:
      key = KEY_FIRE;
      break;
    case SDLK_SPACE:
      key = KEY_USE;
      break;
    case SDLK_LSHIFT:
    case SDLK_RSHIFT:
      key = KEY_RSHIFT;
      break;
    case SDLK_LALT:
    case SDLK_RALT:
      key = KEY_LALT;
      break;
    case SDLK_F2:
      key = KEY_F2;
      break;
    case SDLK_F3:
      key = KEY_F3;
      break;
    case SDLK_F4:
      key = KEY_F4;
      break;
    case SDLK_F5:
      key = KEY_F5;
      break;
    case SDLK_F6:
      key = KEY_F6;
      break;
    case SDLK_F7:
      key = KEY_F7;
      break;
    case SDLK_F8:
      key = KEY_F8;
      break;
    case SDLK_F9:
      key = KEY_F9;
      break;
    case SDLK_F10:
      key = KEY_F10;
      break;
    case SDLK_F11:
      key = KEY_F11;
      break;
    case SDLK_EQUALS:
    case SDLK_PLUS:
      key = KEY_EQUALS;
      break;
    case SDLK_MINUS:
      key = KEY_MINUS;
      break;
    default:
      key = tolower(key);
      break;
    }

  return key;
}

static void addKeyToQueue(int pressed, unsigned int keyCode){
  unsigned char key = convertToDoomKey(keyCode);

  unsigned short keyData = (pressed << 8) | key;

  s_KeyQueue[s_KeyQueueWriteIndex] = keyData;
  s_KeyQueueWriteIndex++;
  s_KeyQueueWriteIndex %= KEYQUEUE_SIZE;
}

static void handleKeyInput(){
  SDL_Event e;
  while (SDL_PollEvent(&e)){
    if (e.type == SDL_QUIT){
      puts("Quit requested");

      //close TCP socket
      TCPSocketClose();

      atexit(SDL_Quit);
      exit(1);
    }

    if (e.type == SDL_KEYDOWN &&
                  ( e.key.keysym.sym == SDLK_g ||
                    e.key.keysym.sym == SDLK_o ||
                    e.key.keysym.sym == SDLK_l ))
    {
      switch (e.key.keysym.sym)
      {
        case SDLK_g:
          graphicMode++;
          if (graphicMode == NumberOfGraphicModes)
          {
            graphicMode = 0;
          }
          break;
        case SDLK_o:
          if(fps < FPS_MAX)
          {
            fps++;
          }
          break;
        case SDLK_l:
          if(fps > FPS_MIN)
          { 
            fps--;
          }
          break;
        default:
          break;
      }

      frames_display_config = fps * DISPLAY_SECONDS;
      TT_ShowDebugInfo(tt_page, fps, graphicMode);
      return;
    }

    if (e.type == SDL_KEYDOWN) {
      //KeySym sym = XKeycodeToKeysym(s_Display, e.xkey.keycode, 0);
      //printf("KeyPress:%d sym:%d\n", e.xkey.keycode, sym);
      addKeyToQueue(1, e.key.keysym.sym);
    } else if (e.type == SDL_KEYUP) {
      //KeySym sym = XKeycodeToKeysym(s_Display, e.xkey.keycode, 0);
      //printf("KeyRelease:%d sym:%d\n", e.xkey.keycode, sym);
      addKeyToQueue(0, e.key.keysym.sym);
    }
  }
}

void DG_Init(){
  window = SDL_CreateWindow("DOOM",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            DOOMGENERIC_RESX,
                            DOOMGENERIC_RESY,
                            SDL_WINDOW_SHOWN
                            );

  // Setup renderer
  renderer =  SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED);
  // Clear winow
  SDL_RenderClear( renderer );
  // Render the rect to the screen
  SDL_RenderPresent(renderer);

  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, DOOMGENERIC_RESX, DOOMGENERIC_RESY);

  //default tcp parameters
  char* tcp_server_ip = "127.0.0.1";
  int tcp_server_port = 8080;

  int tt_ip_param = M_CheckParmWithArgs("-tt_stream_ip", 1);
  if (tt_ip_param > 0)
  {
    tcp_server_ip = myargv[tt_ip_param + 1];
  }
  else
  {
    printf("Argument '-tt_stream_ip' was not provided. Using %s\n", tcp_server_ip);
  }

  int tt_port_param = M_CheckParmWithArgs("-tt_stream_port", 1);
  if (tt_port_param > 0)
  {
    tcp_server_port = atoi(myargv[tt_port_param + 1]);
  }
  else
  {
    printf("Argument '-tt_stream_port' was not provided. Using %d\n", tcp_server_port);
  }
  
  //create tcp client
  TCPSocketCreate(tcp_server_ip, tcp_server_port);

  //init tt page
  TT_InitPage(tt_page);
  TT_InitStatusbar(tt_statusbar);
}

void DG_DrawFrame()
{
  current_frame++;
  
  switch (frames_display_config)
  {
    case 0:
      break;
    case 1:
      TT_HideDebugInfo(tt_page);
      frames_display_config--;
      break;
    default:
      frames_display_config--;
      break;
  }

  SDL_UpdateTexture(texture, NULL, DG_ScreenBuffer, DOOMGENERIC_RESX*sizeof(uint32_t));

  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);

  handleKeyInput();

  if (DG_Player != NULL)
  {
    TT_SetHealth(tt_statusbar, DG_Player->health, DG_PlayerInRampageMode);
    TT_SetArmor(tt_statusbar, DG_Player->armorpoints);
    TT_SetAvailableWeapons(tt_statusbar, DG_Player->weaponowned[1], DG_Player->weaponowned[2], DG_Player->weaponowned[3], DG_Player->weaponowned[4], DG_Player->weaponowned[5], DG_Player->weaponowned[6]);
      
    TT_SetAmmunitionValues(tt_statusbar, DG_Player->ammo[0], DG_Player->maxammo[0], DG_Player->ammo[1], DG_Player->maxammo[1], DG_Player->ammo[3], DG_Player->maxammo[3], DG_Player->ammo[2], DG_Player->maxammo[2]);

    TT_SetCards(tt_statusbar, DG_Player->cards[0], DG_Player->cards[1], DG_Player->cards[2]);
    //TODO: what about DG_Player->cards[3 / 4 / 5] ? =>  it_blueskull, it_yellowskull, it_redskull,

    switch (DG_Player->readyweapon)
    {
      case wp_pistol:
      case wp_chaingun:
          TT_SetActiveAmmunition(tt_statusbar, DG_Player->ammo[0]);
          break;
      case wp_shotgun:
      case wp_supershotgun:
          TT_SetActiveAmmunition(tt_statusbar, DG_Player->ammo[1]);
          break;
      case wp_missile:
          TT_SetActiveAmmunition(tt_statusbar, DG_Player->ammo[3]);
          break;
      case wp_plasma:
      case wp_bfg:
          TT_SetActiveAmmunition(tt_statusbar, DG_Player->ammo[2]);
          break;
      default:
          TT_SetActiveAmmunitionToInfinite(tt_statusbar);
          break;
    }
  }
  
  TT_InsertStatusbar(tt_page, tt_statusbar);

  switch (graphicMode)
  {
    case MOSAIC_SEPARATED:
      TT_RenderInMosaicBlackWhite(DG_ScreenBuffer, tt_rendering, true);
      break;
    case MOSAIC_CONTIGUOUS:
      TT_RenderInMosaicBlackWhite(DG_ScreenBuffer, tt_rendering, false);
      break;
    default:
        puts("Invalid graphic mode");
        exit(-1);
  }

  if(DG_HintMessageActive)
  {
    TT_WriteHintMessage(tt_page, 2, DG_HintMessage);
  }
  else
  {
    TT_ClearLine(tt_page, 2);
  }

  if(DG_MenuMessageActive)
  {
    TT_InsertMenuMessage(tt_rendering, DG_MenuMessageString);
  }
  else if(DG_MenuActive)
  {
printf("DG_CurrentMenu: %d\n", DG_CurrentMenu);

    switch (DG_CurrentMenu)
    {
      case DOOM_MENU_READDEF1:
        printf("ReadDef1 Menu\n");
        break;
      case DOOM_MENU_READDEF2:
        printf("ReadDef2 Menu\n");
        break;
      case DOOM_MENU_SAVEDDEF:
        printf("Save Menu\n");
        break;
      case DOOM_MENU_LOADDEF:
        printf("Load Menu\n");
        break;
      case DOOM_MENU_MAINDEF:
      case DOOM_MENU_EPIDEF:
      case DOOM_MENU_NEWDEF:
      case DOOM_MENU_OPTIONSDEF:
      case DOOM_MENU_SOUNDDEF:
        TT_OverlayMenu(tt_rendering, DG_MenuItemsCount, DG_MenuEntriesNames, DG_MenuItemOn, DG_MenuEntriesStati);
        break;
      default:
        break;
    }
    
    /*
    printf("active menu: %s\n", "asd");
    printf(" %d entries\n", DG_MenuItemsCount);

    for(int i = 0; i < DG_MenuItemsCount; i++)
    {
      printf(" * %s : %d\t[%d]\n", DG_MenuEntriesNames[i], DG_MenuEntriesStati[i, i == DG_MenuItemOn]);
    }
    */
  }

  TT_InsertGameRendering(tt_page, tt_rendering);

  //send tcp packet
  TCPSocketSendTTPage(tt_page);

  usleep(1000000 / fps);
}

void DG_SleepMs(uint32_t ms)
{
  SDL_Delay(ms);
}

uint32_t DG_GetTicksMs()
{
  return SDL_GetTicks();
}

int DG_GetKey(int* pressed, unsigned char* doomKey)
{
  if (s_KeyQueueReadIndex == s_KeyQueueWriteIndex){
    //key queue is empty
    return 0;
  }else{
    unsigned short keyData = s_KeyQueue[s_KeyQueueReadIndex];
    s_KeyQueueReadIndex++;
    s_KeyQueueReadIndex %= KEYQUEUE_SIZE;

    *pressed = keyData >> 8;
    *doomKey = keyData & 0xFF;

    return 1;
  }

  return 0;
}

void DG_SetWindowTitle(const char * title)
{
  if (window != NULL){
    SDL_SetWindowTitle(window, title);
  }
}
