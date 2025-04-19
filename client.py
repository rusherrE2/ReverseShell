import socket
import os
import sys

def is_empty_or_whitespace(s):
    return s.strip() == ""

def main():
    server_ip = input("Enter server IP (default 127.0.0.1): ").strip() or "127.0.0.1"
    port_input = input("Enter server port (default 5555): ").strip()
    server_port = int(port_input) if port_input.isdigit() else 5555

    try:
        s = socket.socket()
        s.connect((server_ip, server_port))
    except Exception as e:
        print(f"[!] Connection failed: {e}")
        sys.exit(1)
    os.system("clear")
    print(f"[+] Connected to {server_ip}:{server_port}")

    while True:
        try:
            cmd = input("cmd>> ").strip()
        except EOFError:
            break

        if is_empty_or_whitespace(cmd):
            print("[!] Empty command, ignoring.")
            continue

        if cmd.lower() == "exit":
            s.sendall(cmd.encode())
            break

        if cmd.lower() == "cls":
            os.system("clear")
            continue

        # Отправляем команду как есть (в том числе cd)
        try:
            s.sendall(cmd.encode())
        except:
            print("[!] Connection lost.")
            break

        # Получение данных до [done]
        response = b""
        while True:
            chunk = s.recv(2048)
            if not chunk:
                break
            response += chunk
            if b"[done]" in response:
                break

        # Вывод ответа
        output = response.decode(errors="ignore").replace("[done]", "").strip()
        print(output)

    s.close()
    print("[-] Disconnected.")

if __name__ == "__main__":
    main()
