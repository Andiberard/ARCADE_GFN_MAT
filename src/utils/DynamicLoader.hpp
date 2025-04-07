/* ------------------------------------------------------------------------------------ *
 *                                                                                      *
 * EPITECH PROJECT - Wed, Mar, 2025                                                     *
 * Title           - B-OOP-400-TLS-4-1-arcade-nolan.dumoulin [WSL: Debian]              *
 * Description     -                                                                    *
 *     DynamicLoader                                                                    *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ *
 *                                                                                      *
 *       ▄▀▀█▄▄▄▄  ▄▀▀▄▀▀▀▄  ▄▀▀█▀▄    ▄▀▀▀█▀▀▄  ▄▀▀█▄▄▄▄  ▄▀▄▄▄▄   ▄▀▀▄ ▄▄             *
 *      ▐  ▄▀   ▐ █   █   █ █   █  █  █    █  ▐ ▐  ▄▀   ▐ █ █    ▌ █  █   ▄▀            *
 *        █▄▄▄▄▄  ▐  █▀▀▀▀  ▐   █  ▐  ▐   █       █▄▄▄▄▄  ▐ █      ▐  █▄▄▄█             *
 *        █    ▌     █          █        █        █    ▌    █         █   █             *
 *       ▄▀▄▄▄▄    ▄▀        ▄▀▀▀▀▀▄   ▄▀        ▄▀▄▄▄▄    ▄▀▄▄▄▄▀   ▄▀  ▄▀             *
 *       █    ▐   █         █       █ █          █    ▐   █     ▐   █   █               *
 *       ▐        ▐         ▐       ▐ ▐          ▐        ▐         ▐   ▐               *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ */

#pragma once

#include <string>
#include <dlfcn.h>
#include <iostream>
#include "../exceptions/LibraryLoadException.hpp"

namespace arcade {

    template <typename T>
    class DynamicLoader {
    private:
        void *handle;
        std::string libraryPath;
     
    public:
        DynamicLoader(const std::string &path) : handle(nullptr), libraryPath(path) {
            std::cerr << "DynamicLoader created for path: " << path << std::endl;
        }
         
        ~DynamicLoader() {
            // The library should remain open as long as objects created from it are in use
            // closeLibrary();
        }
     
        T *getInstance(const std::string &entryPoint) {
            try {
                std::cerr << "getInstance called for entry point: " << entryPoint << std::endl;
                
                if (!handle) {
                    std::cerr << "Opening library: " << libraryPath << std::endl;
                    handle = dlopen(libraryPath.c_str(), RTLD_LAZY);
                    if (!handle) {
                        throw LibraryLoadException("Cannot open library: " + std::string(dlerror()));
                    }
                } else {
                    std::cerr << "Library already open, reusing handle" << std::endl;
                }
         
                dlerror();
         
                std::cerr << "Looking up symbol: " << entryPoint << std::endl;
                using CreateInstanceFunc = T *(*)();
                CreateInstanceFunc createInstance = reinterpret_cast<CreateInstanceFunc>(dlsym(handle, entryPoint.c_str()));
         
                const char *dlsym_error = dlerror();
                if (dlsym_error) {
                    throw LibraryLoadException("Cannot load symbol '" + entryPoint + "': " + std::string(dlsym_error));
                }
                
                std::cerr << "Symbol found, attempting to call function..." << std::endl;
                T* instance = createInstance();
                std::cerr << "Function called successfully, instance created" << std::endl;
                
                if (!instance) {
                    std::cerr << "WARNING: createInstance returned nullptr" << std::endl;
                    return nullptr;
                }
                
                std::cerr << "Returning valid instance" << std::endl;
                return instance;
            } catch (const std::exception& e) {
                std::cerr << "Exception in getInstance: " << e.what() << std::endl;
                throw;
            } catch (...) {
                std::cerr << "Unknown exception in getInstance" << std::endl;
                throw;
            }
        }
     
        void closeLibrary() {
            if (handle != nullptr) {
                std::cerr << "Closing library handle" << std::endl;
                dlclose(handle);
                handle = nullptr;
            }
        }
    };
}

/* ------------------------------------------------------------------------------------ *
 *                                                                                      *
 * MIT License                                                                          *
 * Copyright (c) 2025 Anonymous                                                         *
 *                                                                                      *
 * Permission is hereby granted, free of charge, to any person obtaining a copy         *
 * of this software and associated documentation files (the "Software"), to deal        *
 * in the Software without restriction, including without limitation the rights         *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell            *
 * copies of the Software, and to permit persons to whom the Software is                *
 * furnished to do so, subject to the following conditions:                             *
 *                                                                                      *
 * The above copyright notice and this permission notice shall be included in all       *
 * copies or substantial portions of the Software.                                      *
 *                                                                                      *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR           *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,             *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE          *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER               *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,        *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE        *
 * SOFTWARE.                                                                            *
 *                                                                                      *
 * ------------------------------------------------------------------------------------ */