/*
 * loadstr.h
 *
 *  Created on: 5 jan. 2016
 *      Author: chris
 */

#ifndef BLOADSTR_H_
#define BLOADSTR_H_
#include "bstring.h"

namespace b {

// Assigns the uID string resource to wsDest, returns length (0 if no resource)
template<class T> int LoadString(TString<T>& wsDest, UINT uID,
		HINSTANCE hInstance = ::GetModuleHandle(NULL)) {
	PTCHAR wsBuf; // no need to initialize
	wsDest.clear();
	if (size_t len = ::LoadString(hInstance, uID, (PTCHAR) &wsBuf, 0)) {
		strncpy(wsDest.get_buffer(len + 1), wsBuf, len);
		wsDest.fix_buffer();
	}
	return wsDest.length();
}

// Returns a StringType with uID string resource content (empty if no resource)
template<class T> TString<T> LoadString(UINT uID, HINSTANCE hInstance) {
	TString<T> sDest;
	return LoadString(sDest, uID, hInstance) ? sDest : TString<T>();
}

} // End Namespace
#endif /* LOADSTR_H_ */
