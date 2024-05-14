#pragma once

void OpenGpioKey(void);
int ReadGpio(void);
void CloseGpioKey(void);
uint64_t TimeSinceLastGpioKeyPress(void);
