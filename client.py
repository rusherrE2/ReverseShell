import socket

HOST = '192.168.0.100'  
PORT = 5555            

def main():
    print(f"[+] Connecting to {HOST}:{PORT}...")
    try:
        with socket.create_connection((HOST, PORT)) as s:
            print("[+] Connected. Type commands or 'exit' to quit.\n")

            while True:
                try:
                    cmd = input("cmd> ")
                    if not cmd.strip():
                        continue

                    s.sendall(cmd.encode())

                    if cmd.strip().lower() == "exit":
                        break

                    response = b""
                    while True:
                        chunk = s.recv(4096)
                        if not chunk:
                            break
                        response += chunk
                        if b"[done]" in response:
                            break

                    output = response.decode(errors="ignore").replace("[done]", "")
                    print(output.strip())

                except KeyboardInterrupt:
                    print("\n[-] Stopped by user.")
                    break
                except Exception as e:
                    print(f"[-] Runtime error: {e}")
                    break

    except Exception as e:
        print(f"[-] Connection failed: {e}")

if __name__ == "__main__":
    main()
