#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include "stdio.h"
#include <iostream>
#include <sstream>
#include <string>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp> 

#define CW_GRAMMAR cwgrammar

#define RERUN_MODE

#define DEFAULT_INPUT_FILE "../base_test_programs_2025/file1.k03"

#define MAX_TEXT_SIZE 8192

namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;

#define SAME_RULE(RULE) ((RULE) | (RULE))

template <typename Iterator>
struct cwgrammar : qi::grammar<Iterator> {
    cwgrammar(std::ostringstream& error_stream) : cwgrammar::base_type(program), error_stream_(error_stream) {
        labeled_point = ident >> tokenCOLON;
        program_name = SAME_RULE(ident);
        value_type = SAME_RULE(tokenINTEGER16_t);
        declaration_element = ident >> -(tokenLEFTSQUAREBRACKETS >> unsigned_value >> tokenRIGHTSQUAREBRACKETS);
        other_declaration_ident = tokenCOMMA >> declaration_element;
        declaration = value_type >> declaration_element >> *other_declaration_ident;

        index_action = tokenLEFTSQUAREBRACKETS >> expression >> tokenRIGHTSQUAREBRACKETS;

        unary_operator = SAME_RULE(tokenNOT);
        unary_operation = unary_operator >> expression;
        binary_operator = tokenAND | tokenOR | tokenEQUAL | tokenNOTEQUAL | tokenLESSOREQUAL | tokenGREATEROREQUAL | tokenLESS | tokenPLUS | tokenMINUS | tokenMUL | tokenDIV | tokenMOD;
        binary_action = binary_operator >> expression;

        left_expression = group_expression | unary_operation | ident >> -index_action | value | cond_block__with_optionally_return_value;
        expression = left_expression >> *binary_action;

        group_expression = tokenGROUPEXPRESSIONBEGIN >> expression >> tokenGROUPEXPRESSIONEND;

        bind_left_to_right = expression >> tokenLRBIND >> ident >> -index_action;

        if_expression = SAME_RULE(expression);
        body_for_true__with_optionally_return_value = SAME_RULE(block_statements__with_optionally_return_value);
        false_cond_block_without_else__with_optionally_return_value = tokenELSE >> tokenIF >> if_expression >> body_for_true__with_optionally_return_value;
        body_for_false__with_optionally_return_value = tokenELSE >> block_statements__with_optionally_return_value;
        cond_block__with_optionally_return_value = tokenIF >> if_expression >> body_for_true__with_optionally_return_value >> *false_cond_block_without_else__with_optionally_return_value >> (-body_for_false__with_optionally_return_value);
        cond_block__with_optionally_return_value_and_optionally_bind = cond_block__with_optionally_return_value >> -(tokenLRBIND >> ident >> -index_action);

        cycle_begin_expression = SAME_RULE(expression);
        cycle_end_expression = SAME_RULE(expression);
        cycle_counter = SAME_RULE(ident);
        cycle_counter_lr_init = cycle_begin_expression >> tokenLRBIND >> cycle_counter;
        cycle_counter_init = SAME_RULE(cycle_counter_lr_init);
        cycle_counter_last_value = SAME_RULE(cycle_end_expression);
        cycle_body = tokenDO >> (statement | block_statements);
        forto_cycle = tokenFOR >> cycle_counter_init >> (tokenTO | tokenDOWNTO) >> cycle_counter_last_value >> cycle_body;

        statement_in_while_and_if_body = statement | continue_while | break_while;
        block_statements_in_while_and_if_body = tokenBEGINBLOCK >> *statement_in_while_and_if_body >> tokenENDBLOCK;
        while_cycle_head_expression = SAME_RULE(expression);
        while_cycle = tokenWHILE >> while_cycle_head_expression >> block_statements_in_while_and_if_body;

        input = tokenGET >> (ident >> -index_action | tokenGROUPEXPRESSIONBEGIN >> ident >> -index_action >> tokenGROUPEXPRESSIONEND);
        output = tokenPUT >> expression;
        statement = bind_left_to_right | cond_block__with_optionally_return_value_and_optionally_bind | forto_cycle | input | output | tokenSEMICOLON;
        block_statements = tokenBEGINBLOCK >> *statement >> tokenENDBLOCK;
        block_statements__with_optionally_return_value = tokenBEGINBLOCK >> *statement_in_while_and_if_body >> -expression >> tokenENDBLOCK;
        program = BOUNDARIES >> tokenNAME >> program_name >> tokenSEMICOLON >> tokenDATA >> (-declaration) >> tokenSEMICOLON >> tokenBEGIN >> *statement >> tokenEND;

        digit = digit_0 | digit_1 | digit_2 | digit_3 | digit_4 | digit_5 | digit_6 | digit_7 | digit_8 | digit_9;
        non_zero_digit = digit_1 | digit_2 | digit_3 | digit_4 | digit_5 | digit_6 | digit_7 | digit_8 | digit_9;
        unsigned_value = ((non_zero_digit >> *digit) | digit_0) >> BOUNDARIES;
        value = (-sign) >> unsigned_value >> BOUNDARIES;
        letter_in_lower_case = a | b | c | d | e | f | g | h | i | j | k | l | m | n | o | p | q | r | s | t | u | v | w | x | y | z;
        letter_in_upper_case = A | B | C | D | E | F | G | H | I | J | K | L | M | N | O | P | Q | R | S | T | U | V | W | X | Y | Z;
        ident = tokenUNDERSCORE >> letter_in_lower_case >> letter_in_lower_case >> digit >> STRICT_BOUNDARIES;

        sign = sign_plus | sign_minus;
        sign_plus = SAME_RULE(tokenPLUS);
        sign_minus = SAME_RULE(tokenMINUS);

        digit_0 = '0';
        digit_1 = '1';
        digit_2 = '2';
        digit_3 = '3';
        digit_4 = '4';
        digit_5 = '5';
        digit_6 = '6';
        digit_7 = '7';
        digit_8 = '8';
        digit_9 = '9';

        tokenCOLON = ":" >> BOUNDARIES;
        tokenINTEGER16_t = "int16_t" >> STRICT_BOUNDARIES;
        tokenCOMMA = "," >> BOUNDARIES;
        tokenNOT = "!" >> STRICT_BOUNDARIES;
        tokenAND = "and" >> STRICT_BOUNDARIES;
        tokenOR = "|" >> STRICT_BOUNDARIES;
        tokenEQUAL = "eg" >> BOUNDARIES;
        tokenNOTEQUAL = "ne" >> BOUNDARIES;
        tokenLESSOREQUAL = "<=" >> BOUNDARIES;
        tokenGREATEROREQUAL = ">=" >> BOUNDARIES;
        tokenPLUS = "+" >> BOUNDARIES;
        tokenMINUS = "-" >> BOUNDARIES;
        tokenMUL = "*" >> BOUNDARIES;
        tokenDIV = "/" >> STRICT_BOUNDARIES;
        tokenMOD = "%" >> STRICT_BOUNDARIES;
        tokenGROUPEXPRESSIONBEGIN = "(" >> BOUNDARIES;
        tokenGROUPEXPRESSIONEND = ")" >> BOUNDARIES;
        tokenLRBIND = ":>" >> BOUNDARIES;
        tokenELSE = "else" >> STRICT_BOUNDARIES;
        tokenIF = "if" >> STRICT_BOUNDARIES;
        tokenDO = "do" >> STRICT_BOUNDARIES;
        tokenFOR = "for" >> STRICT_BOUNDARIES;
        tokenDOWNTO = "downto" >> STRICT_BOUNDARIES;
        tokenGET = "get" >> STRICT_BOUNDARIES;
        tokenPUT = "put" >> STRICT_BOUNDARIES;
        tokenNAME = "program" >> STRICT_BOUNDARIES;
        tokenDATA = "data" >> STRICT_BOUNDARIES;
        tokenBEGIN = "start" >> STRICT_BOUNDARIES;
        tokenEND = "finish" >> STRICT_BOUNDARIES;
        tokenBEGINBLOCK = "{" >> BOUNDARIES;
        tokenENDBLOCK = "}" >> BOUNDARIES;
        tokenLEFTSQUAREBRACKETS = "[" >> BOUNDARIES;
        tokenRIGHTSQUAREBRACKETS = "]" >> BOUNDARIES;
        tokenSEMICOLON = ";" >> BOUNDARIES;

        STRICT_BOUNDARIES = (BOUNDARY >> *(BOUNDARY)) | (!(qi::alpha | qi::char_("_")));
        BOUNDARIES = (BOUNDARY >> *(BOUNDARY) | NO_BOUNDARY);
        BOUNDARY = BOUNDARY_SPACE | BOUNDARY_TAB | BOUNDARY_CARRIAGE_RETURN | BOUNDARY_LINE_FEED | BOUNDARY_NULL;
        BOUNDARY_SPACE = " ";
        BOUNDARY_TAB = "\t";
        BOUNDARY_CARRIAGE_RETURN = "\r";
        BOUNDARY_LINE_FEED = "\n";
        BOUNDARY_NULL = "\0";
        NO_BOUNDARY = "";

        tokenUNDERSCORE = "_";

        A = "A"; B = "B"; C = "C"; D = "D"; E = "E"; F = "F"; G = "G"; H = "H"; I = "I"; J = "J";
        K = "K"; L = "L"; M = "M"; N = "N"; O = "O"; P = "P"; Q = "Q"; R = "R"; S = "S"; T = "T";
        U = "U"; V = "V"; W = "W"; X = "X"; Y = "Y"; Z = "Z";

        a = "a"; b = "b"; c = "c"; d = "d"; e = "e"; f = "f"; g = "g"; h = "h"; i = "i"; j = "j";
        k = "k"; l = "l"; m = "m"; n = "n"; o = "o"; p = "p"; q = "q"; r = "r"; s = "s"; t = "t";
        u = "u"; v = "v"; w = "w"; x = "x"; y = "y"; z = "z";
    }

