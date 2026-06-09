// PASS, NICK, USER, QUIT, CAP

// Commands_connection.cpp

// CAP, PASS, NICK, USER, QUIT
// Attention : Client.hpp a _authenticated + _registered — il faudra les deux pour le welcome 001
// Pas de _password dans Client → tu devras le stocker temporairement, soit en ajoutant un champ dans Client, soit via une std::map<int, std::string> dans Server