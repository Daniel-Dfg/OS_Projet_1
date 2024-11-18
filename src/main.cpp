#include "exception_handler.hpp"
#include "chat_handler.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <sys/shm.h> //TODO => avant fork
#include <sys/stat.h>
#include <sys/types.h>

void signal_handler(const int sig){ //TODO : bouger ceci à un endroit pertinent
    if (sig == 0){
        //...
    }
    exit(0);
}

int main(int argc, char* argv[]) {
    bool bot = false;
    bool manuel = false;
    ExceptionHandler::process_args(argc, argv, bot, manuel);

    std::string* user1_name = new std::string(argv[1]);
    std::string* user2_name = new std::string(argv[2]);

    ChatHandler chat = ChatHandler(*user1_name, *user2_name, bot, manuel);

    // Séparation en deux processus
    int process = fork();

    // Communication avec deux processus (Original: envoi de messages, Secondaire: réception de messages)
    // Il faut avoir 2 terminaux (terminal1: ./chat A B, terminal2: ./chat B A par ex.)
    if (process > 0) { // Père
        signal(SIGINT, signal_handler);
        chat.access_sending_channel(*user2_name);
    } else { // Fils
        chat.access_reception_channel(*user2_name);
    }

    delete user1_name;
    delete user2_name;

    return 0;
}
