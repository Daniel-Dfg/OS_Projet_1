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
    bool manual = false;
    ExceptionHandler::process_args(argc, argv, bot, manual);
    //il est probable qu'on devra regrouper les argv dans une ou plusieurs classe(s), genre 'ChatHandler'.
    //On peut aussi penser à d'autres classes/méthodes spécifiques à chaque message, genre 'display(bool is_formatted=true/false)'

    string user1_name = argv[1];
    string user2_name = argv[2];

    ChatHandler chat = ChatHandler(user1_name, user2_name, bot, manual);

    // Speration en deux processus
    int process = fork();

    // Communication avec deux processus (Original: envoi de messages, Secondaire: réception de messages)
    // Il faut avoir 2 terminaux (terminal1: ./chat A B, terminal2: ./chat B A par ex.)
    if (process > 0) { // Père
        signal(SIGINT, signal_handler);
        chat.access_sending_channel(user2_name);
    } else { // Fils
        chat.access_reception_channel(user2_name);
    }
    return 0;
}
