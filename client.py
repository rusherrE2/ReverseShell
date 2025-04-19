import socket
import os
import sys

def is_empty_or_whitespace(s):
    return s.strip() == ""

def main():
    server_ip = input("Enter server IP (LAN IP or default - 127.0.0.1): ").strip() or "127.0.0.1"
    port_input = input("Enter server port (default 29000): ").strip()
    server_port = int(port_input) if port_input.isdigit() else 29000

    try:
        s = socket.socket()
        s.connect((server_ip, server_port))
    except Exception as e:
        print(f"[!] Connection failed: {e}")
        sys.exit(1)

    print(f"[+] Connected to {server_ip}:{server_port}")
    current_dir = os.getcwd()

    while True:
        try:
            cmd = input(f"{current_dir}> ").strip()
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

        if cmd.lower().startswith("cd"):
            path = cmd[2:].strip(" \"'")
            if not path:
                print("[!] No directory specified.")
                continue

            new_path = os.path.abspath(os.path.join(current_dir, path))
            if os.path.isdir(new_path):
                current_dir = new_path
            else:
                print("[!] Invalid directory.")
            continue

        # отправляем команду с текущей директорией
        full_cmd = f'cd /d "{current_dir}" && {cmd}'
        try:
            s.sendall(full_cmd.encode())
        except:
            print("[!] Connection lost.")
            break

        # Получение ответа от сервера до [done]
        response = b""
        while True:
            chunk = s.recv(2048)
            if not chunk:
                break
            response += chunk
            if b"[done]" in response:
                break
        
        # Удаляем маркер конца и выводим результат
        output = response.decode(errors="ignore").replace("[done]", "").strip()
        print(output)


    s.close()
    print("[-] Disconnected.")

if __name__ == "__main__":
    main()
