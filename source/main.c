// Include the most common headers from the C standard library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Include the main libnx system header, for Switch development
#include <switch.h>

#include <curl/curl.h>

// Main program entrypoint
int main(int argc, char *argv[])
{
    consoleInit(NULL);

    // Configure our supported input layout: a single player with standard controller styles
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);

    // Initialize the default gamepad (which reads handheld mode inputs as well as the first connected controller)
    PadState pad;
    padInitializeDefault(&pad);

    socketInitializeDefault();

    printf("Crudeferns patches.ini updater v1.0\n");

    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    FILE *patches;
    patches = fopen("/bootloader/patches.ini", "w");

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "https://dardel.codeberg.page/nxdumpguide/assets/patches.ini");
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Crudeferns patch updater/1.0");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, patches);
        // Add any other options here.

        consoleUpdate(NULL);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        else
            printf("patches updated");
        curl_easy_cleanup(curl);
    }

    fclose(patches);
    curl_global_cleanup();

    printf("\npress + to exit");

    while (appletMainLoop())
    {
        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);
        if (kDown & HidNpadButton_Plus)
            break;
        consoleUpdate(NULL);
    }

    // Deinitialize and clean up resources used by the console (important!)
    socketExit();
    consoleExit(NULL);
    return 0;
}
