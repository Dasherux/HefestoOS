void tolower(char *text) {
	int i = 0;
	while (text[i] != '\0') {
		if (text[i] >= 'A' && text[i] <= 'Z') {
			text[i] = text[i] + 32;
		}
		i++;
	}
}

int strcmp(const char *a, const char *b) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i])
            return 0;
        i++;
    }
    return a[i] == b[i];
}