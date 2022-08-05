#include<low_level.h>
#include<screen.h>
#include<serial.h>

int main () 
{
    int i;
    char color = DEFAULT_COLOR_ATTR;
    char buffer[100] = {0};
    clear_screen(color);
    
    print("Welcome to MiOS by ",color);
    print("Victor",PACK_COLOR_ATTR(SCREEN_BLACK,SCREEN_BLUE | SCREEN_GREEN));

    serial_configure_baud_rate(SERIAL_COM1_BASE,SERIAL_BAUD_115200);
    
    union serial_line_config line_config;
    union serial_buffer_config buffer_config;
    union serial_modem_config modem_config;

    line_config.line_config_byte = 0x03;
    buffer_config.buffer_config_byte = 0xC7;
    modem_config.modem_config_byte = 0x03;
    
    serial_configure_line(SERIAL_COM1_BASE,line_config);
    serial_configure_buffers(SERIAL_COM1_BASE,buffer_config);
    serial_configure_modem(SERIAL_COM1_BASE,modem_config);

    ksprintf(buffer,"\nThis is a Serial COM %d\n", 1);
    serial_write(SERIAL_COM1_BASE, buffer);

    return 0;
}
