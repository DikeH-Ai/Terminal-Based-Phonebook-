#ifndef MAIN_H
#define MAIN_H
typedef struct contact
{
    /* data */
    char name[42];
    char number[20];
    char email[100];
    struct contact *next;
} contact;

#endif /*MAIN_H*/
