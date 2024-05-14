#include <linux/input.h> // input_event
#include <fcntl.h> // O_RDONLY, O_NONBLOCK
#include <stdio.h> // printf
#include <stdint.h> // uint64_t
#include <pigpio.h>
#include <pigpiod_if2.h>

#include "config.h"
#include "keyboard.h"
#include "util.h"
#include "tick.h"

#if defined(BACKLIGHT_CONTROL_FROM_GPIO_KEY) && defined(TURN_DISPLAY_OFF_AFTER_USECS_OF_INACTIVITY)
#define READ_GPIO_KEY_ENABLED
#endif

int _pi = -1;
const int _gpio_list[] = GPIO_KEY_LISTS;
const int _gpio_list_size = sizeof(_gpio_list) / sizeof(_gpio_list[0]);

void OpenGpioKey()
{
#ifdef READ_GPIO_KEY_ENABLED
  _pi = pigpio_start(PIGPIOD_HOST, PIGPIOD_PORT);
  if (_pi < 0) {
      printf("Failed pigpiod connected\n");
  } else {
    for (int i = 0; i < _gpio_list_size; ++i) {
      set_mode(_pi, _gpio_list[i], PI_INPUT);
      set_pull_up_down(_pi, _gpio_list[i], PI_PUD_UP);
    }
  }
#endif
}

int ReadGpio()
{
#ifdef READ_GPIO_KEY_ENABLED
  if (_pi < 0) return 0;
  int numRead = 0;
  
  for (int i = 0; i < _gpio_list_size; ++i) {
      if (!gpio_read(_pi, _gpio_list[i]))
        numRead++;
  }
  
  return numRead;
#else
  return 0;
#endif
}

void CloseGpioKey()
{
#ifdef READ_GPIO_KEY_ENABLED
  if (_pi >= 0)
  {
    pigpio_stop(_pi);
    _pi = -1;
  }
#endif
}

static uint64_t lastGpioKeyPressTime = 0;
static uint64_t lastGpioKeyPressCheckTime = 0;

uint64_t TimeSinceLastGpioKeyPress(void)
{
#ifdef READ_GPIO_KEY_ENABLED
  uint64_t now = tick();
  if (now - lastGpioKeyPressCheckTime >= 250000) // ReadGpio() takes about 8 usecs on Pi 3B, so 250msecs poll interval should be fine
  {
    lastGpioKeyPressCheckTime = now;
    if (ReadGpio())
      lastGpioKeyPressTime = now;
  }
  return now - lastGpioKeyPressTime;
#else
  return 0;
#endif
}
