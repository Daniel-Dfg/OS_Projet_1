#include <iostream>
#include <ostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

using std::cout, std::string, std::cerr, std::unordered_map, std::endl, std::unordered_set;
//TODO : à voir si on met le namespace dans un hpp, et si on rajoute des fichiers. Il faudra relire les consignes à ce sujet.

namespace ExceptionHandler{
    //Utilisation d'un namespace : facile d'accès et d'utilisation, flexible. Pourrait être remplacé par une classe plus tard, au besoin.
    enum ExitCodes{
        VALID = 0,
        LACKING_USERNAME = 1,
        USERNAME_TOO_LONG = 2,
        INVALID_CHAR_IN_USERNAME = 3,
        /*
            ↓↓↓ Est correct selon les consignes???
        */
        //RESERVED_EXPRESSION_USERNAME = 4 //Si l'utilisateur s'appelle '.', '..', etc.
    };

    string error_color = "\x1B[31m";
    string end_color = "\x1B[0m";

    //On va probablement devoir raffiner ces exceptions, en disant par exemple lequel des deux usernames présentés est fautif.
    const unordered_map<ExceptionHandler::ExitCodes, string> EXIT_CODE_MESSAGES = {
        {LACKING_USERNAME, error_color+"chat pseudo_utilisateur pseudo_destinataire [--bot] [--manuel]"+end_color},
        {USERNAME_TOO_LONG, error_color+"Un nom d'utilisateur est trop long (> 30 )"+end_color},
        {INVALID_CHAR_IN_USERNAME, error_color+"Un nom d'utilisateur ne peut contenir un caractère interdit ('[', ']', '-', '/') or étre ('.', '..')"+end_color},
        //{RESERVED_EXPRESSION_USERNAME, error_color+"Un nom d'utilisateur ne peut être un mot réservé (comme '.' ou '..')"+end_color}
        //etc...
    };

    //cette fonction n'étant pour l'instant pas encore utilisée hors de ce namespace, on pourrait penser à adapter le namespace
    //en classe pour permettre de poser cette méthode comme private. Mais ceci est encore à discuter.
     void display_error_and_exit(const ExceptionHandler::ExitCodes &exit_code){ //devrait prendre args en paramètre pour les pseudos avec caractères invalides
        auto it = EXIT_CODE_MESSAGES.find(exit_code);

        /*
        TODO it != EXIT_CODE_MESSAGES.end() is not correct
        */

        // if(it != EXIT_CODE_MESSAGES.end()){
        //     cerr << "Erreur inconnue, code " << exit_code << endl;
        // }
        cerr << it->second << endl;
        exit(exit_code);
    };
    void check_username_validity(const string &username){
        //les 3 trucs en-dessous sont peut-être à redéfinir comme des constantes globales ?
        const unordered_set<char> invalid_chars = {'[', ']', '-', '/'};
        const short int username_size_limit = 30;
        unordered_set<string> invalid_names = {"..", "."};
        if (username.empty()){
            display_error_and_exit(LACKING_USERNAME);
        }
        if(username.size() > username_size_limit){
            display_error_and_exit(USERNAME_TOO_LONG);
        }
        for (char c : username){
            if(invalid_chars.find(c) != invalid_chars.end()){
                display_error_and_exit(INVALID_CHAR_IN_USERNAME);
            }
        }
        for (string name: invalid_names){
            if(name == username){
                display_error_and_exit(INVALID_CHAR_IN_USERNAME);
            }
        }
    };
}

void display_warning(string &&warning_message){
    //cette fonction existe car elle pourra (voire devra) être étendue,
    //pour tous les warnings qu'on pourrait afficher (on pourrait même créer une classe juste pour ça)
    cout << "WARNING : " << warning_message << endl;
}


int main(int argc, char* argv[]) {
    if (argc < 3){
        ExceptionHandler::display_error_and_exit(ExceptionHandler::LACKING_USERNAME);
    }

    //il est probable qu'on devra regrouper les argv dans une ou plusieurs classe(s), genre 'ChatHandler'.
    //On peut aussi penser à d'autres classes/méthodes spécifiques à chaque message, genre 'display(bool is_formatted=true/false)'
    ExceptionHandler::check_username_validity(argv[1]);
    ExceptionHandler::check_username_validity(argv[2]);
    string user1_name = argv[1];
    string user2_name = argv[2];
    bool bot_arg_written = false;
    bool manual_arg_written = false;
    //...

    if (argc > 5){
        display_warning("Trop d'arguments passés (plus de 4)");
    }

    for (int i = 3; i < argc; i++){
        string arg = argv[i];
        if (arg == "--bot" || arg == "--manuel"){
            if ((bot_arg_written && arg == "--bot") || (manual_arg_written && arg == "--manuel")){
                display_warning("Un même argument a été écrit deux fois (" + arg + ")");
            }
            else {
                arg == "--bot" ? bot_arg_written = true : manual_arg_written = true;
            }
        }
    }

   return 0;
}