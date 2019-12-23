#include <iostream>
#include <string>
#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>

#define DEFAULT_RTSP_PORT "8554"
#define DEFAULT_ENDPOINT "video"

int main(int argc, char** argv) {
    GMainLoop *loop;
    GstRTSPServer *server;
    GstRTSPMountPoints *mounts;
    GstRTSPMediaFactory *factory;
    GOptionContext *optctx;
    GError *error = NULL;

    struct {
        char *port = nullptr;
        char *endpoint = nullptr;
    } opts;

    GOptionEntry entries[] = {
        {"port", 'p', 0, G_OPTION_ARG_STRING, &opts.port, "Port to listen on (default: " DEFAULT_RTSP_PORT ")", "PORT"},
        {"endpoint", 'e', 0, G_OPTION_ARG_STRING, &opts.endpoint, "Endpoint name (default: " DEFAULT_ENDPOINT ")", "ENDPOINT"},
        {nullptr}
    };

    optctx = g_option_context_new("<launch line> - Simple RTSP server\n\n"
        "Sample pipeline: \"( videotestsrc ! x264enc ! rtph264pay name=pay0 pt=96 )\"");
    g_option_context_add_main_entries(optctx, entries, NULL);
    g_option_context_add_group(optctx, gst_init_get_option_group());
    if (!g_option_context_parse(optctx, &argc, &argv, &error)) {
        g_printerr("Error parsing options: %s", error->message);
        g_option_context_free(optctx);
        g_clear_error(&error);
        return -1;
    }

    if (nullptr == argv[1]) {
        g_printerr("Error: empty pipeline\n");
        return -1;
    }

    g_option_context_free(optctx);

    if (nullptr == opts.port) {
        g_print("Using default port: %s\n", DEFAULT_RTSP_PORT);
        opts.port = strdup(DEFAULT_RTSP_PORT);
    } else {
        g_print("Using port: %s\n", opts.port);
    }

    if (nullptr == opts.endpoint) {
        g_print("Using default endpoint: %s\n", DEFAULT_ENDPOINT);
        opts.endpoint = strdup(DEFAULT_ENDPOINT);
    } else {
        g_print("Using endpoint: %s\n", opts.endpoint);
    }

    loop = g_main_loop_new(NULL, FALSE);

    server = gst_rtsp_server_new();
    g_object_set(server, "service", opts.port, nullptr);
    mounts = gst_rtsp_server_get_mount_points(server);
    factory = gst_rtsp_media_factory_new();

    gst_rtsp_media_factory_set_launch(factory, argv[1]);
    gst_rtsp_media_factory_set_shared(factory, TRUE);

    g_print("Using pipeline (as parsed): %s\n", gst_rtsp_media_factory_get_launch(factory));
    std::string mount = "/";
    mount += opts.endpoint;
    gst_rtsp_mount_points_add_factory(mounts, mount.c_str(), factory);

    g_object_unref(mounts);

    gst_rtsp_server_attach(server, NULL);

    g_print("Stream ready at rtsp://127.0.0.1:%s/%s\n", opts.port, opts.endpoint);
    g_main_loop_run(loop);

    return 0;
}
