#include <stdio.h>
#include "/home/emika/emilibrary/files.h"
#include "/home/emika/emilibrary/strings.h"




typedef enum Block_Type Block_Type;
enum Block_Type {
   title = 0,
   subtitle,
   date,
   triggers,
   foreword,
   location,
   time,
   text,

   incorrect_header
};
char *block_headers[] = {
   "title",
   "subtitle",
   "date",
   "triggers",
   "foreword",
   "location",
   "time",
   "text"
};


bool add_block(String source, u64 *index_p, String *result_p);


int main(int argc, char *argv[]) {
   if(argc != 2) {
      printf("provide one file\n");
      return 0;
   }

   Arena *arena = arena_create(0x10000);
   String source = file_load(argv[1], arena);


   String result;
   result.str = malloc(source.len * 2 + 10000);
   result.len = 0;

   result = string_append(result, str("<!DOCTYPE html>\n<html lang=\"en-UK\">\n\n\n<head>\n<title>Emika's personal website</title>\n<link rel=\"stylesheet\" href=\"style-story.css\">\n<meta charset='utf-8' />\n</head>\n\n\n\n<body>\n\n<div class=\"block\">\n\n\n"));
   for(u64 i=0; i<source.len; i++) {
      if(source.str[i] == '[') {
         add_block(source, &i, &result);
      }
   }
   result = string_append(result, str("</div>\n\n<div class=\"block\">\n<p><a href=\"contents.html\">return to contents</a></p>\n<p><a href=\"../index.html\">return home</a></p>\n</div>\n\n\n\n</body>\n\n</html>"));










   string_println(result);

   return 0;
}


/* returns true on success, false on failure */
bool add_block(String source, u64 *index_p, String *result_p) {
   String result = *result_p;
   u64 start = string_find_next(source, *index_p, '\n')+1;
   u64 end = string_find_next(source, start, ']');


   String header = substring_word(source, *index_p+1);
   Block_Type type = incorrect_header;

   for(u32 i=0; i<sizeof(block_headers) / sizeof(*block_headers); i++) {
      if(string_equals(header, str(block_headers[i]))) {
         type = i;
      }
   }

   if(type == incorrect_header) {
      printf("incorrect header: %.*s\n", header.len, header.str);
      return false;
   }



   switch(type) {
      case title:
         result = string_append(result, str("<h1 id=\"title\">"));
         result = string_append(result, substring_create(source, start, end));
         result = string_append(result, str("</h1>\n\n"));
         break;

      case subtitle:
         // result = string_append(result, str("<h3>\"or:\"</h3>\n<h2 id=\"subtitle\">"));
         result = string_append(result, str("<h3 style=\"font-size: 12px; margin-top: 0px; text-align: center;\">or:</h3>\n<h2 id=\"subtitle\">"));
         result = string_append(result, substring_create(source, start, end));
         result = string_append(result, str("</h2>\n\n"));
         break;

      case date:
         result = string_append(result, str("<h6 id=\"date\">"));
         result = string_append(result, substring_create(source, start, end));
         result = string_append(result, str("</h6>\n\n"));
         break;

      case triggers:
         result = string_append(result, str("<h2 id=\"triggers\">TRIGGERS: "));
         result = string_append(result, substring_create(source, start, end));
         result = string_append(result, str("</h2>\n\n"));
         break;

      case foreword:
         result = string_append(result, str("<details>\n<summary><h2 id=\"foreword\">foreword</h2></summary>\n<p id=\"foreword_paragraph\">"));

         bool is_italic = false;

         for(u64 i=start; i<end; i++) {
            if(source.str[i] == '\n') {
               if(source.str[i+1] == '\n') {
                  result = string_append(result, str("</p>\n<hr class=\"paragraph-split\">\n<p id=\"foreword_paragraph\">"));
                  i++;
               } else {
                  result = string_append(result, str("</p>\n<p id=\"foreword_paragraph\">"));
               }
            } else if(source.str[i] == '*') {
               result = string_append(result, is_italic ? str("</i>") : str("<i>"));
               is_italic = !is_italic;
            } else {
               result.str[result.len++] = source.str[i];
            }
         }

         result = string_append(result, str("</p>\n</details>\n\n"));
         break;

      case location:
         result = string_append(result, str("<h3 id=\"location\">"));
         result = string_append(result, substring_create(source, start, end));
         result = string_append(result, str("</h3>\n\n"));
         break;

      case time:
         result = string_append(result, str("<h3 id=\"time\">"));
         result = string_append(result, substring_create(source, start, end));
         result = string_append(result, str("</h3>\n\n"));
         break;

      case text:
         
         result = string_append(result, str("<p>"));

         for(u64 i=start; i<end; i++) {
            if(source.str[i] == '\n') {
               if(source.str[i+1] == '\n') {
                  result = string_append(result, str("</p>\n<hr class=\"paragraph-split\">\n<p>"));
                  i++;
               } else {
                  result = string_append(result, str("</p>\n<p>"));
               }
            } else if(source.str[i] == '*') {
               result = string_append(result, is_italic ? str("</i>") : str("<i>"));
               is_italic = !is_italic;
            } else {
               result.str[result.len++] = source.str[i];
            }
         }

         result = string_append(result, str("</p>\n\n"));
         break;

      default:
         result = string_append(result, str("<p>"));
         result = string_append(result, substring_create(source, start, end));
         result = string_append(result, str("</p>"));
         result = string_append(result, str("\n\n"));
         break;

   }

   *index_p = end+1;
   *result_p = result;

   return true;

}

