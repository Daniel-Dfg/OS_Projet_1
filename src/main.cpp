#include "exception_handler.hpp"
#include "chat_handler.hpp"

#include <signal.h>
#include <unistd.h>

//using namespace ChatGlobals;
ChatHandler* ChatHandler::current_instance = nullptr;  // Initialize to nullptr

int main(int argc, char* argv[]) {

    bool bot = false;
    bool manuel = false;

    ExceptionHandler::process_args(argc, argv, bot, manuel);

    
    std::string* user1_name = new std::string(argv[1]);
    std::string* user2_name = new std::string(argv[2]);
    
    /*
    std::string user1_name = argv[1];
    std::string user2_name = argv[2];
    */

    ChatHandler chat_handler(*user1_name, *user2_name, bot, manuel);
    //ChatHandler chat = ChatHandler(*user1_name, *user2_name, bot, manuel);
    //ChatGlobals::g_chat_handler = new ChatHandler(*user1_name, *user2_name, bot, manuel);
    //ChatGlobals::g_chat_handler = &chat;

    // Séparation en deux processus
    pid_t process = fork();
    // Communication avec deux processus (Original: envoi de messages, Secondaire: réception de messages)

    if (process > 0) { // Père
        signal(SIGINT, Signal_Handler);
        //ChatGlobals::g_chat_handler->access_sending_channel(*user2_name);
        chat_handler.access_sending_channel(*user2_name);
    } else { // Fils
        signal(SIGINT, SIG_IGN);
        //ChatGlobals::g_chat_handler->access_reception_channel(*user2_name);
        chat_handler.access_reception_channel(*user2_name);
    }
    //delete ChatGlobals::g_chat_handler;
    delete user1_name;
    delete user2_name;

    return 0;
}
