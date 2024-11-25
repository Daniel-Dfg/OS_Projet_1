#include "chat_handler.hpp"
#include "exception_handler.hpp"


#include <cstdio>
#include <fcntl.h>
#include <iostream>
#include <cstring>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <csignal>

std::string g_path_from_user1;
std::string g_path_from_user2;
int g_file_desc1;
int g_file_desc2;
ChatHandler* g_chat_handler = nullptr;

ChatHandler::ChatHandler(const string &username1_, const string &username2_, const bool &bot_, const bool &manuel_)
    : user1_name{username1_}, user2_name{username2_}, bot{bot_}, manuel{manuel_} {
    // Tout initialiser
    DIR *temp = opendir("temp");
    if (temp) {
        ExceptionHandler::return_code_check(closedir(temp));
    } else {
        ExceptionHandler::return_code_check(mkdir("temp", FOLDER_PERMISSION));
    }

    // Initialize FIFO paths
    path_from_user1 = "temp/" + user1_name + "_" + user2_name + ".chat";
    path_from_user2 = "temp/" + user2_name + "_" + user1_name + ".chat";
    g_path_from_user1 = path_from_user1;
    g_path_from_user2 = path_from_user2;

    // Initialize file descriptors
    file_desc1 = access(path_from_user1.c_str(), F_OK);
    file_desc2 = access(path_from_user2.c_str(), F_OK);
    g_file_desc1 = file_desc1;
    g_file_desc2 = file_desc2;

    if (manuel){
        shared_memory_queue = init_shared_memory_block();
    }
    // Create Named Pipes (FIFO)
    if (file_desc1 < 0) {
        ExceptionHandler::return_code_check(mkfifo(path_from_user1.c_str(), FIFO_PERMISSION));
    }
    if (file_desc2 < 0) {
        ExceptionHandler::return_code_check(mkfifo(path_from_user2.c_str(), FIFO_PERMISSION));
    }
}
void Signal_Handler(const int sig){
    if (sig == SIGINT){
        if (process > 0){
            std::cout << "You closed the chat. sigint pere" << std::endl;
            kill(process, SIGTERM);
            close(g_file_desc1);
            close(g_file_desc2);
            unlink(g_path_from_user1.c_str());
            unlink(g_path_from_user2.c_str());
            exit(4);
        }
        else if (process == 0){
        std::cout << "The other user closed the chat. Signal " << strsignal(sig) << " received." << std::endl;
        }
    }
    else if (sig == SIGPIPE) {
        if (process > 0){
        printf("Closing the chat.. sigpipe\n");
        if (g_chat_handler && g_chat_handler->manuel){
            g_chat_handler->display_pending_messages();
        }
        }
        // Close the fifo channels before removing the files
        exit(4);
    /*
    else if (sig == SIGPIPE){
        printf("The other user closed the chat. pipesig\n");
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
    else if (sig == SIGTERM){
        printf("SIGTERM\n");
        exit(4);
    }
    */
    }
}
void ChatHandler::access_sending_channel(const string &recipient) {
    string path = (recipient == user2_name) ? path_from_user1 : path_from_user2;
    string sender = (recipient == user2_name) ? user1_name : user2_name;
    char message_to_send[BUFFER_SIZE];
    int bytes_written;
    string ansi_beginning = bot ? "" : "\x1B[4m";
    string ansi_end = bot ? "" : "\x1B[0m";
    file_desc1 = open(path.c_str(), O_WRONLY);
    if (file_desc1 == -1) {
        perror("open");
        return; // Sortir de la fonction si l'ouverture a échoué
    }
    do {
        if (file_desc1 != -1) { //peut-être inclure le file_desc2 dans la condition, pour voir si le chat est toujours actif ?
            bytes_written = send_message(message_to_send);
            if (bytes_written > 0 && !bot) {
                printf("[%s%s%s] %s", ansi_beginning.c_str(), sender.c_str(), ansi_end.c_str(), message_to_send);
            }
            display_pending_messages();
        } else {
            cerr << "Descripteur de fichier invalide : impossible d'y écrire des informations" << endl;
            bytes_written = -1;
        }
    } while (bytes_written > 0);
    if (bytes_written < 0) {
        cerr << "Erreur en écriture dans le fichier: " << strerror(errno) << endl << this->error_log << endl;
    } else if (bytes_written == 0) {
        string end_user = (path == path_from_user1) ? user1_name : user2_name;
        this->error_log = "Conversation terminée par " + end_user;
        this->exit_code = EXIT_SUCCESS;
    }
    close(file_desc1);
    exit(this->exit_code);
}
void ChatHandler::access_reception_channel(const string &sender) {
    signal(SIGPIPE, Signal_Handler);
    string path = (sender == user2_name) ? path_from_user2 : path_from_user1;
    file_desc2 = open(path.c_str(), O_RDONLY);
    if (file_desc2 < 0) {
        cerr << "Error opening file: " << strerror(errno) << endl;
        exit(EXIT_FAILURE);
    }
    char received_message[BUFFER_SIZE];
    int bytes_read = 0;
    string ansi_beginning = bot ? "" : "\x1B[4m";
    string ansi_end = bot ? "" : "\x1B[0m";
    do {
        bytes_read = receive_message(received_message);
        if (bytes_read <= 0) {
            this->error_log = "Erreur de lecture";
            this->exit_code = EXIT_FAILURE;
            cerr << "Erreur en lecture dans le fichier: " << strerror(errno) << endl << this->error_log << endl;
            break;
        }
        else if (bytes_read > 0) {
            if (manuel && !bot) {
                string formatted_message;
                if (shared_memory_queue->first_message){
                    formatted_message = "[" + ansi_beginning + sender + ansi_end + "] " + received_message;
                    shared_memory_queue->first_message = false;
                } else{
                    formatted_message = received_message;
                }
                add_message_to_shared_memory(formatted_message);
                pending_bytes += bytes_read;
                std::cout<< "\a" << std::flush;
            } else {
                printf("[%s%s%s] %s", ansi_beginning.c_str(), sender.c_str(), ansi_end.c_str(), received_message);
                fflush(stdout);
            }
        }
    } while (bytes_read >= 0);

    if (bytes_read != 0) {
        this->error_log = "Problème de lecture !";
        this->exit_code = EXIT_FAILURE;
        cerr << "Problème de lecture !" << endl;
    } else {
        this->error_log = "";
        this->exit_code = EXIT_SUCCESS;
    }
    close(file_desc2);
    exit(this->exit_code);
}
int ChatHandler::send_message(char (&message_to_send)[BUFFER_SIZE]){
    //TODO : définir la taille exacte du message, et écrire cette quantité précisément :
    //de même, le receveur du message doit pouvoir déterminer combien de bytes il doit lire...
    /*
    struct sigaction action;
    action.sa_handler = Signal_Handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    if (sigaction(SIGINT, &action, NULL) < 0) {
        perror("sigaction() SIGINT");
    }
    if (sigaction(SIGTERM, &action, NULL) < 0) {
        perror("sigaction() SIGTERM");
    }
    */
    //if(FD_ISSET(STDIN_FILENO, &read_fds)){
    char* input = fgets(message_to_send, sizeof(message_to_send), stdin);
    if (input == NULL){
        if (feof(stdin)) {
            this->error_log = "End of input reached.";
            this->exit_code = EXIT_SUCCESS;
        } else if (ferror(stdin)) {
            if (errno == SIGPIPE || errno == SIGINT){
                std::cout << "Interrupted by signal, the other user closed the chat" << std::endl;
                this->error_log = "Interrupted by signal, the other user closed the chat.";
                this->exit_code = EXIT_FAILURE;
                exit(4);
            }else{
            this->error_log = "Error reading input: ";
            this->exit_code = EXIT_FAILURE;
            }
        }   
        return -1;
    }
    ssize_t bytes_written = write(file_desc1, message_to_send, strlen(message_to_send));
        if (bytes_written < 0){
            if (errno == EPIPE) {
                std::cerr << "Failed to send msg";
                kill(process, SIGPIPE);
                return -1;
            }
        }
    //this-> exit_code ? bytes_written = -1 : bytes_written;
    return static_cast<int>(bytes_written);
}
int ChatHandler::receive_message(char (&received_message)[BUFFER_SIZE]) {
    ssize_t bytes_read = read(file_desc2, received_message, sizeof(received_message) - 1);
    if (bytes_read < 0) {
        perror("read");
        return -1;
    }
    if (bytes_read == 0) {
        std::cerr << "The other user has disconnected. \n";
        kill(process, SIGPIPE);
        return -1;
        //exit(4); // Fin de la conversation
    }
    received_message[bytes_read] = '\0'; // Null-terminate the string
    return static_cast<int>(bytes_read);
}
void ChatHandler::display_pending_messages() {
    if(shared_memory_queue){
        std::cout.write(shared_memory_queue->messages, shared_memory_queue->total_chars);
        shared_memory_queue->total_chars = 0;
        shared_memory_queue->first_message = true;
    }
}
SharedMemoryQueue* ChatHandler::init_shared_memory_block(){
    // Autoriser les lectures et ecritures
    const int protection = PROT_READ | PROT_WRITE;
    // Partager avec son/ses enfants 
    const int visibility = MAP_SHARED | MAP_ANONYMOUS;
    // Le fichier pas utilise
    const int fd = -1;
    const int offset = 0;

    void* shared_memory_ptr = mmap(NULL, SHARED_MEMORY_SIZE, protection, visibility, fd, offset);

    if (shared_memory_ptr == MAP_FAILED){
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    return new (shared_memory_ptr) SharedMemoryQueue();  
}
ChatHandler::~ChatHandler(){
    // Removes the shared memory block
    if (manuel && shared_memory_queue){
        if (munmap(shared_memory_queue, SHARED_MEMORY_SIZE) == -1){
            perror("munmap");
        }
    }
    shared_memory_queue = nullptr;
}
void ChatHandler::add_message_to_shared_memory(const string& formatted_message){
    size_t message_size = formatted_message.size() + 1; // null terminator included

    if (shared_memory_queue->total_chars + message_size > SHARED_MEMORY_SIZE){
        display_pending_messages();
        shared_memory_queue->total_chars = 0;
    }
    std::memcpy(&shared_memory_queue->messages[shared_memory_queue->total_chars], formatted_message.c_str(), message_size);
    shared_memory_queue->total_chars += message_size;
}