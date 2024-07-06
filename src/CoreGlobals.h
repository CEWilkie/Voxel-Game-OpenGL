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

#endif //UNTITLED7_COREGLOBALS_H
