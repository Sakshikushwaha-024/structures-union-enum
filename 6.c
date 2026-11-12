#include <stdio.h>
#include <string.h>

int main(void) {
  char text[200];
  char pattern[200];

  if ((fgets(text, sizeof(text), stdin) == NULL) || (fgets(pattern, sizeof(pattern), stdin) == NULL)) {
    return 1;
  }

  text[strcspn(text, "\n")] = '\0';
  pattern[strcspn(pattern, "\n")] = '\0';
  size_t text_length = strlen(text);
  size_t pattern_length = strlen(pattern);

  if (pattern_length == 0) {
    return 1;
  }

  int matches = 0;

  for (size_t start = 0; (start + pattern_length) <= text_length; start++) {
    if (strncmp(text + start, pattern, pattern_length) == 0) {
      printf("%zu ", start);
      matches++;
    }
  }

  printf("\nMatches: %d\n", matches);

  return 0;
}