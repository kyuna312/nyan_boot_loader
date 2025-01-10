// Define our own integer types since we can't use stdint.h
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

// Define NULL since we can't use stddef.h
#define NULL ((void *)0)

#define VIDEO_MEM ((volatile uint16_t *)0xB8000)
#define COLS 80
#define ROWS 25

// VGA text mode color codes
#define BLACK 0x0
#define BLUE 0x1
#define GREEN 0x2
#define CYAN 0x3
#define RED 0x4
#define MAGENTA 0x5
#define BROWN 0x6
#define L_GRAY 0x7
#define D_GRAY 0x8
#define L_BLUE 0x9
#define L_GREEN 0xA
#define L_CYAN 0xB
#define L_RED 0xC
#define L_MAG 0xD
#define YELLOW 0xE
#define WHITE 0xF

// Simple Nyan Cat frames
static const char *FRAME1[] = {
    "+-------------+",
    "| NYAN CAT :3|",
    "+-------------+",
    "   /\\___/\\   ",
    "  (  o o  )  ",
    "  (  =^=  )  ",
    "   (______)  ",
    NULL};

static const char *FRAME2[] = {
    "+-------------+",
    "| NYAN CAT :3|",
    "+-------------+",
    "   /\\___/\\   ",
    "  (  - -  )  ",
    "  (  =^=  )  ",
    "   (______)  ",
    NULL};

void write_direct(int x, int y, const char *str, uint8_t attr)
{
  volatile uint16_t *where = VIDEO_MEM + (y * COLS + x);
  int i = 0;
  while (str[i] != '\0')
  {
    where[i] = str[i] | (attr << 8);
    i++;
  }
}

void clear_direct(void)
{
  volatile uint16_t *vm = VIDEO_MEM;
  uint16_t blank = ' ' | (((BLUE << 4) | WHITE) << 8);
  for (int i = 0; i < COLS * ROWS; i++)
  {
    vm[i] = blank;
  }
}

void draw_frame_direct(const char **frame, int x, int y, uint8_t attr)
{
  int i = 0;
  while (frame[i] != NULL)
  {
    write_direct(x, y + i, frame[i], attr);
    i++;
  }
}

void draw_rainbow_direct(int x, int y)
{
  const char *wave = "~~~~~~~~~~~~~~~~";
  write_direct(x, y, wave, (BLUE << 4) | L_RED);
  write_direct(x, y + 1, wave, (BLUE << 4) | YELLOW);
  write_direct(x, y + 2, wave, (BLUE << 4) | L_GREEN);
  write_direct(x, y + 3, wave, (BLUE << 4) | L_CYAN);
}

void delay(int count)
{
  for (volatile int i = 0; i < count * 100000; i++)
  {
    __asm__ volatile("nop");
  }
}

void __attribute__((section(".text.start"))) kernel_main(void)
{
  // Direct video memory test
  volatile uint16_t *vm = VIDEO_MEM;
  vm[0] = 'H' | ((BLUE << 4 | WHITE) << 8);
  vm[1] = 'e' | ((BLUE << 4 | WHITE) << 8);
  vm[2] = 'l' | ((BLUE << 4 | WHITE) << 8);
  vm[3] = 'l' | ((BLUE << 4 | WHITE) << 8);
  vm[4] = 'o' | ((BLUE << 4 | WHITE) << 8);

  delay(10);
  clear_direct();

  int frame = 0;
  const int center_x = (COLS - 13) / 2;
  const int center_y = (ROWS - 7) / 2;

  // Main animation loop
  while (1)
  {
    // Draw title
    write_direct(center_x - 5, center_y - 2,
                 "Welcome to NyanNix!",
                 (BLUE << 4) | WHITE);

    // Draw current frame
    if (frame % 2 == 0)
    {
      draw_frame_direct(FRAME1, center_x, center_y,
                        (BLUE << 4) | L_MAG);
    }
    else
    {
      draw_frame_direct(FRAME2, center_x, center_y,
                        (BLUE << 4) | L_MAG);
    }

    // Draw rainbow
    draw_rainbow_direct(center_x - 15, center_y + 3);

    delay(2);
    frame++;

    // Clear only the cat area
    for (int i = 0; i < 7; i++)
    {
      write_direct(center_x, center_y + i,
                   "             ",
                   (BLUE << 4) | WHITE);
    }
  }
}