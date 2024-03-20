/*
 * Userspace program that communicates with the vga_ball device driver
 * through ioctls
 *
 * Stephen A. Edwards
 * Columbia University
 */

#include <stdio.h>
#include "vga_ball.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define WIDTH 1600 - 64 
#define HEIGHT 525 - 32

int vga_ball_fd;

struct Point {
	int x;
	int y;
	int dx;
	int dy;
};

// Update the position of the ball
void update(struct Point point) {
	vga_ball_arg_t vla;
	vga_ball_color_t position;
	
	point.x += point.dx;
	point.y += point.dy;

	if (point.x <= 0 || point.x >= WIDTH - 1) {
		point.dx = -point.dx;
	}
	if (point.y <= 0 || point.y >= HEIGHT - 1) {
		point.dy = -point.dy;
	}
	
	position = {0x00, (unsigned char) point.x, (unsigned char) point.y};
	vla.background = position; 

	if (ioctl(vga_ball_fd, VGA_BALL_WRITE_BACKGROUND, &vla)) {
		perror("ioctl(VGA_BALL_WRITE_POSITION) failed");
		return;
	}
}

// Set the background color 
void set_background_color(const vga_ball_color_t *c)
{
  vga_ball_arg_t vla;
  vla.background = *c;
  if (ioctl(vga_ball_fd, VGA_BALL_WRITE_BACKGROUND, &vla)) {
      perror("ioctl(VGA_BALL_SET_BACKGROUND) failed");
      return;
  }
}


int main()
{
  vga_ball_arg_t vla;
  int i;
  static const char filename[] = "/dev/vga_ball";
  static const vga_ball_color_t colors[] = {
    { 0xff, 0x00, 0x00 }, /* Red */
    { 0x00, 0xff, 0x00 }, /* Green */
    { 0x00, 0x00, 0xff }, /* Blue */
    { 0xff, 0xff, 0x00 }, /* Yellow */
    { 0x00, 0xff, 0xff }, /* Cyan */
    { 0xff, 0x00, 0xff }, /* Magenta */
    { 0x80, 0x80, 0x80 }, /* Gray */
    { 0x00, 0x00, 0x00 }, /* Black */
    { 0xff, 0xff, 0xff }  /* White */
  };

# define COLORS 9

  printf("VGA ball Userspace program started\n");

  if ( (vga_ball_fd = open(filename, O_RDWR)) == -1) {
    fprintf(stderr, "could not open %s\n", filename);
    return -1;
  }
/*
  printf("initial state: ");
  print_background_color();

  for (i = 0 ; i < 24 ; i++) {
    set_background_color(&colors[i % COLORS ]);
    print_background_color();
    usleep(400000);
  }
*/

// Update ball position 
	struct Point ball = {10, 10, 10, 10};
	while (1) {
		update(ball);
    usleep(400000);
	}

  printf("VGA BALL Userspace program terminating\n");
  return 0;
}
