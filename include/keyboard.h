#ifndef _KEYBOARD_H
#define _KEYBOARD_H

/**********************************
*
*  keyboard.h: Funciones para manejo
*         de teclado
*
***********************************/

int printable(unsigned char c);
struct key_type * parse_scancode(unsigned char c);

#endif