// Define integer types manually for a bare-metal environment
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

// Define NULL manually
#define NULL ((void *)0)

// Define video memory and screen dimensions
#define VIDEO_MEM ((volatile uint16_t *)0xB8000)
#define COLS 80
#define ROWS 25

// Define color attributes
#define BLUE 1
#define WHITE 15
#define ATTR ((BLUE << 4) | WHITE)

// Define Nyan Cat frame
static const char *NYAN_CAT[] = {
    "  /\\_/\\  ",
    " ( o.o ) ",
    " > ^ <  ",
    NULL};

// Function to write a string to video memory
void write_string(int x, int y, const char *str, uint8_t attr)
{
  volatile uint16_t *where = VIDEO_MEM + (y * COLS + x);
  uint16_t attr_shifted = attr << 8;
  while (*str)
  {
    *where++ = *str++ | attr_shifted;
  }
}

// Function to clear the screen
void clear_screen(uint8_t attr)
{
  volatile uint16_t *vm = VIDEO_MEM;
  uint16_t blank = ' ' | (attr << 8);
  for (int i = 0; i < ROWS * COLS; i++)
  {
    vm[i] = blank;
  }
}

// Main function to display the Nyan Cat
void kernel_main(void)
{
  clear_screen(ATTR);

  int center_x = (COLS - 9) / 2;
  int center_y = (ROWS - 3) / 2;

  // Test: Write a single character to the top-left corner
  write_string(0, 0, "A", ATTR);

  while (1)
  {
    // Draw Nyan Cat
    for (int i = 0; NYAN_CAT[i] != NULL; i++)
    {
      write_string(center_x, center_y + i, NYAN_CAT[i], ATTR);
    }

    // Simple delay loop for frame timing
    for (volatile int delay = 0; delay < 1000000; delay++)
      ;

    // Clear the screen for the next frame
    clear_screen(ATTR);
  }
}