    std::ostringstream& error_stream_;

    qi::rule<Iterator>
        labeled_point, goto_label, program_name, value_type, declaration_element, other_declaration_ident, declaration,
        index_action, unary_operator, unary_operation, binary_operator, binary_action, left_expression, expression,
        group_expression, bind_left_to_right, if_expression, body_for_true, body_for_true__with_optionally_return_value,
        false_cond_block_without_else__with_optionally_return_value, body_for_false__with_optionally_return_value,
        cond_block__with_optionally_return_value, cond_block__with_optionally_return_value_and_optionally_bind,
        cycle_begin_expression, cycle_end_expression, cycle_counter, cycle_counter_lr_init, cycle_counter_init,
        cycle_counter_last_value, cycle_body, forto_cycle, while_cycle_head_expression, while_cycle,
        continue_while, break_while, statement_in_while_and_if_body, block_statements_in_while_and_if_body,
        repeat_until_cycle_cond, repeat_until_cycle, input, output, statement, block_statements,
        block_statements__with_optionally_return_value, program,
        tokenCOLON, tokenGOTO, tokenINTEGER16_t, tokenCOMMA, tokenNOT, tokenAND, tokenOR, tokenEQUAL, tokenNOTEQUAL,
        tokenLESSOREQUAL, tokenGREATEROREQUAL, tokenLESS, tokenGREATER, tokenPLUS, tokenMINUS, tokenMUL, tokenDIV,
        tokenMOD, tokenGROUPEXPRESSIONBEGIN, tokenGROUPEXPRESSIONEND, tokenLRBIND, tokenELSE, tokenIF, tokenDO,
        tokenFOR, tokenTO, tokenDOWNTO, tokenWHILE, tokenCONTINUE, tokenBREAK, tokenEXIT, tokenREPEAT, tokenUNTIL,
        tokenGET, tokenPUT, tokenNAME, tokenBODY, tokenDATA, tokenBEGIN, tokenEND, tokenBEGINBLOCK, tokenENDBLOCK,
        tokenLEFTSQUAREBRACKETS, tokenRIGHTSQUAREBRACKETS, tokenSEMICOLON,
        STRICT_BOUNDARIES, BOUNDARIES, BOUNDARY, BOUNDARY_SPACE, BOUNDARY_TAB, BOUNDARY_CARRIAGE_RETURN,
        BOUNDARY_LINE_FEED, BOUNDARY_NULL, NO_BOUNDARY,
        sign, sign_plus, sign_minus, digit_0, digit_1, digit_2, digit_3, digit_4, digit_5, digit_6, digit_7,
        digit_8, digit_9, digit, non_zero_digit, value, unsigned_value, letter_in_upper_case, letter_in_lower_case,
        ident, tokenUNDERSCORE,
        a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z,
        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z;
};

