#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/syscall.h> 
#include <unistd.h>

/*********************************************************************
 *
 * These C functions use patterns and functionality often found in
 * operating system code. Your job is to implement them. Of course you
 * should write test cases. However, do not hand in your test cases
 * and (especially) do not hand in a main() function since it will
 * interfere with our tester.
 *
 * Additional requirements on all functions you write:
 *
 * - you may not refer to any global variables
 *
 * - you may not call any functions except those specifically
 *   permitted in the specification
 *
 * - your code must compile successfully on CADE lab Linux
 *   machines when using:
 *
 * /usr/bin/gcc -O2 -fmessage-length=0 -pedantic-errors -std=c99 -Werror -Wall -Wextra -Wwrite-strings -Winit-self -Wcast-align -Wcast-qual -Wpointer-arith -Wstrict-aliasing -Wformat=2 -Wmissing-include-dirs -Wno-unused-parameter -Wshadow -Wuninitialized -Wold-style-definition -c assign1.c 
 *
 * NOTE 1: Some of the specifications below are specific to 64-bit
 * machines, such as those found in the CADE lab.  If you choose to
 * develop on 32-bit machines, some quantities (the size of an
 * unsigned long and the size of a pointer) will change. Since we will
 * be grading on 64-bit machines, you must make sure your code works
 * there.
 *
 * NOTE 2: You should not need to include any additional header files,
 * but you may do so if you feel that it helps.
 *
 * HANDIN: submit your finished file, still called assign1.c, in Canvas
 *
 *
 *********************************************************************/

/*********************************************************************
 *
 * byte_sort()
 *
 * specification: byte_sort() treats its argument as a sequence of
 * 8 bytes, and returns a new unsigned long integer containing the
 * same bytes, sorted numerically, with the smaller-valued bytes in
 * the lower-order byte positions of the return value
 * 
 * EXAMPLE: byte_sort (0x0403deadbeef0201) returns 0xefdebead04030201
 *
 *********************************************************************/

unsigned long byte_sort (unsigned long arg)
{
   unsigned long bytes[8];

   unsigned long i;
   unsigned long j;

   for (i = 0; i < 8; ++i)
   {
   		bytes[i] = (arg & ( (unsigned long)0xff << (i * 8))) >> (i * 8);
   }

   for (i = 0; i < 8; ++i)
   {
   		for (j = i; j < 8; ++j)
   		{
   			if(bytes[i] < bytes[j])
   			{
   				unsigned long t = bytes[i];
   				bytes[i] = bytes[j];
   				bytes[j] = t;
   			}
   		}
   }

   unsigned long ret = 0x0;
   for(i = 0; i < 8; i++)
   {
   		ret = ret | (bytes[(7-i)] << ((i) * 8));
   }

   return ret;
}



/*********************************************************************
 *
 * nibble_sort()
 *
 * specification: nibble_sort() treats its argument as a sequence of 16 4-bit
 * numbers, and returns a new unsigned long integer containing the same nibbles,
 * sorted numerically, with smaller-valued nibbles towards the "small end" of
 * the unsigned long value that you return
 *
 * the fact that nibbles and hex digits correspond should make it easy to
 * verify that your code is working correctly
 * 
 * EXAMPLE: nibble_sort (0x0403deadbeef0201) returns 0xfeeeddba43210000
 *
 *********************************************************************/

unsigned long nibble_sort (unsigned long arg)
{
   unsigned long bytes[16];

   unsigned long i;
   unsigned long j;

   for (i = 0; i < 16; ++i)
   {
   		bytes[i] = (arg & ( (unsigned long)0xf << (i * 4))) >> (i * 4);
   		printf("%lx\n", bytes[i]);
   }

   for (i = 0; i < 16; ++i)
   {
   		for (j = i; j < 16; ++j)
   		{
   			if(bytes[i] < bytes[j])
   			{
   				unsigned long t = bytes[i];
   				bytes[i] = bytes[j];
   				bytes[j] = t;
   			}
   		}
   }

   unsigned long ret = 0x0;
   for(i = 0; i < 16; i++)
   {
   		ret = ret | (bytes[(15-i)] << ((i) * 4));
   }

   return ret;
  return 0;
}

/*********************************************************************
 *
 * name_list()
 *
 * specification: allocate and return a pointer to a linked list of
 * struct elts
 *
 * - the first element in the list should contain in its "val" field the first
 *   letter of your first name; the second element the second letter, etc.;
 *
 * - the last element of the linked list should contain in its "val" field
 *   the last letter of your first name and its "link" field should be a null
 *   pointer
 *
 * - each element must be dynamically allocated using a malloc() call
 *
 * - if any call to malloc() fails, your function must return NULL and must also
 *   free any heap memory that has been allocated so far; that is, it must not
 *   leak memory when allocation fails
 *  
 *********************************************************************/

