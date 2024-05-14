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
#include "spi.h"

#if defined(BACKLIGHT_CONTROL_FROM_GPIO_KEY) && defined(TURN_DISPLAY_OFF_AFTER_USECS_OF_INACTIVITY)
#define READ_GPIO_KEY_ENABLED
#endif

const int _gpio_list[] = GPIO_KEY_LISTS;
const int _gpio_list_size = sizeof(_gpio_list) / sizeof(_gpio_list[0]);

void OpenGpioKey()
{
#ifdef READ_GPIO_KEY_ENABLED
    for (int i = 0; i < _gpio_list_size; ++i) {
      SET_GPIO_MODE(_gpio_list[i], 0);
    }
#endif
}

int ReadGpio()
{
#ifdef READ_GPIO_KEY_ENABLED
  int numRead = 0;
  
  for (int i = 0; i < _gpio_list_size; ++i) {
      if (!GET_GPIO(_gpio_list[i]))
        numRead++;
  }
  printf("%d\n", numRead);
  return numRead;
#else
  return 0;
#endif
}

void CloseGpioKey()
{
#ifdef READ_GPIO_KEY_ENABLED

#endif
}

static uint64_t lastGpioKeyPressTime = 0;
static uint64_t lastGpioKeyPressCheckTime = 0;

uint64_t TimeSinceLastGpioKeyPress(void)
{
#ifdef READ_GPIO_KEY_ENABLED
  uint64_t now = tick();
  if (now - lastGpioKeyPressCheckTime >= 20000) // ReadGpio() takes about 8 usecs on Pi 3B, so 250msecs poll interval should be fine
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
