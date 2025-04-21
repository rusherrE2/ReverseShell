#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <thread>
#include <filesystem>

#pragma once

#pragma comment(lib, "ws2_32.lib")
// Integrating library

#define BUFFER_SIZE 2048
#define PORT 29000