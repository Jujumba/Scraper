#include "Http.hpp"
std::string Http::get(const std::string& url, bool save) {
    WSADATA wsaData {};
    if ( WSAStartup(REQUIRED_VERSION, &wsaData) != 0) {
        return "";
    }

    std::string content = read(const_cast<std::string &>(url));
    WSACleanup();
    if (save) {
        std::ofstream ofs{"../download.html"}; //todo: generate random file name!
        ofs << content;
    }
    return content;
}


void Http::parse_url(std::string& url, std::string &server, std::string &filepath) {
    int n;
    int from = (url.starts_with(HTTP_PREFIX)) ? 8 : 0; // it doesn't matter either if http:// or https://
    n = url.find('/', from - 1);
    if (n != std::string::npos) {
        int to = url.find('/', n+ 1);
        if (to != std::string::npos) {
            server = url.substr(n + 1, to - n - 1);
            filepath = url.substr(to);
        } else {
            server = url.substr(n + 1);
            filepath = "/";
        }
    } else {
        server = url;
        filepath = "/";
    }
}

void Http::connect(SOCKET *socket, std::string& host) {
    hostent *hostent;
    unsigned addr;
    sockaddr_in server{};

    *socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (*socket == INVALID_SOCKET) {
        return;
    }

    if(inet_addr(host.c_str()) == INADDR_NONE) {
        hostent=gethostbyname(host.c_str());
    } else {
        addr=inet_addr(host.c_str());
        hostent=gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);
    }

    if(hostent == nullptr) {
        closesocket(*socket);
        return;
    }

    server.sin_addr.s_addr=*((ulong*)hostent->h_addr);
    server.sin_family=AF_INET;
    server.sin_port=htons(HTTP_PORT);
    if(::connect(*socket, (struct sockaddr*)&server, sizeof(server))) {
        closesocket(*socket);
        return;
    }
}

ulong Http::get_header_length(const std::string &content) {
    char *findPos;
    ulong ofset = -1;

    findPos = strstr((char*) content.c_str(), HTTP_DELIM1);
    if (findPos != nullptr) {
        ofset = findPos - content.c_str();
        ofset += strlen(HTTP_DELIM1);
    }
    else {
        findPos = strstr((char*) content.c_str(), HTTP_DELIM2);
        if (findPos != nullptr) {
            ofset = findPos - content.c_str();
            ofset += strlen(HTTP_DELIM2);
        }
    }
    return ofset;
}

std::string Http::read(std::string& url) {
    char buffer[BUFFER_SIZE];
    SOCKET socket;
    std::string server, filepath;

    parse_url(url, server, filepath);
    connect(&socket, server);
    if (socket == INVALID_SOCKET) {

        return "";
    }
    std::string request = "GET " + filepath + " HTTP/1.0\r\nHost: " + server + "\r\n\r\n";
    send(socket, request.c_str(), (int) request.size(), 0);

    long total_read = 0, curr_read;
    std::string total;
    while ((curr_read = recv (socket, buffer, BUFFER_SIZE, 0)) > 0) {
        total += std::string(buffer, curr_read);
        total_read += curr_read;
    }
    closesocket(socket);
    ulong header_len = get_header_length(total);
    return total.substr(header_len < total.size() ? header_len : 0);
}