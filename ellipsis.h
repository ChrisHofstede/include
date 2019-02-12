#ifndef ELLIPSIS_H
#define ELLIPSIS_H
#include <windows.h>

/**
 * Algorithm
 * 1. Determine how many characters, nFit, will fit within a given rectangle.
 * 2. Truncate the string with ellipsis characters immediately following the last character
 *    that fits at nFit.
 * 3. Test to see whether the modified string will fit the rectangle.
 * 4. If the string does not fit the rectangle, decrement nFit and repeat with step 2.
 *
 * The following sample code implements this algorithm. Note that the code uses three
 * periods (...) to represent an ellipsis character. If a Unicode font is known to contain
 * an ellipsis character, it could be used in place of the string of periods.
 *
 * This code also assumes that a nonrotated font is used. To modify the code to work with
 * a rotated font, the lpRect parameter must be changed to a scaler width or another
 * datatype that does not assume alignment with the coordinate space axis. Also, the
 * clipping that is used in the ExtTextOut function call must be dropped in favor of
 * another method, such as a clipping region.
 */


BOOL TextOutEllipsis(HDC hDC, LPCTSTR lpString, int nCount, LPRECT lpRect)
{
    SIZE    Size;
    int     nFit;
    int     nWidth = lpRect->right-lpRect->left;
    LPTSTR  lpEllipsisString = NULL;
    BOOL    fSuccess = FALSE;

    if (lpRect)
    {
        // Get how many chars will fit.
        fSuccess = GetTextExtentExPoint( hDC,
            lpString,
            lstrlen(lpString),
            nWidth,
            &nFit,
            NULL,
            &Size);

        // Get the dimensions of the full string.
        if (fSuccess)
        {
            fSuccess = GetTextExtentExPoint( hDC,
                lpString,
                lstrlen(lpString),
                nWidth,
                NULL,
                NULL,
                &Size);
        }

        // Allocate space for text that fits, NULL, and Ellipsis.
        // Note that this is max we need, the result is always <=.
        // Regardless of success this always must be deleted.
        lpEllipsisString = (LPTSTR) new TCHAR[nFit + 1 + 3];

        if (fSuccess && lpEllipsisString != NULL)
        {
            // Copy to our working buffer.
            memcpy( lpEllipsisString, lpString, nFit*sizeof(TCHAR) );
            lpEllipsisString[nFit] = '\0';

            // If we need Ellipsis'.
            while (Size.cx > nWidth && fSuccess && nFit > 0)
            {
                // Add them to what will fit and try again.
                lstrcpy( &lpEllipsisString[nFit], TEXT("...") );
                if (!GetTextExtentExPoint( hDC,
                    lpEllipsisString,
                    lstrlen(lpEllipsisString),
                    nWidth,
                    NULL,
                    NULL,
                    &Size))
                {
                    fSuccess = FALSE;
                }
                nFit--; // Decrement in case we need another pass.

                // Rectangle too small.
                if (nFit <= 0)
                    fSuccess = FALSE;
            }

            // Working buffer now contains the string for ExtTextOut.
        }
    }
    // Else no rectangle so fall out of the rest of this function.

    if (fSuccess)
    {
        // We have a string with ellipsis that fits the rect.
        fSuccess = ExtTextOut( hDC,
                lpRect->left,
                lpRect->top,
                ETO_CLIPPED,
                lpRect,
                lpEllipsisString,
                lstrlen( lpEllipsisString ),
                NULL );
    }

    // Clean up.
    if (lpEllipsisString != NULL)
        delete [] lpEllipsisString;

    return fSuccess;

}
#endif //ELLIPSIS_H
