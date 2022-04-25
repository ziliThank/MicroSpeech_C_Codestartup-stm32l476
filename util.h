#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

void USART2_Init(void);

void USART2_write_int(int ch);

void USART2_write(const char *data);

#ifdef __cplusplus
}
#endif

#endif
