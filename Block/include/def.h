#ifndef __DEF_H__
#define __DEF_H__

#define BC 7
#define FC 3
struct shape{
    int s[5][5];
};

struct data{
    int x;
    int y;
};
extern struct shape shape_arr[7];
extern struct data t;
#endif //__DEF_H_
