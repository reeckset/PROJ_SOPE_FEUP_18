#ifndef MACROS_H
#define  MACROS_H

#define notCaseFlag 1;
#define showFileNamesFlag (1 << 1);
#define showLineNumberFlag (1 << 2);
#define nLinesFlag (1 << 3);
#define wholeWordFlag (1 << 4);
#define recursiveFlag (1 << 5);

#define IsNotCaseFlag(mask) (mask & notCaseFlag)
#define IsShowFileNamesFlag(mask) (mask & showFileNamesFlag)
#define IsShowLineNumberFlag(mask) (mask & showLineNumberFlag)
#define IsNLinesFlag(mask) (mask & nLinesFlag)
#define IsWholeWordFlag(mask) (mask & wholeWordFlag)
#define IsRecursiveFlag(mask) (mask & recursiveFlag)


/*

BITMASK FOR PROGRAM FLAGS
|  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |
|  0 |  0 | -r | -w | -c | -n | -l | -i |

*/

#endif
