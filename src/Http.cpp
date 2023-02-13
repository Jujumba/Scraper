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


void Http::parse_url(std::string& mUrl, std::string &serverName, std::string &filepath, std::string &filename) {
    std::string::size_type n;
    std::string url = mUrl;

    if (url.substr(0,7) == "http://")
        url.erase(0,7);

    if (url.substr(0,8) == "https://")
        url.erase(0,8);

    n = url.find('/');
    if (n != std::string::npos) {
        serverName = url.substr(0,n);
        filepath = url.substr(n);
        n = filepath.rfind('/');
        filename = filepath.substr(n+1);
    } else {
        serverName = url;
        filepath = "/";
        filename = "";
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
    const char *srchStr1 = "\r\n\r\n", *srchStr2 = "\n\r\n\r";
    char *findPos;
    long ofset = -1;

    findPos = strstr((char*) content.c_str(), srchStr1);
    if (findPos != nullptr) {
        ofset = findPos - content.c_str();
        ofset += strlen(srchStr1);
    }
    else {
        findPos = strstr((char*)content.c_str(), srchStr2);
        if (findPos != nullptr) {
            ofset = findPos - content.c_str();
            ofset += strlen(srchStr2);
        }
    }
    return ofset;
}

std::string Http::read(std::string& url) {
    char buffer[BUFFER_SIZE];
    SOCKET socket;
    std::string server, filepath, filename;

    parse_url(url, server, filepath, filename);

    connect(&socket, server);
    if (socket == INVALID_SOCKET) return "";
    std::string superreq = "GET "+filepath+" HTTP/1.0\r\nHost: " +server+"\r\n\r\n";
    send(socket, superreq.c_str(), superreq.size(), 0);

    long totalBytesRead = 0, thisReadSize = 0;
    std::string total;
    while ((thisReadSize = recv (socket, buffer, BUFFER_SIZE, 0)) > 0) {
        total += std::string(buffer, thisReadSize);
        totalBytesRead += thisReadSize;
    }

    closesocket(socket);
    return total.substr(this->get_header_length(total));
}