size_t loadSource(char** text, char* fileName) {
    if (!fileName) {
        printf("No input file name\r\n");
        return 0;
    }

    FILE* file = fopen(fileName, "rb");

    if (file == 0) {
        printf("File not loaded\r\n");
        return 0;
    }

    fseek(file, 0, 2);
    long fileSize_ = ftell(file);
    if (fileSize_ >= 8192) {
        printf("the file(%ld bytes) is larger than %d bytes\r\n", fileSize_, 8192);
        fclose(file);
        exit(2);
    }
    size_t fileSize = fileSize_;
    rewind(file);

    if (!text) {
        printf("Load source error\r\n");
        return 0;
    }
    *text = (char*)malloc(sizeof(char) * (fileSize + 1));
    if (*text == 0) {
        fputs("Memory error", (__acrt_iob_func(2)));
        fclose(file);
        exit(2);
    }

    size_t result = fread(*text, sizeof(char), fileSize, file);
    if (result != fileSize) {
        fputs("Reading error", (__acrt_iob_func(2)));
        fclose(file);
        exit(3);
    }
    (*text)[fileSize] = '\0';

    fclose(file);

    return fileSize;
}

#define MAX_LEXEM_SIZE 1024

int commentRemover(char* text, const char* openStrSpc, const char* closeStrSpc) {
    bool eofAlternativeCloseStrSpcType = false;
    bool explicitCloseStrSpc = true;
    if (!strcmp(closeStrSpc, "\n")) {
        eofAlternativeCloseStrSpcType = true;
        explicitCloseStrSpc = false;
    }

    unsigned int commentSpace = 0;

    unsigned int textLength = strlen(text);
    unsigned int openStrSpcLength = strlen(openStrSpc);
    unsigned int closeStrSpcLength = strlen(closeStrSpc);
    if (!closeStrSpcLength) {
        return -1;
    }
    unsigned char oneLevelComment = 0;
    if (!strncmp(openStrSpc, closeStrSpc, MAX_LEXEM_SIZE)) {
        oneLevelComment = 1;
    }

    for (unsigned int index = 0; index < textLength; ++index) {
        if (!strncmp(text + index, closeStrSpc, closeStrSpcLength) && (explicitCloseStrSpc || commentSpace)) {
            if (commentSpace == 1 && explicitCloseStrSpc) {
                for (unsigned int index2 = 0; index2 < closeStrSpcLength; ++index2) {
                    text[index + index2] = ' ';
                }
            }
            else if (commentSpace == 1 && !explicitCloseStrSpc) {
                index += closeStrSpcLength - 1;
            }
            oneLevelComment ? commentSpace = !commentSpace : commentSpace = 0;
        }
        else if (!strncmp(text + index, openStrSpc, openStrSpcLength)) {
            oneLevelComment ? commentSpace = !commentSpace : commentSpace = 1;
        }

        if (commentSpace && text[index] != ' ' && text[index] != '\t' && text[index] != '\r' && text[index] != '\n') {
            text[index] = ' ';
        }
    }

    if (commentSpace && !eofAlternativeCloseStrSpcType) {
        return -1;
    }

    return 0;
}

