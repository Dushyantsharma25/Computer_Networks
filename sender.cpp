#include <iostream>
#include <string>
#include <bitset>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

string to_binary_string(const string& input) {
    string result = "";
    for (char c : input) {
        result += bitset<8>(c).to_string(); // Convert each char to 8-bit binary
    }
    return result;
}

string compute_crc(const string& data, const string& poly) {
    string msg = data + string(poly.length() - 1, '0'); // Append zeros
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

int main() {
    string message = "DUSHY";
    string data_binary = to_binary_string(message);
    string polynomial = "11010101"; // CRC-8 (0xD5)

    string crc = compute_crc(data_binary, polynomial);
    string data_with_crc = data_binary + crc;

    cout << "[Sender] Sending binary with CRC: " << data_with_crc << endl;

    // Socket setup
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.23", &server_addr.sin_addr);

    connect(sock, (sockaddr*)&server_addr, sizeof(server_addr));
    send(sock, data_with_crc.c_str(), data_with_crc.length(), 0);

    cout << "[Sender] Data sent!\n";
    close(sock);

    return 0;
}
