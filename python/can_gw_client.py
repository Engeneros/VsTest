import socket
import select
import sys

can_gw_addr = '192.168.1.2'
can_gw_port = 29536
can_gw_dev = 'can0'

def main(ip=can_gw_addr, port=can_gw_port, can_dev = can_gw_dev):



    try:
        while True:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            try:
                sock.connect((ip, port))
            except socket.error as msg:
                print(msg)
                continue
            except Exception:
                print('Hello!')
                break

            data = sock.recv(1024)
            if not data:
                sock.close()
            print(data)

            sock.send(b'< open can0 >')

            data = sock.recv(1024)
            if not data:
                sock.close()
            print(data)



            while True:
                readable, writable, exceptional = select.select([sys.stdin, sock], [], [])
                for r in readable:
                    if r is sock:
                        data = sock.recv(1024)
                        if not data:
                            sock.close()
                            break
                        print(data)
                    else:
                        msg = sys.stdin.readline()
                        sock.send(msg)
                        # prompt()

                for w in writable:
                    print("what!?")

                for e in exceptional:
                    print("no way!?")


    except Exception as msg:
        print(msg)



if __name__ == '__main__':
    main()