/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#ifndef __ERROR_H__
#define __ERROR_H__

typedef enum {
  ERR_ENDOFCOMMENT,
  ERR_IDENTTOOLONG,
  ERR_INVALIDCHARCONSTANT,
  ERR_OVERFLOW,
  ERR_IDENTSTARTWNUM,
  ERR_INVALIDIDENT,
  ERR_INVALIDSYMBOL
} ErrorCode;


#define ERM_ENDOFCOMMENT "End of comment expected!"
#define ERM_IDENTTOOLONG "Identification too long!"
#define ERM_INVALIDCHARCONSTANT "Invalid const char!"
#define ERM_INVALIDIDENT "Invalid ident! An indent can only start with a letter then followed by letters and numbers!"
#define ERM_INVALIDSYMBOL "Invalid symbol!"
#define ERM_IDENTSTARTWNUM "Ident can't start with a number!"
#define ERM_OVERFLOW "Number value exceeds container type!"

void error(ErrorCode err, int lineNo, int colNo);

#endif
