/*===---- stdbool.h - Standard header for booleans -------------------------===
    2  *
    3  * Copyright (c) 2008 Eli Friedman
    4  *
    5  * Permission is hereby granted, free of charge, to any person obtaining a copy
    6  * of this software and associated documentation files (the "Software"), to deal
    7  * in the Software without restriction, including without limitation the rights
    8  * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    9  * copies of the Software, and to permit persons to whom the Software is
   10  * furnished to do so, subject to the following conditions:
   11  *
   12  * The above copyright notice and this permission notice shall be included in
   13  * all copies or substantial portions of the Software.
   14  *
   15  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   16  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   17  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   18  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   19  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   20  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   21  * THE SOFTWARE.
   22  *
   23  *===-----------------------------------------------------------------------===
   24  */
   #ifndef __STDBOOL_H
   #define __STDBOOL_H
   
   /* Don't define bool, true, and false in C++, except as a GNU extension. */
   #ifndef __cplusplus
   #define bool _Bool
   #define true 1
   #define false 0
   #elif defined(__GNUC__) && !defined(__STRICT_ANSI__)
   /* Define _Bool, bool, false, true as a GNU extension. */
   #define _Bool bool
   #define bool  bool
   #define false false
#define true  true
#endif

#define __bool_true_false_are_defined 1
   
#endif /* __STDBOOL_H */