struct elt {
  char val;
  struct elt *link;
};

struct elt *name_list (void)
{
    const char *name = "monish";

    struct elt *first = (struct elt*)malloc(sizeof(struct elt));
    if(first == NULL)
        return NULL;
    first->val = name[0];

    struct elt *runner = first;
    int i;
    for (i = 1; i < 7; i++)
    {
        struct elt *alloc = (struct elt*)malloc(sizeof(struct elt));
        if(alloc == NULL)
        {
                runner = first;
                while(runner != NULL)
                {
                    struct elt * temp = runner;
                    runner = runner->link;
                    free(temp);
                }
                return NULL;
        }
        alloc->val = name[i];
        runner->link = alloc;
        runner = runner->link;
    }
  return first;
}



/*********************************************************************
 *
 * convert()
 *
 * specification: depending on the value of "mode", print "value" as
 * octal, binary, or hexidecimal to stdout, followed by a newline
 * character
 *
 * extra requirement 1: output must be via putc()
 *
 * extra requirement 2: print nothing if "mode" is not one of OCT,
 * BIN, or HEX
 *
 * extra requirement 3: all leading/trailing zeros should be printed
 *
 * EXAMPLE: convert (HEX, 0xdeadbeef) should print
 * "00000000deadbeef\n" (including the newline character but not
 * including the quotes)
 *
 *********************************************************************/

enum format_t {
  OCT = 66, BIN, HEX
};

void convert (enum format_t mode, unsigned long value)
{
	int base = 0;
	int bits = 0;
	int len = 0;
	switch(mode)
	{
		case OCT: { base = 8; bits = 3; len = 22; break;}
		case BIN: { base = 2; bits = 1; len = 64; break;}
		case HEX: { base = 16; bits = 4; len = 16; break;}
	}

	unsigned long remainder = value;
	char result[len];

	int i; int ri = 0;
	for (i = 0; i < 64; i+=bits)
	{
		if(base == 16)
		{
			switch(remainder % base)
			{
				case 10: { result[ri] = 'a'; break; }
				case 11: { result[ri] = 'b'; break; }
				case 12: { result[ri] = 'c'; break; }
				case 13: { result[ri] = 'd'; break; }
				case 14: { result[ri] = 'e'; break; }
				case 15: { result[ri] = 'f'; break; }

				default:
					result[ri] = (remainder % base) + '0';
			}
		}
		else
		{
			result[ri] = (remainder % base) + '0';
		}
		
		ri++;
		remainder /= base;
	}

	for (i = len - 1; i >= 0; i--)
	{
		putc(result[i],stdout);
	}
	putc('\n',stdout);
}

int main(void)
{
    //printf("%lx\n",byte_sort(0x0403deadbeef0201));
    //printf("%lx\n",nibble_sort(0x0403deadbeef0201));
    // struct elt *answer = name_list();
    // if(answer == NULL)
    // {
    //     printf("answer was null\n");
    //     return 0;
    // }
    // while(answer->link != NULL)
    // {
    //     printf("%c ",answer->val);
    //     answer = answer->link;
    // }
    // printf("\n");
    // return 0;
	convert(HEX, 0xdeadbeef);
	convert(HEX, 0xffffffffffffffff);
	convert(HEX, 0x0);
	convert(BIN, 0xdeadbeef);
	convert(BIN, 0xffffffffffffffff);
	convert(BIN, 0x0);
	convert(OCT, 0xdeadbeef);
	convert(OCT, 0xffffffffffffffff);
	convert(OCT, 0x0);

}

/*********************************************************************
 *
 * draw_me()
 *
 * this function creates a file called me.txt which contains an ASCII-art
 * picture of you (it does not need to be very big). the file must (pointlessly,
 * since it does not contain executable content) be marked as executable.
 * 
 * extra requirement 1: you may only call the function syscall() (type "man
 * syscall" to see what this does)
 *
 * extra requirement 2: you must ensure that every system call succeeds; if any
 * fails, you must clean up the system state (closing any open files, deleting
 * any files created in the file system, etc.) such that no trash is left
 * sitting around
 *
 * you might be wondering how to learn what system calls to use and what
 * arguments they expect. one way is to look at a web page like this one:
 * http://blog.rchapman.org/post/36801038863/linux-system-call-table-for-x86-64
 * another thing you can do is write some C code that uses standard I/O
 * functions to draw the picture and mark it as executable, compile the program
 * statically (e.g. "gcc foo.c -O -static -o foo"), and then disassemble it
 * ("objdump -d foo") and look at how the system calls are invoked, then do
 * the same thing manually using syscall()
 *
 *********************************************************************/

void draw_me (void)
{
}
