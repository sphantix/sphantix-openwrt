/***********************************************************************
 *
 *  Copyright (c) 2015 sphantix
 *  All Rights Reserved
 *
# 
# 
# 
#
 * 
 ************************************************************************/

#ifndef __UTL_ASSERT_H__
#define __UTL_ASSERT_H__

#include "utl.h"

/** Check that an assumed condition is TRUE.
 * 
 * This should be used during development only.  For production code,
 * compile with NDEBUG defined.
 */
#define utlAst_assert(expr) utlAst_assertFunc(__FILE__, __LINE__, __STRING(expr), (expr))

/** The actual assert function, don't call this directly, use the macro.
 *
 * @param filename (IN) file where the assert occured.
 * @param lineNumber (IN) Linenumber of the assert statement.
 * @param exprString (IN) The actual expression that is being asserted.
 * @param expr       (IN) The result of the evaluation of the expression.  0 is fail,
 *                        non-zero is pass.
 */
void utlAst_assertFunc(const char *filename, UINT32 lineNumber, const char *exprString, SINT32 expr);
#endif /* __UTL_ASSERT_H__ */
