#include <low_level.h>
#include <serial.h>

void serial_configure_baud_rate(unsigned short com, enum serial_baud_speed divisor)
{
    port_byte_out(SERIAL_LINE_COMMAND_PORT(com),SERIAL_LINE_ENABLE_DLAB);
    port_byte_out(SERIAL_DATA_PORT(com),(divisor >> 8) & 0x00FF);
    port_byte_out(SERIAL_DATA_PORT(com),divisor & 0x00FF);
}

void serial_configure_line(unsigned short com, union serial_line_config line_config)
{    
    port_byte_out(SERIAL_LINE_COMMAND_PORT(com),line_config.line_config_byte);
}

void serial_configure_buffers(unsigned short com, union serial_buffer_config buffer_config)
{
    port_byte_out(SERIAL_FIFO_COMMAND_PORT(com),buffer_config.buffer_config_byte);
}

void serial_configure_modem(unsigned short com, union serial_modem_config modem_config)
{
    port_byte_out(SERIAL_MODEM_COMMAND_PORT(com), modem_config.modem_config_byte);
}

int serial_is_transmit_fifo_empty(unsigned short com)
{
    return port_byte_in(SERIAL_LINE_STATUS_PORT(com)) & 0x20;
}

void serial_write(unsigned short com, const char* zstring)
{
    int i = 0;

    while(zstring[i]!=0)
    {
        if(serial_is_transmit_fifo_empty(com))
        {
            port_byte_out(SERIAL_DATA_PORT(com),zstring[i++]);
        }
    }
}
