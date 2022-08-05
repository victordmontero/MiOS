#ifndef SERIAL_H_
#define SERIAL_H_


enum serial_baud_speed{
    SERIAL_BAUD_115200 = 1,
    SERIAL_BAUD_57600,
    SERIAL_BAUD_38400,
    SERIAL_BAUD_28800,
    SERIAL_BAUD_23040,
    SERIAL_BAUD_19200,
    SERIAL_BAUD_16457,
    SERIAL_BAUD_14400,
    SERIAL_BAUD_12800,
    SERIAL_BAUD_11520,
    SERIAL_BAUD_10472,
    SERIAL_BAUD_9600,
    SERIAL_BAUD_4800 = 24,
    SERIAL_BAUD_2400 = 48,
    SERIAL_BAUD_1200 = 96,
    SERIAL_BAUD_300 = 384,
    SERIAL_BAUD_110 = 1047,
    SERIAL_BAUD_75 = 1536
};


/* All the I/O ports are calculated relative to the data port. This is because
* all serial ports (COM1, COM2, COM3, COM4) have their ports in the same
* order, but they start at different values.
*/

#define SERIAL_COM1_BASE 0x3F8 /* COM1 base port */

#define SERIAL_DATA_PORT(base) (base)
#define SERIAL_FIFO_COMMAND_PORT(base) (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base) (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base) (base + 5)

/* The I/O port commands */
/* SERIAL_LINE_ENABLE_DLAB:
* Tells the serial port to expect first the highest 8 bits on the data port,
* then the lowest 8 bits will follow
*/
#define SERIAL_LINE_ENABLE_DLAB 0x80

#define SERIAL_DEFAULT_LINE_CONFIG 0x03 /* Default line configuration */
#define SERIAL_DEFAULT_FIFO_CONFIG 0xC7 /* Default fifo configuration */
#define SERIAL_DEFAULT_MODEM_CONFIG 0x03 /* Default modem configuration */

/**
 * Enables (d = 1) or disables (d = 0) DLAB
 * b If break control is enabled (b = 1) or disabled (b = 0)
 * prty The number of parity bits to use
 * s The number of stop bits to use (s = 0 equals 1, s = 1 equals 1.5 or 2)
 * dl Describes the length of the data
 */
union serial_line_config
{
    struct 
    {
        char enable_dlab:1;             /** Enables (d = 1) or disables (d = 0) DLAB */
        char enable_break_control:1;    /** If break control is enabled (b = 1) or disabled (b = 0) */
        char parity_bits:3;             /** The number of parity bits to use */
        char stop_bits:1;               /** The number of stop bits to use (s = 0 equals 1, s = 1 equals 1.5 or 2) */
        char dl:2;                      /** Describes the length of the data */
    }line_config_st;

    unsigned char line_config_byte;
    
};

/**
 * lvl How many bytes should be stored in the FIFO buffers
 * bs If the buffers should be 16 or 64 bytes large
 * r Reserved for future use
 * dma How the serial port data should be accessed
 * clt Clear the transmission FIFO buffer
 * clr Clear the receiver FIFO buffer
 * e If the FIFO buffer should be enabled or not
*/
union serial_buffer_config
{
    struct
    {
        char lvl:2; /** How many bytes should be stored in the FIFO buffers */
        char bs:1;  /** If the buffers should be 16 or 64 bytes large */
        char r:1;   /** Reserved for future use */
        char dma:1; /** How the serial port data should be accessed */
        char clt:1; /** Clear the transmission FIFO buffer */
        char clr:1; /** Clear the receiver FIFO buffer */
        char e:1;   /** If the FIFO buffer should be enabled or not */
    }buffer_config_st;
    
    unsigned char buffer_config_byte;
};

union serial_modem_config{
    struct
    {
        char r:2;   /** Reserved */
        char af:1;  /** Autoflow control enabled */
        char lb:1;  /** Loopback mode (used for debugging serial ports) */
        char ao2:1; /** Auxiliary output 2, used for receiving interrupts */
        char ao1:1; /** Auxiliary output 1 */
        char rts:1; /** Ready To Transmit */
        char dtr:1; /** Data Terminal Ready */ 
    }modem_config_st;

    unsigned char modem_config_byte;
};

/** 
 * Sets the speed of the data being sent. The default speed of a serial port
 * is 115200 bits/s. The argument is a divisor of that number, hence
 * the resulting speed becomes (115200/divisor) bits/s.
 * 
 * @param com The COM port to configure
 * @param divisor The divisor
 */

void serial_configure_baud_rate(unsigned short com, enum serial_baud_speed divisor);

/** serial_configure_line:
 * Configures the line of the given serial port. The port is set to have a
 * data length of 8 bits, no parity bits, one stop bit and break control
 * disabled.
 * --------------------------------------------------------------------------
 * d Enables (d = 1) or disables (d = 0) DLAB
 * b If break control is enabled (b = 1) or disabled (b = 0)
 * prty The number of parity bits to use
 * s The number of stop bits to use (s = 0 equals 1, s = 1 equals 1.5 or 2)
 * dl Describes the length of the data
 * --------------------------------------------------------------------------
 * Bit:     | 7 | 6 | 5 4 3 | 2 | 1 0 |
 * Content: | d | b | prty  | s |  dl |
 * Value:   | 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
 * @param com            The serial port to configure
 * @param line_config    Byte to configure line
*/
void serial_configure_line(unsigned short com, union serial_line_config line_config);

/**
 * Configures the buffer of the given serial port.
 * --------------------------------------------------------------------------
 * lvl How many bytes should be stored in the FIFO buffers
 * bs If the buffers should be 16 or 64 bytes large
 * r Reserved for future use
 * dma How the serial port data should be accessed
 * clt Clear the transmission FIFO buffer
 * clr Clear the receiver FIFO buffer
 * e If the FIFO buffer should be enabled or not
 * --------------------------------------------------------------------------
 * Bit:     | 7 6 | 5  | 4 | 3   | 2   | 1   | 0 |
 * Content: | lvl | bs | r | dma | clt | clr | e |
 * @param com            The serial port to configure
 * @param buffer_config  Byte to configure buffer
*/
void serial_configure_buffers(unsigned short com, union serial_buffer_config buffer_config);

/**
 * Configures the modem.
 * --------------------------------------------------------------------------
 * r   Reserved
 * af  Autoflow control enabled
 * lb  Loopback mode (used for debugging serial ports)
 * ao2 Auxiliary output 2, used for receiving interrupts
 * ao1 Auxiliary output 1
 * rts Ready To Transmit
 * dtr Data Terminal Ready
 * --------------------------------------------------------------------------
 * Bit:     | 7 | 6 | 5  | 4  | 3   | 2   | 1   | 0   |
 * Content: | r | r | af | lb | ao2 | ao1 | rts | dtr |
 * @param com The serial port to configure
 * @param modem_config Byte to configure modem
*/
void serial_configure_modem(unsigned short com, union serial_modem_config modem_config);

/**
 * Checks whether the fifo buffer is empty
 * @param com The serial port to check the fifo buffer for
 * @return 0 if fifo buffer is not empty, 1 if it is.
 */
int serial_is_transmit_fifo_empty(unsigned short com);

void serial_write(unsigned short com, const char* zstring);

#endif
