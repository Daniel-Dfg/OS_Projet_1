#ifndef CHAT_HANDLER_HPP
#define CHAT_HANDLER_HPP
/*

La classe chat est censée :
- Gérer la création du chat (créer et gérer les folders et les pipes)
- Assurer la communication et la terminaison des chats

Les signaux qui en découlent sont gérés dans une fonction à adapter TODO

Probablement à séparer en d'autres sous-classes
*/

#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <sys/shm.h> //TODO => avant fork
#include <sys/stat.h>
#include <sys/types.h>
#include "exception_handler.hpp"


class ChatHandler{
    private:
    const mode_t FIFO_PERMISSION = 0666;
    const mode_t FOLDER_PERMISSION = 0777;

    const string user1_name, user2_name;
    string path_from_user1, path_from_user2;
    int file_desc1, file_desc2;

    const bool bot, manual;
    public:
    ChatHandler(const string &username1_, const string &username2_, const bool &bot_, const bool &manual_);

    //TODO mieux encapsuler tout cela
    void send_message_to(const std::string &recipient);
    void receive_message_from(const std::string &sender);
};

#endif // CHAT_HANDLER_HPP
