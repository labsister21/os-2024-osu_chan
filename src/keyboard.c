#include "header/driver/keyboard.h"
#include "header/text/framebuffer.h"
#include "header/cpu/portio.h"
#include "header/stdlib/string.h"

const char keyboard_scancode_1_to_ascii_map[256] = {
    0,
    0x1B,
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    '0',
    '-',
    '=',
    '\b',
    '\t',
    'q',
    'w',
    'e',
    'r',
    't',
    'y',
    'u',
    'i',
    'o',
    'p',
    '[',
    ']',
    '\n',
    0,
    'a',
    's',
    'd',
    'f',
    'g',
    'h',
    'j',
    'k',
    'l',
    ';',
    '\'',
    '`',
    0,
    '\\',
    'z',
    'x',
    'c',
    'v',
    'b',
    'n',
    'm',
    ',',
    '.',
    '/',
    0,
    '*',
    0,
    ' ',
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    '-',
    0,
    0,
    0,
    '+',
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,

    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};

static struct KeyboardDriverState keyboard_state;

// Activate keyboard ISR / start listen keyboard & save to buffer
void keyboard_state_activate(void)
{
  keyboard_state.keyboard_input_on = true;
  // keyboard_state.index = 0;
  // memset(keyboard_state.keyboard_buffer, 0, 256);
}

// Deactivate keyboard ISR / stop listening keyboard interrupt
void keyboard_state_deactivate(void)
{
  keyboard_state.keyboard_input_on = false;
}

// Get keyboard buffer value and flush the buffer - @param buf Pointer to char buffer
void get_keyboard_buffer(char *buf)
{
  memcpy(buf, keyboard_state.keyboard_buffer, 256);
}

/**
 * Handling keyboard interrupt & process scancodes into ASCII character.
 * Will start listen and process keyboard scancode if keyboard_input_on.
 */
int row = 0;
int col = 0;
// static bool tertekan = false;

void keyboard_isr(void)
{
  uint8_t scancode = in(KEYBOARD_DATA_PORT);
  char hasil_map = keyboard_scancode_1_to_ascii_map[scancode];

  // hmmm, kalau keyboard driver gak aktif , indeknya kita set ke 0
  // masih agak ragu
  if (!keyboard_state.keyboard_input_on)
  {
    keyboard_state.index = 0;
  }
  else
  {
    // kalau keyboard yang diteken dan dikenali
    if (hasil_map != 0)
    {
      if (hasil_map == '\b')
      {
        if (keyboard_state.index > 0)
        {
          keyboard_state.index--;
          keyboard_state.keyboard_buffer[keyboard_state.index] = 0;
          if (col == 0)
          {
            row--;
            col = 79;
            if (row < 0)
            {
              row = 0;
              col = 0;
            }
          }
          else
          {
            col--;
          }

          framebuffer_write(row, col, ' ', 0xFF, 0);
        }
      }
      else if (hasil_map == '\n')
      {
        memset(keyboard_state.keyboard_buffer, 0, sizeof(keyboard_state.keyboard_buffer));
        keyboard_state.index = 0;
        keyboard_state.keyboard_input_on = 0;
        row++;
        col = 0;
      }
      else
      {
        keyboard_state.keyboard_buffer[keyboard_state.index] = hasil_map;
        keyboard_state.index++;

        if (col >= 80)
        {
          row++;
          col = 0;
        }
        framebuffer_write(row, col, hasil_map, 0xFF, 0);
        col++;
      }
    }
    else
    {

      if (scancode == EXT_SCANCODE_UP)
      {
        if (row > 0)
        {
          row--;
        }
      }

      else if (scancode == EXT_SCANCODE_DOWN)
      {
        if (row < 25)
        {
          row++;
        }
      }

      else if (scancode == EXT_SCANCODE_LEFT)
      {
        if (col > 0)
        {
          col--;
        }
        else
        {
          if (row > 0)
          {
            row--;
            col = 79;
          }
        }
      }

      else if (scancode == EXT_SCANCODE_RIGHT)
      {
        if (col < 79)
        {
          keyboard_state.keyboard_buffer[keyboard_state.index] = '0';
          keyboard_state.index++;
          col++;
        }
      }

      else
      {
        pic_ack(IRQ_KEYBOARD);
        return;
      }
    }
    framebuffer_set_cursor(row, col);
  }
  pic_ack(IRQ_KEYBOARD);
}

void put_char(char c, uint32_t color)
{
  if (c != '\n'){
    framebuffer_write(row, col, c, color, 0);
  }
  if (col == 80 - 1 || c == '\n')
  {
    row++;
    col = 0;
    if (row == 25)
    {
      // kita salin dari baris kedua, taro di atas
      memcpy(FRAMEBUFFER_MEMORY_OFFSET, FRAMEBUFFER_MEMORY_OFFSET + 80 * 2, (80 * 2 * 25) - (80 * 2));

      // bersh-bersih baris dibawah
      for (int i = 0; i < 80; i++)
      {
        framebuffer_write(25 - 1, i, ' ', 0xFF, 0);
      }
    }
  }
  else
  {
    col++;
  }
}

void puts(const char *str, uint32_t len, uint32_t color)
{
  for (uint32_t i = 0; i < len; i++)
  {
    if (str[i] == '\0'){
      break;
    }
    put_char(str[i], color);
  }
  framebuffer_set_cursor(row, col);
}
