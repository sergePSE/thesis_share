//
// Created by sergei on 02.06.18.
//

#include "html_header_parser.h"

#include <regex.h>
#include <stdlib.h>

#include "mono_parser.h"


// max size for header, usually HTTP/1.1 code code_text \n Redirect url: url(2048 practically), 500 for other headers
#define HEADER_HEAP 2548
#define HTTP_UNTIL_CODE_SIZE 20
#define MIN(A,B) A<B?A:B

int get_code(char* data_pointer, size_t size, error_report_t** error_out)
{
    text_t* header = init_text(data_pointer, MIN(HTTP_UNTIL_CODE_SIZE, size));
    text_t* code_text = parse_first(header->text, "HTTP\\S+ ([0-9]+)", error_out);
    destroy_text(header);
    if (*error_out != NULL)
        return -1;
    int code = atoi(code_text->text);
    destroy_text(code_text);
    return code;
}

text_t* get_redirect_url(char* data_pointer, size_t size, error_report_t** error_out)
{
    text_t* header = init_text(data_pointer, MIN(HEADER_HEAP, size));
    text_t* url = parse_first(header->text, "location:\\s+(\\S+)\r", error_out);
    destroy_text(header);
    if (*error_out != NULL)
        return NULL;
    return url;
}

struct header_info* get_response_header(char* data_pointer, size_t size, error_report_t** error_out)
{
    int code = get_code(data_pointer, size, error_out);
    if (*error_out != NULL)
        return NULL;

    text_t* redirect_url = NULL;
    // 3xx redirection codes
    if (code >= 300 && code < 400)
        redirect_url = get_redirect_url(data_pointer, size, error_out);
    if (*error_out != NULL)
        return NULL;
    struct header_info* header_info = malloc(sizeof(struct header_info));
    header_info->redirect_url = redirect_url;
    header_info->response_code = code;
    return header_info;
}

