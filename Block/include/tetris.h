#ifndef __TETRIS_H__
#define __TETRIS_H__
void draw_eleyment(int x,int y,int c);
void draw_shape(int x,int y,struct shape p,int c);
void timer_tetris(struct data* p);
int tetris(struct data* p);
#endif
