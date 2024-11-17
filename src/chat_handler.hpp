#ifndef CHAT_HANDLER_HPP
#define CHAT_HANDLER_HPP
/*

La classe chat est censée :
- Gérer la création du chat (créer et gérer les folders et les pipes)
- Assurer la communication et la terminaison des chats

Les signaux qui en découlent sont gérés dans une fonction à adapter TODO
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
    static const mode_t FIFO_PERMISSION = 0666;
    static const mode_t FOLDER_PERMISSION = 0777;
    static const short unsigned int BUFFER_SIZE = 1024;

    static const string EXIT_KEYWORD;

    string error_log; //utilisé pour stocker une éventuelle erreur dans le chat
    int exit_code = 0;

    const string user1_name, user2_name;
    string path_from_user1, path_from_user2;
    int file_desc1, file_desc2;

    const bool bot, manual;

    int send_message(char (&thing)[BUFFER_SIZE]); //renvoie : nombre de bytes envoyés si tout s'est bien passé, -1 sinon.
    int receive_message(char (&received_message)[BUFFER_SIZE]); //même idée que pour send_message
    static inline void clear_current_line(){
        std::cout << "\x1b[1A" << "\x1b[2K" << std::flush;}
    public:
    ChatHandler(const string &username1_, const string &username2_, const bool &bot_, const bool &manual_);

    void access_sending_channel(const string &recipient);
    void access_reception_channel(const string &sender);
};

#endif // CHAT_HANDLER_HPP
