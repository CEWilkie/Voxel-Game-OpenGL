//
// Created by cew05 on 18/04/2024.
//

#ifndef UNTITLED7_COREGLOBALS_H
#define UNTITLED7_COREGLOBALS_H

#include <vector>
#include <iostream>

template<class T>
inline void LogError(const std::string& errorMsgDecorative = "", const T& errorMsg = "", bool fatal = false) {
    printf("New Error Raised!\nError Desc: %s\nError Proper: %s\nError Fatal?  %d\n",
           errorMsgDecorative.c_str(), errorMsg, fatal);
}



static void glClearErrors() {
    while (glGetError() != GL_NO_ERROR);
}

static bool glErrorStatus(const char* function, int line) {
    while (GLenum error = glGetError()) {
        printf("OPENGL ERROR: CODE %d, STRING %s\nIN FUNCTION: %s, LINE: %d\n",
               error, glewGetErrorString(error), function, line);
        return true;
    }
    return false;
}

#define GLCHECK(x) glClearErrors(); x; glErrorStatus(#x, __LINE__);

#endif //UNTITLED7_COREGLOBALS_H
