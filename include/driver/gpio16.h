#ifndef __GPIO16_H__
#define __GPIO16_H__

void gpio16_output_conf(void);
void gpio16_output_set(int value);
void gpio16_input_conf(void);
int gpio16_input_get(void);

#endif
