#ifndef __CONSOLE_H__
#define __CONSOLE_H__

/*
 * This is the function called by printf to send its output to the screen. You
 * have to implement it in the kernel and in the user program.
 */
extern void console_putbytes(const char *data, int len);
extern void console_write(const char *data);
extern void console_write_color(const char *data, int color_type);
extern void console_write_hour(const char hour[8]);
extern void init_console(void);
extern void clear_console(void);

#endif
