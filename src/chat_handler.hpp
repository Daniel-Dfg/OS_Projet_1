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
//#include <sys/shm.h> //TODO => avant fork
#include<sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <queue>
#include "exception_handler.hpp"

extern std::string g_path_from_user1;
extern std::string g_path_from_user2;
extern int g_file_desc1;
extern int g_file_desc2;

class ChatHandler{
    private:
    std::queue<string> pending_messages;
    size_t pending_bytes = 0;
    static const mode_t FIFO_PERMISSION = 0666;
    static const mode_t FOLDER_PERMISSION = 0777;
    static const short unsigned int BUFFER_SIZE = 1024;

    static const string EXIT_KEYWORD;

    string error_log; //utilisé pour stocker une éventuelle erreur dans le chat
    int exit_code = 0;

    const string user1_name, user2_name;
    string path_from_user1, path_from_user2;
    int file_desc1, file_desc2;

    const bool bot, manuel;
    std::queue<string>* shared_memory_ptr;
    const size_t shared_memory_size = 4096;

    int send_message(char (&thing)[BUFFER_SIZE]); //renvoie : nombre de bytes envoyés si tout s'est bien passé, -1 sinon.
    int receive_message(char (&received_message)[BUFFER_SIZE]); //même idée que pour send_message
    void display_pending_messages();
    public:

    ChatHandler(const string &username1_, const string &username2_, const bool &bot_, const bool &manuel_);
    ~ChatHandler();
    void access_sending_channel(const string &recipient);
    void access_reception_channel(const string &sender);
    std::queue<std::string>* init_shared_memory_block();
    void remove_shared_memory_block();
};

#endif // CHAT_HANDLER_HPP
