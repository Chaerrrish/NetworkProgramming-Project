#ifndef HANGMAN_H
# define HANGMAN_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>


# define TOTAL_WORDS 100
# define BUFFER_SIZE 100
# define MAX_CLIENTS 10

char start[500] = "\nWelcome to Hangman Game !\n\nPlease Choose mode\n1. Play Game\n2. Exit\n";

char exit_msg[100] = "See you again~\n";
char lose_msg[100] = "\n\nYou died! Start a new game\n";
char win_msg[100] = "\n\nCongratulation. You win!\nDo you want to start a new game?\n\n";
char newgame_msg[100] = "1. Play\n2. Exit\n";
char answer_msg[100] = "The answer is ";
char hangman_stages[7][100] = {
    "  +---+\n"
    "  |   |\n"
    "      |\n"
    "      |\n"
    "      |\n"
    "      |\n"
    "=========\n",

    "  +---+\n"
    "  |   |\n"
    "  O   |\n"
    "      |\n"
    "      |\n"
    "      |\n"
    "=========\n",

    "  +---+\n"
    "  |   |\n"
    "  O   |\n"
    "  |   |\n"
    "      |\n"
    "      |\n"
    "=========\n",

    "  +---+\n"
    "  |   |\n"
    "  O   |\n"
    " /|   |\n"
    "      |\n"
    "      |\n"
    "=========\n",

    "  +---+\n"
    "  |   |\n"
    "  O   |\n"
    " /|\\  |\n"
    "      |\n"
    "      |\n"
    "=========\n",

    "  +---+\n"
    "  |   |\n"
    "  O   |\n"
    " /|\\  |\n"
    " /    |\n"
    "      |\n"
    "=========\n",

    "  +---+\n"
    "  |   |\n"
    "  X   |\n"
    " /|\\  |\n"
    " / \\  |\n"
    "      |\n"
    "=========\n"
};

char *word_list[100] = {
    "algorithm", "array", "binary", "bug", "byte",
    "cache", "class", "compiler", "constant", "constructor",
    "data", "debug", "declaration", "dependency", "dictionary",
    "dynamic", "encapsulation", "exception", "framework", "function",
    "git", "hash", "heap", "inheritance", "initialization",
    "instance", "interface", "iteration", "library", "linkedlist",
    "loop", "method", "module", "namespace", "object",
    "operator", "optimization", "parameter", "pointer", "polymorphism",
    "program", "protocol", "recursion", "reference", "repository",
    "runtime", "scope", "script", "stack", "statement",
    "string", "structure", "syntax", "template", "thread",
    "token", "tree", "tuple", "variable", "vector",
    "version", "virtual", "wrapper", "XML", "JSON",
    "API", "backend", "bit", "boolean", "bytecode",
    "callback", "character", "client", "closure", "command",
    "component", "compression", "concurrency", "configuration", "connection",
    "constructor", "container", "controller", "cookie", "CPU",
    "database", "default", "delegate", "dependency", "deployment",
    "descriptor", "developer", "development", "directive", "document"
};



#endif