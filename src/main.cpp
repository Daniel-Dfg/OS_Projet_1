/* Name            Prénom         ULBID           Matricule
 * El Hussein      Abdalrahman    aelh0063        000596003
 * Ibrahim         Dogan          idog0003        000586544
 * Daniel          Defoing        ddef0003        000589910
*/
#include "exception_handler.hpp"
#include "chat_handler.hpp"

#include <cstdio>
#include <signal.h>
#include <unistd.h>

ChatHandler* ChatHandler::current_instance = nullptr;

int main(int argc, char* argv[]) {
    bool bot = false;
    bool manuel = false;

    ExceptionHandler::process_args(argc, argv, bot, manuel);


    std::string user1_name = argv[1];
    std::string user2_name = argv[2];

    ChatHandler chat_handler(user1_name, user2_name, bot, manuel);


    // Séparation en deux processus
    pid_t process = fork();
    chat_handler.pid = process;
    // Communication avec deux processus (Original: envoi de messages, Secondaire: réception de messages)
    if (process > 0) { // Père
        signal(SIGTERM, Signal_Handler);
        signal(SIGINT, Signal_Handler);
        chat_handler.access_sending_channel(user2_name);
    } else { // Fils
        signal(SIGINT, SIG_IGN);
        chat_handler.access_reception_channel(user2_name);
        kill(getppid(), SIGTERM);
    }
    return 0;
}
