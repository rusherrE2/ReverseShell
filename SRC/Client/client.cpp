#include "..\precomp.h"


bool isEmptyOrWhitespace(const std::string& str) {
    return std::all_of(str.begin(), str.end(), isspace);
}

int main() {
    WSADATA wsaData;
    SOCKET clientSocket;
    sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];

    std::string serverIP;
    int port;
    std::string currentDir = std::filesystem::current_path().string();

    std::cout << "Enter server IP (LAN IP or 127.0.0.1): ";
    std::getline(std::cin, serverIP);
    if (isEmptyOrWhitespace(serverIP)) {
        serverIP = "127.0.0.1";
    }

    std::string portInput;
    std::cout << "Enter server port (default 29000): ";
    std::getline(std::cin, portInput);
    port = isEmptyOrWhitespace(portInput) ? 29000 : std::stoi(portInput);

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return 1;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed." << std::endl;
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(serverIP.c_str());

    if (connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed. Error code: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "[+] Connected to server at " << serverIP << ":" << port << std::endl;

    while (true) {
        std::string cmd;
        std::cout << "cmd >> ";
        std::getline(std::cin, cmd);

        // Очистка экрана по команде "cls"
        if (cmd == "cls") {
            system("cls");
            continue;
        }
        if (isEmptyOrWhitespace(cmd)) {
            std::cout << "[!] Empty command, ignoring." << std::endl;
            continue;
        }

       
        send(clientSocket, cmd.c_str(), cmd.length(), 0);
        if (cmd == "exit") break;

        int bytesReceived;
        std::string output;

        do {
            bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
            if (bytesReceived <= 0) break;

            buffer[bytesReceived] = '\0';
            output += buffer;

            if (output.find("[done]") != std::string::npos) break;
        } while (bytesReceived > 0);

        size_t pos = output.find("[done]");
        if (pos != std::string::npos) output.erase(pos);

        std::cout << output << std::endl;
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}