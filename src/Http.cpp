#include "Http.hpp"

std::string Http::strhash(const std::string& s) {
    unsigned hash {};
    for (char c : s) {
        hash = (hash << 31) + c;
    }
    std::stringstream ss;
    ss << std::hex << std::setw(8) << std::setfill('0') << hash;
    return ss.str();
}

std::string Http::get(const std::string& url, bool save) {
    WSADATA wsaData {};
    if ( WSAStartup(REQUIRED_VERSION, &wsaData) != 0) {
        return "";
    }

    std::string content = read(const_cast<std::string&>(url));
    WSACleanup();
    if (save) {
        std::hash<std::string> hash;
        std::ofstream ofs{"../" + strhash(url) +".html"}; //todo: generate random file name!
        ofs << content;
    }
    return content;
}


void Http::parse_url(std::string& url, std::string &server, std::string &filepath) {
    if (url.starts_with(HTTP_PREFIX)) {
        url = url.erase(0,7);
    } else if (url.starts_with(HTTPS_PREFIX)) {
        url = url.erase(0,8);
    }
    int slash = url.find('/');
    if (slash != std::string::npos) {
        filepath = url.substr(slash);
        server = url.substr(0, url.size() - filepath.size());
    } else {
        filepath = "/";
        server = url;
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
    char buffer[BUFFER_SIZE*5];
    SOCKET socket;
    std::string server, filepath;

    parse_url(url, server, filepath);
    connect(&socket, server);
    if (socket == INVALID_SOCKET) {

        return "";
    }
    std::string request = "GET " + filepath + " HTTP/1.1\r\nHost: " + server + "\r\nUser-Agent: Mozilla/5.0\r\nAccept: text/html\r\n\r\n";
    send(socket, request.c_str(), (int) request.size(), 0);
    long total_read = 0, curr_read;
    std::string total;
    while ((curr_read = recv (socket, buffer, BUFFER_SIZE*5, 0)) > 0) {
        total += std::string(buffer,curr_read);
        total_read += curr_read;
        if (total.ends_with(HTTP_DELIM1) || total.ends_with(HTTP_DELIM2)) break;
    }
    closesocket(socket);
    ulong header_len = get_header_length(total);
    return total.substr(header_len < total.size() ? header_len : 0);
}