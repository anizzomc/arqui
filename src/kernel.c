#include "../include/kasm.h"
#include "../include/defs.h"

DESCR_INT idt[0x100]; /* IDT de 10 entradas */
IDTR idtr; /* IDTR */

/* Definir� las entradas del IDT */
void setup_IDT_content();

/* Definir� el IDTR */
void setup_IDTR();

/* Definir� las terminales virtuales */
void setup_vts();

void setup_keyboard_buffer();

/**********************************************
 kmain()
 Punto de entrada de cóo C.
 *************************************************/

kmain() {

	int i, num;

	/* Borra la pantalla. */
	k_clear_screen();

	setup_keyboard_buffer();

	/* Crea la pantalla */
	setup_vts();

	/* Configura el puerto serie */
	setup_serial_port();

	/* Carga de entradas en IDT */
	setup_IDT_content();

	/* Carga de IDTR */
	setup_IDTR();

	_Cli();

	/* Habilita interrupciones en el PIC */
	_mascaraPIC1(0xF8);
	_mascaraPIC2(0xFF);

	_Sti();

//	for (i = 0; i < SCREEN_SIZE - 2; i++) {
//		putc('a');
//	}
	start_shell();
	//printf("Esto es un nu%mero: %d \nEsto es un c\aracter: %c \nEsto es un string: %s \n",34, "T","hello world");
//	refresh_screen();

	while (1) {

	}
}

/**********************************************
 setup_IDT_content()
 Carga de IDT con rutinas de atenci�n IRQ
 *************************************************/

void setup_IDT_content() {
	//	IRQ0: timer tick
	setup_IDT_entry(&idt[0x08], 0x08, (dword) &_int_08_hand, ACS_INT, 0);
	//	IRQ1: keyboard
	setup_IDT_entry(&idt[0x09], 0x08, (dword) &_int_09_hand, ACS_INT, 0);
	//	IRQ2: int80
	setup_IDT_entry(&idt[0x80], 0x08, (dword) &_int_80_hand, ACS_INT, 0);
	//	IRQ4: serial port COM2
	setup_IDT_entry(&idt[0x0C], 0x08, (dword) &_int_0C_hand, ACS_INT, 0);
}

/**********************************************
 setup_IDTR()
 Carga de IDTR
 *************************************************/

void setup_IDTR() {
	idtr.base = 0;
	idtr.base += (dword) &idt;
	idtr.limit = sizeof(idt) - 1;

	_lidt(&idtr);
}

/**********************************************
 Inicializa el teclado.
 *************************************************/

struct keyboard_type keyboard = { ENGLISH, FALSE, FALSE, FALSE, FALSE, FALSE,
		FALSE, 0, 0 };

void setup_keyboard_buffer() {
	int j;
	for (j = 0; j < KEYBOARD_BUFFER_SIZE; j++) {
		keyboard.buffer[j] = 0;
	}
}

/**********************************************
 setup_vts()
 Inicializa las terminales virtuales.
 *************************************************/

struct screen_type screen[VT_AMOUNT] = { { 0 }, { 0 }, { 0 }, { 0 } };

struct shell_type shell[VT_AMOUNT] = { { 0 }, { 0 }, { 0 }, { 0 } };

struct vt_type vt[VT_AMOUNT] = { { &screen[0], &shell[0] }, { &screen[1],
		&shell[1] }, { &screen[2], &shell[2] }, { &screen[3], &shell[3] } };

int current_vt = 0;

void setup_vts() {
	int i, j;
	for (i = 0; i < VT_AMOUNT; i++) {
		for (j = 0; j < SCREEN_SIZE; j++) {
			vt[i].screen->content[j++] = 0;
			vt[i].screen->content[j] = WHITE_TXT;
		}
		for (j = 0; j < SHELL_BUFFER_SIZE; j++) {
			vt[i].shell->buffer[j] = WHITE_TXT;
			vt[i].shell->buffer[j++] = 0;
		}
	}

}

/**********************************************
 setup_serial_port()
 Inicializa los par�metros del puerto serie.
 *************************************************/

void setup_serial_port(){
	//seteo el bit 7 de LCR para setear baud rate
	outb(0x2FB, 0x08);

	//seteo el baud rate en LSB
	outb(0x2F8, 115200 / BAUD);

	//seteo el bit 0 de FCR en 1
	outb(0x2FA, 0x01);

	//apago dlab
	outb(0x2FB, 0x00);


//	/* seteo las opciones del usuario */
//	myout (addr + LCR, cant_bits | paridad | stop_bit);
//
//	/* FCR */
//	//myout (addr + FCR, FIFO1);
//
//	/* seteo la interrupcion por recepcion de datos */
//	myout (addr + IER, RI_ON);
//
//	myout (addr + MCR, AUX_OUT2_ON);
//	/* FCR */
//	myout (addr + FCR, FIFO1);
//	/* LSR */
//	//myout (addr + LSR, DATA_READY);
//


}


void change_terminal(int number) {
	if (current_vt != number) {
		current_vt = number;
		refresh_screen();
	}
}
