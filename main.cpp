#include <memory>
#include <csignal>

#include <liveMedia.hh>
#include <BasicUsageEnvironment.hh>

char quit = 0;
TaskScheduler *scheduler = nullptr;
UsageEnvironment *env = nullptr;

int main(int argc, char** argv) {
    // Begin by setting up our usage environment:
    scheduler = BasicTaskScheduler::createNew();
    env = BasicUsageEnvironment::createNew(*scheduler);

    UserAuthenticationDatabase* authDB = nullptr;
#ifdef ACCESS_CONTROL
    // To implement client access control to the RTSP server, do the following:
  authDB = new UserAuthenticationDatabase;
  authDB->addUserRecord("username1", "password1"); // replace these with real strings
  // Repeat the above with each <username>, <password> that you wish to allow
  // access to the server.
#endif

    // Create the RTSP server:
    RTSPServer* rtspServer = RTSPServer::createNew(*env, 8554, authDB);
    if (rtspServer == nullptr) {
        printf("Failed to create RTSP server: %s\n", env->getResultMsg());
        exit(1);
    }

    char const* descriptionString = "Session streamed by \"testOnDemandRTSPServer\"";

    {
        // To make the second and subsequent client for each stream reuse the same
        // input stream as the first client (rather than playing the file from the
        // start for each client), change the following "False" to "True":
        Boolean reuseFirstSource = False;

        char const* streamName = "h264ESVideoTest";
        char const* inputFileName = "test.264";
        ServerMediaSession* sms = ServerMediaSession::createNew(*env, streamName, streamName, descriptionString);
        sms->addSubsession(H264VideoFileServerMediaSubsession::createNew(*env, inputFileName, reuseFirstSource));
        rtspServer->addServerMediaSession(sms);

        std::unique_ptr<char[]> url(rtspServer->rtspURL(sms));
        printf("%s stream, from the file: %s. URL: %s\n", streamName, inputFileName, url.get());
    }

    if (rtspServer->setUpTunnelingOverHTTP(80) ||
        rtspServer->setUpTunnelingOverHTTP(8000) ||
        rtspServer->setUpTunnelingOverHTTP(8080))
    {
        printf("Use port: %d for RTSP-over-HTTP tunneling\n", rtspServer->httpServerPortNum());
    } else {
        printf("RTSP-over-HTTP tunneling is not available\n");
    }

    std::signal(SIGUSR1, [](int){
        quit = 1;
        env->reclaim();
        delete scheduler;
    });

    env->taskScheduler().doEventLoop(&quit); // does not return

    printf("Quit from live555 event loop\n\n");

    return 0; // only to prevent compiler warning
}