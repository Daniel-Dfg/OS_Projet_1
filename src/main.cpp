#include "exception_handler.hpp"
#include "chat_handler.hpp"

#include <iostream>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <sys/shm.h> //TODO => avant fork
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdio>
#include <csignal>

pid_t process;

int main(int argc, char* argv[]) {

    bool bot = false;
    bool manuel = false;

    ExceptionHandler::process_args(argc, argv, bot, manuel);
    
    
    struct sigaction action;
    action.sa_handler = Signal_Handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    
    
    if(sigaction(SIGPIPE, &action, NULL) < 0){
        perror("sigaction() sigpipe");
    }
    


    std::string* user1_name = new std::string(argv[1]);
    std::string* user2_name = new std::string(argv[2]);

    //ChatHandler chat = ChatHandler(*user1_name, *user2_name, bot, manuel);
    g_chat_handler = new ChatHandler(*user1_name, *user2_name, bot, manuel);
    
    //signal(SIGPIPE, Signal_Handler);

    // Séparation en deux processus
    process = fork();
    // Communication avec deux processus (Original: envoi de messages, Secondaire: réception de messages)
    // Il faut avoir 2 terminaux (terminal1: ./chat A B, terminal2: ./chat B A par ex.)
    if (process > 0) { // Père
        if(sigaction(SIGINT, &action, NULL) < 0){
            perror("sigaction()");
        }
        g_chat_handler->access_sending_channel(*user2_name);
    } else { // Fils
        
        //signal(SIGINT, SIG_IGN); // SIGINT ignore par le second processus
        g_chat_handler->access_reception_channel(*user2_name);
    }

    delete g_chat_handler;
    delete user1_name;
    delete user2_name;

    return 0;
}
