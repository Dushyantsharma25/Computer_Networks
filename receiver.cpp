#include <iostream>
#include <string>
#include <bitset>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

string compute_crc(const string& data, const string& poly) {
    string msg = data + string(poly.length() - 1, '0');
    string remainder = msg;

    for (size_t i = 0; i <= msg.length() - poly.length(); ++i) {
        if (remainder[i] == '1') {
            for (size_t j = 0; j < poly.length(); ++j) {
                remainder[i + j] = (remainder[i + j] == poly[j]) ? '0' : '1';
            }
        }
    }

    return remainder.substr(msg.length() - poly.length() + 1);
}

bool is_crc_valid(const string& received, const string& poly) {
    string remainder = compute_crc(received, poly);
    return remainder.find('1') == string::npos;
}

int main() {
    char buffer[1024] = {0};

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_addr{}, client_addr{};
    socklen_t addr_len = sizeof(client_addr);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_fd, 1);

    cout << "[Receiver] Waiting for data...\n";

    int client_fd = accept(server_fd, (sockaddr*)&client_addr, &addr_len);
    read(client_fd, buffer, sizeof(buffer));
    string received = buffer;

    cout << "[Receiver] Received: " << received << endl;

    string polynomial = "11010101";
    if (is_crc_valid(received, polynomial)) {
        cout << "[Receiver] Data is VALID (CRC passed)." << endl;
    } else {
        cout << "[Receiver] Data is CORRUPTED (CRC failed)." << endl;
    }

    close(client_fd);
    close(server_fd);
    return 0;
}
