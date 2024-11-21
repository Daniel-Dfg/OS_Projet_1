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

ChatHandler* g_chat_handler = nullptr; // I was kinda obligated to do this..

void signal_handler(const int sig){
    if (sig == SIGINT){
        printf("Exiting now... sigint\n");
        if (g_chat_handler && g_chat_handler->manuel){
            g_chat_handler->display_pending_messages();
        }
        // Close the fifo channels before removing the files
        close(g_file_desc1);
        close(g_file_desc2);
        std::remove(g_path_from_user1.c_str());
        std::remove(g_path_from_user2.c_str());
        exit(4);    
    }
    else if (sig == SIGPIPE){
        printf("The other user closed the chat. pipesig\n");
        if (g_chat_handler && g_chat_handler->manuel){
            g_chat_handler->display_pending_messages();
        }

        exit(4);
    }
}
int main(int argc, char* argv[]) {

    bool bot = false;
    bool manuel = false;

    ExceptionHandler::process_args(argc, argv, bot, manuel);
    
    std::string* user1_name = new std::string(argv[1]);
    std::string* user2_name = new std::string(argv[2]);

    //ChatHandler chat = ChatHandler(*user1_name, *user2_name, bot, manuel);
    g_chat_handler = new ChatHandler(*user1_name, *user2_name, bot, manuel);

    signal(SIGPIPE, signal_handler);

    // Séparation en deux processus
    int process = fork();
    // Communication avec deux processus (Original: envoi de messages, Secondaire: réception de messages)
    // Il faut avoir 2 terminaux (terminal1: ./chat A B, terminal2: ./chat B A par ex.)
    if (process > 0) { // Père
        signal(SIGINT, signal_handler);  
        g_chat_handler->access_sending_channel(*user2_name);
    } else { // Fils
        g_chat_handler->access_reception_channel(*user2_name);
    }

    delete g_chat_handler;
    delete user1_name;
    delete user2_name;

    return 0;
}
