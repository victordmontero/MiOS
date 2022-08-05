
#include<screen.h>
#include<low_level.h>
#include<util.h>
#include<serial.h>

char attribute_byte = PACK_COLOR_ATTR(SCREEN_BLACK,SCREEN_GREEN);

void kputchar(char c)
{
    print_char(c,-1,-1,attribute_byte);
}

void print_char(char character, int col, int row, char attribute_byte)
{
    int offset;
    unsigned char* vidmem = (unsigned char*)VIDEO_ADDRESS;

    if(!attribute_byte)
    {
        attribute_byte = PACK_COLOR_ATTR(SCREEN_BLACK,SCREEN_WHITE);
    }

    if(col >= 0 && row >= 0)
    {
        offset = get_screen_offset(col, row);
    }
    else
    {
        offset = get_cursor();
    }

    if(character=='\n')
    {
        int rows = offset / (2*MAX_COLS);
        offset = get_screen_offset(MAX_COLS-1,rows);
    }
    else
    {
        vidmem[offset]=character;
        vidmem[offset+1]=attribute_byte;
    }

    offset += 2;

    offset = handle_scrolling(offset);
    
    set_cursor(offset);
}

void print_at(char* message, int col, int row, char attribute_byte)
{
    if(col >= 0 && row >= 0)
    {
        set_cursor(get_screen_offset(col,row));
    }

    int i = 0;
    while(message[i]!=0)
    {
        print_char(message[i++],col+i,row,attribute_byte);
    }
    
}

void print(char* message, char attribute_byte)
{
    print_at(message,-1,-1, attribute_byte);
}

int get_screen_offset(int col,int row)
{
    return ((row * MAX_COLS)+col)*2;
}

void clear_screen(char attribute_byte)
{
    fill_screen(0,attribute_byte);
    set_cursor(get_screen_offset(0,0));
}

void fill_screen(char character, char attribute_byte)
{
    int row, col;
    row = col = 0;

    for(row = 0; row<MAX_ROWS;row++)
        for(col = 0;col<MAX_COLS;col++)
            print_char(character,col,row,attribute_byte);
}


int get_cursor(void)
{
    int offset = 0;
    port_byte_out(REG_SCREEN_CTRL,SCREEN_LOW_BYTE_CMD);
    offset = port_byte_in(REG_SCREEN_DATA);
    port_byte_out(REG_SCREEN_CTRL,SCREEN_HIGH_BYTE_CMD);
    offset += port_byte_in(REG_SCREEN_DATA) << 8;
    
    return offset*2;
}

int set_cursor(int offset)
{
    int cursor_offset = offset;
    offset /= 2;

    port_byte_out(REG_SCREEN_CTRL,SCREEN_LOW_BYTE_CMD);
    port_byte_out(REG_SCREEN_DATA,(unsigned char)(offset & 0xFF));
    port_byte_out(REG_SCREEN_CTRL,SCREEN_HIGH_BYTE_CMD);
    port_byte_out(REG_SCREEN_DATA,(unsigned char)((offset>>8) & 0xFF));

    cursor_offset -= 2*MAX_COLS;
    
    return cursor_offset;
}


int handle_scrolling(int offset)
{
    if(offset < MAX_COLS*MAX_ROWS*2)
        return offset;
    
    for (int i = 1; i < MAX_ROWS; i++)
    {
        memory_copy(get_screen_offset(0,i)+VIDEO_ADDRESS,
                    get_screen_offset(0,i-1)+VIDEO_ADDRESS,
                    MAX_COLS*2);
    }

    char* last_line = get_screen_offset(0,MAX_ROWS-1) + VIDEO_ADDRESS;
    for(int i = 0; i < MAX_ROWS*2; i++)
        last_line[i] = 0;
    
    offset -= 2*MAX_COLS;
    return offset;
}

void kitoa(char* buffer, int base, int input)
{
    char* tempbuff = buffer;
    char* p1, *p2;
    int divisor = 10;
    unsigned int uinput = input;

    if(base == 'd' && input < 0)
    {
        *tempbuff++='-';
        buffer++;
        uinput = -input;
    }
    else if (base == 'x')
        divisor = 16;
        
    do{
        int remainder = uinput % divisor;
        *tempbuff++ = remainder < 10 ? remainder + '0' : remainder + 'a' - 10;
    }while (uinput /= divisor);
    
    *tempbuff = '\0';

    p1 = buffer;
    p2 = tempbuff - 1;
    while(p1<p2)
    {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }
}

void kprintf(char* format, ...)
{
    char **arg = (char **) &format;
    int c;
    char buf[20];

  arg++;
  
  while ((c = *format++) != 0)
    {
      if (c != '%')
        kputchar (c);
      else
        {
          char *p, *p2;
          int pad0 = 0, pad = 0;
          
          c = *format++;
          if (c == '0')
            {
              pad0 = 1;
              c = *format++;
            }

          if (c >= '0' && c <= '9')
            {
              pad = c - '0';
              c = *format++;
            }

          switch (c)
            {
            case 'd':
            case 'u':
            case 'x':
              kitoa (buf, c, *((int *) arg++));
              p = buf;
              goto string;
              break;

            case 's':
              p = *arg++;
              if (! p)
                p = "(null)";

            string:
              for (p2 = p; *p2; p2++);
              for (; p2 < p + pad; p2++)
                kputchar (pad0 ? '0' : ' ');
              while (*p)
                kputchar (*p++);
              break;

            default:
              kputchar (*((int *) arg++));
              break;
            }
        }
    }

}

void ksprintf(char* buff, char* format, ...)
{
    char **arg = (char **) &format;
    int c;
    char buf[20];

  arg++;
  
  while ((c = *format++) != 0)
    {
      if (c != '%')
        *buff++ = c;
      else
        {
          char *p, *p2;
          int pad0 = 0, pad = 0;
          
          c = *format++;
          if (c == '0')
            {
              pad0 = 1;
              c = *format++;
            }

          if (c >= '0' && c <= '9')
            {
              pad = c - '0';
              c = *format++;
            }

          switch (c)
            {
            case 'd':
            case 'u':
            case 'x':
              kitoa (buf, c, *((int *) arg++));
              p = buf;
              goto string;
              break;

            case 's':
              p = *arg++;
              if (! p)
                p = "(null)";

            string:
              for (p2 = p; *p2; p2++);
              for (; p2 < p + pad; p2++)
                *buff++ = pad0 ? '0' : ' ';
              while (*p)
                *buff++ = *p++;
              break;

            default:
              *buff++ = *((int *) arg++);
              break;
            }
        }
    }
    buff = 0;
}

int kstrlen(const char* buffer)
{
    unsigned long len = 0;
    while(buffer[len])
        len++;
    return len;
}