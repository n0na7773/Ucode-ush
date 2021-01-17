#include "../inc/libmx.h"

char **mx_strsplit(char const *s, char c) {
	if (!s) return NULL;
	int w_cnt = mx_count_words(s, c);
	char **res = (char **)malloc((w_cnt + 1) * sizeof(char *));
	int j = 0;
	for (int i = 0; i < w_cnt; i++) {
		int w_len = 0;
		while (s[j] != c && s[j]) { j++; w_len++; }
		if(w_len == 0 && j < mx_strlen(s)) { j++; i--; } 
			else {
			char *temp_word = mx_strndup(&s[j - w_len], w_len);
			res[i] = temp_word;
		}
	}
	res[w_cnt] = NULL;
	return res;
}
