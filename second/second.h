#ifndef _SECOND_H
#define _SECOND_H

struct chain{
        char* name;
        int* arr;
};
struct component{
        char** input;
        int* input_index;
        int innum;
        char* name;
        int name_index;
        int instruct_index;
        char** selectors;
        struct component* next;
};

struct chain* buildEle(struct chain*, int, char*);
void buildcomp(struct component*, int, int);
void freecomponent(struct component*);
void buildinput(struct chain**, int, int);

int And(int, int);
int Or(int, int);
int Xor(int, int);
int Not(int);
int nor(int, int);
int nand(int, int);
void decoder(struct component*);
int getindex(char*);
void multiplexer(struct component);
void setinputindex();
bool step(FILE*, int, int, int);
void operate(int, int);
void print(int, int);

#endif
