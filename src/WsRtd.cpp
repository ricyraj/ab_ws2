#include "../include/aflplugin.h"
#include <windows.h>
#include <thread>
#include <mutex>
#include <map>
#include <string>
#include <ctime>

std::map<std::string, float> latestPrices;
std::mutex mtx;

DWORD WINAPI PipeListenerThread(LPVOID) {
    while (true) {
        HANDLE hPipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\AbFeedPipe"),
            PIPE_ACCESS_INBOUND,
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,
            1024, 1024, 0, NULL);

        if (hPipe == INVALID_HANDLE_VALUE) continue;

        if (ConnectNamedPipe(hPipe, NULL) || GetLastError() == ERROR_PIPE_CONNECTED) {
            char buffer[1024];
            DWORD bytesRead;

            while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
                buffer[bytesRead] = '\0';
                std::string msg(buffer);

                size_t sep = msg.find(':');
                if (sep != std::string::npos) {
                    std::string symbol = msg.substr(0, sep);
                    float price = std::stof(msg.substr(sep + 1));

                    std::lock_guard<std::mutex> lock(mtx);
                    latestPrices[symbol] = price;
                }
            }
        }
        CloseHandle(hPipe);
    }
    return 0;
}

PLUGINAPI int GetPluginInfo(PluginInfo* info) {
    strcpy(info->Name, "WsRtdPipe");
    strcpy(info->Vendor, "RishiRT");
    return PLUGIN_OK;
}

PLUGINAPI int InitializeRT(int version, RTPluginInfo* info) {
    CreateThread(NULL, 0, PipeListenerThread, NULL, 0, NULL);
    return PLUGIN_OK;
}

PLUGINAPI int RT_GetQuotesEx(RTQuote* quotes, int maxsize) {
    std::lock_guard<std::mutex> lock(mtx);
    int count = 0;

    for (auto& [symbol, price] : latestPrices) {
        if (count >= maxsize) break;

        strncpy(quotes[count].symbol, symbol.c_str(), sizeof(quotes[count].symbol) - 1);
        quotes[count].symbol[sizeof(quotes[count].symbol) - 1] = '\0';

        quotes[count].last = price;
        quotes[count].bid = price - 0.05f;
        quotes[count].ask = price + 0.05f;
        quotes[count].open = price;
        quotes[count].high = price;
        quotes[count].low = price;
        quotes[count].close = price;
        quotes[count].volume = 100;
        quotes[count].datetime = std::time(nullptr);

        count++;
    }

    return count;
}
