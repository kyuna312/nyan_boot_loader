// Define our own integer types since we can't use stdint.h
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

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

// Nyan Cat frames
static const char *FRAME1[] = {
    "+~~~~~~~~~~~~~~~~+",
    "|   NYAN  CAT   |",
    "+~~~~~~~~~~~~~~~~+",
    "    ∧___∧    ",
    "   (=^.^=)   ",
    "   (\")_(\")",
    "~^~^~^~^~^~^~^",
    "+~~~~~~~~~~~~~~~~+",
    NULL};

static const char *FRAME2[] = {
    "+~~~~~~~~~~~~~~~~+",
    "|   NYAN  CAT   |",
    "+~~~~~~~~~~~~~~~~+",
    "    ∧___∧    ",
    "   (=-.-=)   ",
    "   (\")_(\")",
    "^~^~^~^~^~^~^~",
    "+~~~~~~~~~~~~~~~~+",
    NULL};

static void init_video(void)
{
  // Force text mode and clear screen
  __asm__ volatile(
      "int $0x10" : : "a"(0x0003) // Set mode 3 (80x25 text)
  );

  // Set blue background
  for (int i = 0; i < COLS * ROWS; i++)
  {
    VIDEO_MEM[i] = (BLUE << 12) | (WHITE << 8) | ' ';
  }
}

static void write_char(int x, int y, char c, uint8_t fg, uint8_t bg)
{
  if (x >= 0 && x < COLS && y >= 0 && y < ROWS)
  {
    VIDEO_MEM[y * COLS + x] = (bg << 12) | (fg << 8) | c;
  }
}

static void write_string(int x, int y, const char *str, uint8_t fg, uint8_t bg)
{
  for (int i = 0; str[i]; i++)
  {
    write_char(x + i, y, str[i], fg, bg);
  }
}

static void draw_frame(const char **frame, int x, int y, uint8_t fg, uint8_t bg)
{
  for (int i = 0; frame[i] != NULL; i++)
  {
    write_string(x, y + i, frame[i], fg, bg);
  }
}

static void draw_rainbow(int x, int y)
{
  const char *wave = "~^~^~^~^~^~^~^";
  write_string(x, y, wave, L_RED, BLUE);
  write_string(x, y + 1, wave, YELLOW, BLUE);
  write_string(x, y + 2, wave, L_GREEN, BLUE);
  write_string(x, y + 3, wave, L_CYAN, BLUE);
  write_string(x, y + 4, wave, L_BLUE, BLUE);
  write_string(x, y + 5, wave, L_MAG, BLUE);
}

static void clear_screen(void)
{
  for (int i = 0; i < COLS * ROWS; i++)
  {
    VIDEO_MEM[i] = (BLUE << 12) | (BLUE << 8) | ' ';
  }
}

static void delay(int count)
{
  for (volatile int i = 0; i < count * 10000; i++)
  {
    __asm__ volatile("nop");
  }
}

void __attribute__((section(".text.start"))) kernel_main(void)
{
  init_video();

  int frame = 0;
  const char **current_frame;

  // Calculate center position
  int center_x = (COLS - 16) / 2;
  int center_y = (ROWS - 8) / 2;

  while (1)
  {
    clear_screen();

    // Select frame
    current_frame = (frame % 2 == 0) ? FRAME1 : FRAME2;

    // Draw the current frame
    draw_frame(current_frame, center_x, center_y, L_MAG, BLUE);

    // Draw rainbow trail
    draw_rainbow(center_x - 8, center_y + 8);

    // Update animation
    frame++;
    delay(5);
  }
}