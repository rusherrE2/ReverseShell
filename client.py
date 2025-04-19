import socket
import os
import sys

def is_empty_or_whitespace(s):
    return s.strip() == ""

def main():
    server_ip = input("Enter server IP (default 127.0.0.1): ").strip() or "127.0.0.1"
    port_input = input("Enter server port (default 29000): ").strip()
    server_port = int(port_input) if port_input.isdigit() else 29000

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
            
        # Добавляем chcp 65001 к команде перед отправкой
        final_cmd = f"chcp 65001 >nul && {cmd}"

        try:
            s.sendall(final_cmd.encode())
        except:
            print("[!] Connection lost.")
            break

        # Получаем данные до [done]
        response = b""
        while True:
            chunk = s.recv(2048)
            if not chunk:
                break
            response += chunk
            if b"[done]" in response:
                break

        output = response.decode(errors="ignore").replace("[done]", "").strip()
        print(output)

    s.close()
    print("[-] Disconnected.")

if __name__ == "__main__":
    main()