#include <fstream>

int main(int argc, char* argv[]) {
    char* text_;
    char fileName[128] = DEFAULT_INPUT_FILE;
    char choice[2] = { fileName[0], fileName[1] };
    system("CLS");
    std::cout << "Enter file name(Enter \"" << choice[0] << "\" to use default \"" DEFAULT_INPUT_FILE "\"):";
    std::cin >> fileName;
    if (fileName[0] == choice[0] && fileName[1] == '\0') {
        fileName[1] = choice[1];
    }
    size_t sourceSize = loadSource(&text_, fileName);
    if (!sourceSize) {
#ifdef RERUN_MODE
        (void)getchar();
        printf("\nEnter 'y' to rerun program action(to pass action enter other key): ");
        char valueByGetChar = getchar();
        if (valueByGetChar == 'y' || valueByGetChar == 'Y') {
            system((std::string("\"") + argv[0] + "\"").c_str());
        }
        return 0;
#else
        printf("Press Enter to exit . . .");
        (void)getchar();
        return 0;
#endif
    }
    printf("Original source:\r\n");
    printf("-------------------------------------------------------------------\r\n");
    printf("%s\r\n", text_);
    printf("-------------------------------------------------------------------\r\n\r\n");
    int commentRemoverResult = commentRemover(text_, "#*", "*#");
    if (commentRemoverResult) {
        printf("Comment remover return %d\r\n", commentRemoverResult);
        printf("Press Enter to exit . . .");
        (void)getchar();
        return 0;
    }
    printf("Source after comment removing:\r\n");
    printf("-------------------------------------------------------------------\r\n");
    printf("%s\r\n", text_);
    printf("-------------------------------------------------------------------\r\n\r\n");

    std::string text(text_);

    typedef std::string     str_t;
    typedef str_t::iterator str_t_it;

    std::ostringstream error_stream;
    CW_GRAMMAR<str_t_it> cwg(error_stream);

    str_t_it begin = text.begin(), end = text.end();

    bool success = qi::parse(begin, end, cwg);

    if (!success) {
        std::cout << "\nParsing failed!\n";
        std::cout << "Error message: " << error_stream.str();
    }
    else if (begin != end) {
        std::cout << "\nUnknown fragment after success parse at: \"" << str_t(begin, end) << "\"\n";
    }
    else {
        std::cout << "\nParsing success!\n";
    }

    free(text_);

    (void)getchar();

#ifdef RERUN_MODE
    printf("\nEnter 'y' to rerun program action(to pass action enter other key): ");
    char valueByGetChar = getchar();
    if (valueByGetChar == 'y' || valueByGetChar == 'Y') {
        system((std::string("\"") + argv[0] + "\"").c_str());
    }
#endif

    return 0;
}