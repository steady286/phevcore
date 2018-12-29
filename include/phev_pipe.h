#ifndef _PHEV_PIPE_H_
#define _PHEV_PIPE_H_
#include <time.h>
#include "msg_core.h"
#include "msg_pipe.h"
#include "phev_core.h"

#define PHEV_PIPE_MAX_EVENT_HANDLERS 10

#ifndef PHEV_CONNECT_WAIT_TIME
#define PHEV_CONNECT_WAIT_TIME (1000)
#endif

#ifndef PHEV_CONNECT_MAX_RETRIES
#define PHEV_CONNECT_MAX_RETRIES (5)
#endif

#ifdef _WIN32
//  For Windows (32- and 64-bit)
#   include <windows.h>
#   define SLEEP(msecs) Sleep(msecs)
#elif __unix
//  For linux, OSX, and other unixes
#   define _POSIX_C_SOURCE 199309L // or greater
#   include <time.h>
#   define SLEEP(msecs) do {            \
        struct timespec ts;             \
        ts.tv_sec = msecs/1000;         \
        ts.tv_nsec = msecs%1000*1000;   \
        nanosleep(&ts, NULL);           \
        } while (0)
#else
#   error "Unknown system"
#endif

enum {
    PHEV_PIPE_GOT_VIN,
    PHEV_PIPE_AA_ACK,
    PHEV_PIPE_START_ACK,
    PHEV_PIPE_REGISTRATION,
    PHEV_PIPE_ECU_VERSION2,
    PHEV_PIPE_REMOTE_SECURTY_PRSNT_INFO,
    PHEV_PIPE_REG_DISP,
    PHEV_PIPE_MAX_REGISTRATIONS,
    PHEV_PIPE_REG_UPDATE_ACK,
};

typedef struct phevPipeEvent_t 
{
    int event;
    size_t length;
    void * data;
} phevPipeEvent_t;

typedef struct phevVinEvent_t
{
    char vin[18];
    uint8_t data;
    uint8_t registrations;
} phevVinEvent_t;

typedef struct phev_pipe_ctx_t phev_pipe_ctx_t;
typedef void phevError_t;
typedef int (* phevPipeEventHandler_t)(phev_pipe_ctx_t * ctx, phevPipeEvent_t * event);
typedef void (* phevErrorHandler_t)(phevError_t * error);

typedef struct phev_pipe_ctx_t {
    msg_pipe_ctx_t * pipe;
    phevPipeEventHandler_t eventHandler[PHEV_PIPE_MAX_EVENT_HANDLERS];
    int eventHandlers;
    phevErrorHandler_t errorHandler;
    time_t lastPingTime;
    uint8_t currentPing;
    bool connected;
    void * ctx;
} phev_pipe_ctx_t;

typedef struct phev_pipe_settings_t {
    messagingClient_t * in;
    messagingClient_t * out;
    msg_pipe_splitter_t inputSplitter;
    msg_pipe_splitter_t outputSplitter;
    msg_pipe_responder_t inputResponder;
    msg_pipe_responder_t outputResponder;
    msg_pipe_transformer_t outputInputTransformer;
    msg_pipe_transformer_t outputOutputTransformer;
    msg_pipe_connectHook_t preConnectHook;
    phevErrorHandler_t errorHandler;
    void * ctx;
} phev_pipe_settings_t;

void phev_pipe_loop(phev_pipe_ctx_t *);
phev_pipe_ctx_t * phev_pipe_create(messagingClient_t * in, messagingClient_t * out);
phev_pipe_ctx_t * phev_pipe_createPipe(phev_pipe_settings_t);
void phev_pipe_waitForConnection(phev_pipe_ctx_t * ctx);
message_t * phev_pipe_outputChainInputTransformer(void *, message_t *);
message_t * phev_pipe_outputEventTransformer(void *, message_t *);
void phev_pipe_registerEventHandler(phev_pipe_ctx_t *, phevPipeEventHandler_t);
void phev_pipe_deregisterEventHandler(phev_pipe_ctx_t *, phevPipeEventHandler_t);
message_t * phev_pipe_commandResponder(void *, message_t *);
messageBundle_t * phev_pipe_outputSplitter(void *, message_t *);
void phev_pipe_ping(phev_pipe_ctx_t *);
void phev_pipe_resetPing(phev_pipe_ctx_t *);
void phev_pipe_start(phev_pipe_ctx_t * ctx, uint8_t * mac);
void phev_pipe_sendMac(phev_pipe_ctx_t * ctx, uint8_t * mac);
void phev_pipe_updateRegister(phev_pipe_ctx_t *, const uint8_t, const uint8_t);
phevPipeEvent_t * phev_pipe_createRegisterEvent(phev_pipe_ctx_t * phevCtx, phevMessage_t * phevMessage);

//void phev_pipe_sendCommand(phev_core_command_t);

#endif