#include "src/ftdi_eve_lib/ftdi_eve_lib.h"

using namespace FTDI;

class MainScreen : public UIScreen, public UncachedScreen {
  public:
    static void onRedraw(draw_mode_t);
    static void onIdle();
};

void MainScreen::onRedraw(draw_mode_t what) {
  CommandProcessor cmd;
  cmd.cmd(CLEAR_COLOR_RGB(0x555555));
  cmd.cmd(CLEAR(true,true,true));
}

void MainScreen::onIdle() {
}

SCREEN_TABLE {
  DECL_SCREEN(MainScreen)
};
SCREEN_TABLE_POST


void setup() {
  CLCD::turn_on_backlight();
  SoundPlayer::set_volume(255);
  EventLoop::setup();
}

// the loop routine runs over and over again forever:
void loop() {
  EventLoop::loop();
}
