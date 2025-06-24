#ifndef AFLPLUGIN_H
#define AFLPLUGIN_H

#define PLUGINAPI extern "C" __declspec(dllexport)
#define PLUGIN_OK 0

typedef struct {
    char Name[64];
    char Vendor[64];
} PluginInfo;

typedef struct {
    int version;
    int reserved;
} RTPluginInfo;

typedef struct {
    char symbol[32];
    float last;
    float bid;
    float ask;
    float open;
    float high;
    float low;
    float close;
    float volume;
    unsigned int datetime;
} RTQuote;

#endif
