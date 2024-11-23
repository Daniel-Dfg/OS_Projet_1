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
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstring>
#include <iostream> 
#include <memory>
#include "exception_handler.hpp"
#include <fcntl.h>

extern volatile sig_atomic_t signal_received;
extern std::string g_path_from_user1;
extern std::string g_path_from_user2;
extern int g_file_desc1;
extern int g_file_desc2;
extern pid_t process;

const size_t SHARED_MEMORY_SIZE = 4096;

struct SharedMemoryQueue{
    size_t total_chars = 0;
    char messages[SHARED_MEMORY_SIZE];
};

class ChatHandler{
    private:
    //std::queue<string>* pending_messages = nullptr;
    size_t pending_bytes = 0;
    static const mode_t FIFO_PERMISSION = 0666;
    static const mode_t FOLDER_PERMISSION = 0777;
    static const short unsigned int BUFFER_SIZE = 1024;
    
    string error_log; //utilisé pour stocker une éventuelle erreur dans le chat
    int exit_code = 0;

    const string user1_name, user2_name;
    string path_from_user1, path_from_user2;
    int file_desc1, file_desc2;

    SharedMemoryQueue* shared_memory_queue = nullptr;
    SharedMemoryQueue* init_shared_memory_block();

    int send_message(char (&thing)[BUFFER_SIZE]); //renvoie : nombre de bytes envoyés si tout s'est bien passé, -1 sinon.
    int receive_message(char (&received_message)[BUFFER_SIZE]); //même idée que pour send_message

    public:
    const bool bot, manuel;
    ChatHandler(const string &username1_, const string &username2_, const bool &bot_, const bool &manuel_);
    ~ChatHandler();
    void access_sending_channel(const string &recipient);
    void access_reception_channel(const string &sender);

    //
    void display_pending_messages();
    void add_message_to_shared_memory(const string& formatted_message);
    
    //
};


void Signal_Handler(const int sig);
extern ChatHandler* g_chat_handler; // I was kinda obligated to do this..

#endif // CHAT_HANDLER_HPP
