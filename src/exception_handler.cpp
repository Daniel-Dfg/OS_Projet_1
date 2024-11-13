#include "exception_handler.hpp"
const unordered_map<ExceptionHandler::ExitCodes, string> ExceptionHandler::EXIT_CODE_MESSAGES = {
    {LACKING_USERNAME, "chat pseudo_utilisateur pseudo_destinataire [--bot] [--manuel]"},
    {USERNAME_TOO_LONG, "Un nom d'utilisateur est trop long (> 30 )"},
    {INVALID_USERNAME, "Un nom d'utilisateur ne peut contenir un caractère interdit ('[', ']', '-', '/'), être ('.', '..'), ou être le même que le nom d'un autre utilisateur"}
    //etc...
};

void ExceptionHandler::display_error_and_exit(const ExitCodes &exit_code){ //devrait prendre args en paramètre pour les pseudos avec caractères invalides
    auto it = EXIT_CODE_MESSAGES.find(exit_code);

    if(it == EXIT_CODE_MESSAGES.end()){
         cerr << "Erreur inconnue, code " << exit_code << endl;
     }
    cerr << it->second << endl;
    exit(exit_code);
};

void ExceptionHandler::display_warning(string &&warning_message){
    //cette fonction existe car elle pourra (voire devra) être étendue,
    //pour tous les warnings qu'on pourrait afficher (on pourrait limite créer une classe juste pour ça)
    cout << "WARNING : " << warning_message << endl;
    }

void ExceptionHandler::check_username_validity(const string &username1, const string &username2){
    const vector<string> usernames = {username1, username2};
    for (const string &username : usernames){
        //les 3 trucs en-dessous sont peut-être à redéfinir comme des constantes globales ?
        const unordered_set<char> invalid_chars = {'[', ']', '-', '/'};
        const short unsigned int username_size_limit = 30;
        const unordered_set<string> invalid_names = {"..", "."};
        if (username.empty()){
            display_error_and_exit(LACKING_USERNAME);
        }
        if(username.size() > username_size_limit){
            display_error_and_exit(USERNAME_TOO_LONG);
        }
        for (char c : username){
            if(invalid_chars.find(c) != invalid_chars.end()){
                display_error_and_exit(INVALID_USERNAME);
            }
        }
        for (string name: invalid_names){
            if(name == username){
                display_error_and_exit(INVALID_USERNAME);
            }
        }
    }
    if(username1 == username2){
        display_error_and_exit(INVALID_USERNAME);
    }
}

void ExceptionHandler::return_code_check(int error) { // TODO a changé
    // Error si "error" est -1
    if (error < 0) {
        std::cout << "error\n";
        exit(1);
    }
}

void ExceptionHandler::process_args(const int argc, char* argv[], bool &bot, bool &manual) {
    if (argc < 3) {
        display_error_and_exit(LACKING_USERNAME);
    }
    std::vector<std::string> usernames = {argv[1], argv[2]};
    check_username_validity(usernames[0], usernames[1]);

    if (argc > 5) {
        display_warning("Trop d'arguments passés (plus de 4)");
    }

    for (int i = 3; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--bot" || arg == "--manuel") {
            if ((bot && arg == "--bot") || (manual && arg == "--manuel")) {
                display_warning("Un même argument a été écrit deux fois (" + arg + ")");
            } else {
                arg == "--bot" ? bot = true : manual = true;
            }
        }
    }
}
