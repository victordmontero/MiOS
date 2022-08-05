#ifndef LOW_LEVEL_H_
#define LOW_LEVEL_H_

unsigned char port_byte_in(unsigned short port);
void port_byte_out(unsigned short port, unsigned char data);
unsigned short port_word_in(short port);
void port_word_out(unsigned short port, unsigned short data);

#endif