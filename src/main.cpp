#include "exception_handler.hpp"
#include "chat_handler.hpp"

#include <signal.h>

using namespace ChatGlobals;

int main(int argc, char* argv[]) {

    bool bot = false;
    bool manuel = false;

    ExceptionHandler::process_args(argc, argv, bot, manuel);


    std::string* user1_name = new std::string(argv[1]);
    std::string* user2_name = new std::string(argv[2]);

    //ChatHandler chat = ChatHandler(*user1_name, *user2_name, bot, manuel);
    ChatGlobals::g_chat_handler = new ChatHandler(*user1_name, *user2_name, bot, manuel);


    // Séparation en deux processus
    pid_t process = fork();
    // Communication avec deux processus (Original: envoi de messages, Secondaire: réception de messages)

    if (process > 0) { // Père
        signal(SIGINT, Signal_Handler);
        ChatGlobals::g_chat_handler->access_sending_channel(*user2_name);

    } else { // Fils
        signal(SIGINT, SIG_IGN);
        ChatGlobals::g_chat_handler->access_reception_channel(*user2_name);
    }
    // Il ne sont meme pas pris en compte
    delete ChatGlobals::g_chat_handler;
    delete user1_name;
    delete user2_name;

    return 0;
}
